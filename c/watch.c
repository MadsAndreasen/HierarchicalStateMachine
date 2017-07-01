/**
 * Simple digital watch example
 * M. Samek, 01/07/00
 */
#include <assert.h>
#include <stdio.h>
#include "hsm.h"

typedef struct Watch Watch;
struct Watch {
  Hsm super;
  State timekeeping, time, date;
  State setting, hour, minute, day, month;
  State *timekeepingHist;
  int tsec, tmin, thour, dday, dmonth;
};

enum WatchEvents {
  Watch_DATE_EVT,
  Watch_SET_EVT,
  Watch_TICK_EVT
};

void WatchShowTime(Watch *me) {
  printf("time: %2d:%02d:%02d", 
  me->thour, me->tmin, me->tsec);
}

void WatchShowDate(Watch *me) {
    printf("date: %02d-%02d", me->dmonth, me->dday);
}

void WatchTick(Watch *me) {
  static int const month[] = { 
     31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 
  };
  if (++me->tsec == 60) {
    me->tsec = 0;
    if (++me->tmin == 60) {
      me->tmin = 0;
      if (++me->thour == 24) {
        me->thour = 0;
        if (++me->dday == month[me->dmonth-1]+1) {
          me->dday = 1;
          if (++me->dmonth == 13) 
            me->dmonth = 1;
        }
      }
    }
  }
}

Msg const *Watch_top(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(me, &me->setting);
    return 0;
  case Watch_TICK_EVT:
    if (++me->tsec == 60)
      me->tsec = 0;
    printf("Watch::top-TICK;");
    WatchShowTime(me);
    return 0;
  } 
  return msg;
}

Msg const *Watch_timekeeping(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(me, me->timekeepingHist);
    return 0;
  case Watch_SET_EVT:
    STATE_TRAN(me, &me->setting);
    printf("Watch::timekeeping-SET;");
    return 0;
  case EXIT_EVT:
    me->timekeepingHist = STATE_CURR(me);
    return 0;
  } 
  return msg;
}

Msg const *Watch_time(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case ENTRY_EVT:
    WatchShowTime(me);
    return 0;
  case Watch_DATE_EVT:
    STATE_TRAN(me, &me->date);
    printf("Watch::time-DATE;");        
    return 0;
  case Watch_TICK_EVT:
    printf("Watch::time-TICK;");        
    WatchTick(me);
    WatchShowTime(me);
    return 0;
  } 
  return msg;
}

Msg const *Watch_date(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case ENTRY_EVT:
    WatchShowDate(me);
    return 0;
  case Watch_DATE_EVT:
    STATE_TRAN(me, &me->time);
    printf("Watch::date-DATE;");        
    return 0;
  case Watch_TICK_EVT:
    printf("Watch::date-TICK;");        
    WatchTick(me);
    WatchShowDate(me);
    return 0;
  } 
  return msg;
}

Msg const *Watch_setting(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(me, &me->hour);
    return 0;
  } 
  return msg;
}

Msg const *Watch_hour(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(me, &me->minute);
    printf("Watch::hour-SET;");
    return 0;
  } 
  return msg;
}

Msg const *Watch_minute(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(me, &me->day);
    return 0;
  } 
  return msg;
}

Msg const *Watch_day(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(me, &me->month);
    printf("Watch::day-SET;");
    return 0;
  } 
  return msg;
}

Msg const *Watch_month(Watch *me, Msg *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(me, &me->timekeeping);
    printf("Watch::month-SET;");
    return 0;
  } 
  return msg;
}

void WatchCtor(Watch *me) {
  HsmCtor((Hsm *)me, "Watch", (EvtHndlr)Watch_top);
  StateCtor(&me->timekeeping, "timekeeping", 
            &((Hsm *)me)->top, (EvtHndlr)Watch_timekeeping);
    StateCtor(&me->time, "time", &me->timekeeping, 
              (EvtHndlr)Watch_time);
    StateCtor(&me->date, "date", &me->timekeeping, 
              (EvtHndlr)Watch_date);
  StateCtor(&me->setting, "setting", &((Hsm *)me)->top, 
            (EvtHndlr)Watch_setting);
    StateCtor(&me->hour, "hour", &me->setting, 
              (EvtHndlr)Watch_hour);
    StateCtor(&me->minute, "minute", &me->setting, 
              (EvtHndlr)Watch_minute);
    StateCtor(&me->day, "day", &me->setting, 
              (EvtHndlr)Watch_day);
    StateCtor(&me->month, "month", &me->setting, 
              (EvtHndlr)Watch_month);

  me->timekeepingHist = &me->time; 

  me->tsec = me->tmin = me->thour = 0;
  me->dday = me->dmonth = 1;
}

const Msg watchMsg[] = { 
  Watch_DATE_EVT,
  Watch_SET_EVT,
  Watch_TICK_EVT
};

int main() {
  Watch watch;         
  WatchCtor(&watch);
  HsmOnStart((Hsm *)&watch);
  for (;;)  {
    int i;
    printf("\nEvent<-");
      scanf("%d", &i);
      if (i < 0 || sizeof(watchMsg)/sizeof(Msg) <= i) 
        break;
      HsmOnEvent((Hsm *)&watch, &watchMsg[i]); 
  }
  return 0;
}
