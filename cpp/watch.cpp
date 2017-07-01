/**
 * Simple digital watch example
 * M. Samek, 01/07/00
 */
#include <assert.h>
#include <stdio.h>
#include "hsm.h"

class Watch : public Hsm {
  int tsec, tmin, thour, dday, dmonth;
protected:
  State timekeeping, time, date;
  State setting, hour, minute, day, month;
  State *timekeepingHist;
public:
  Watch();
  Msg const *topHndlr(Msg const *msg);  
  Msg const *timekeepingHndlr(Msg const *msg);  
  Msg const *timeHndlr(Msg const *msg);  
  Msg const *dateHndlr(Msg const *msg);  
  Msg const *settingHndlr(Msg const *msg);  
  Msg const *hourHndlr(Msg const *msg);  
  Msg const *minuteHndlr(Msg const *msg);  
  Msg const *dayHndlr(Msg const *msg);  
  Msg const *monthHndlr(Msg const *msg);  
  void tick();
  void showTime();
  void showDate();
};

enum WatchEvents {
  Watch_DATE_EVT,
  Watch_SET_EVT,
  Watch_TICK_EVT
};

void Watch::showTime() {
  printf("time: %2d:%02d:%02d", thour, tmin, tsec);
}

void Watch::showDate() {
  printf("date: %02d-%02d", dmonth, dday);
}

void Watch::tick() {
  static int const month[] = { 
    31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 
  };
  if (++tsec == 60) {
    tsec = 0;
    if (++tmin == 60) {
      tmin = 0;
      if (++thour == 24) {
        thour = 0;
        if (++dday == month[dmonth-1]+1) {
          dday = 1;
          if (++dmonth == 13) 
            dmonth = 1;
        }
      }
    }
  }
}

Msg const *Watch::topHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(&setting);
    return 0;
  case Watch_TICK_EVT:
    if (++tsec == 60)
      tsec = 0;
    printf("Watch::top-TICK;");
    showTime();
    return 0;
  } 
  return msg;
}

Msg const *Watch::timekeepingHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(timekeepingHist);
    return 0;
  case Watch_SET_EVT:
    STATE_TRAN(&setting);
    printf("Watch::timekeeping-SET;");
    return 0;
  } 
  return msg;
}

Msg const *Watch::timeHndlr(Msg const *msg) {
  switch (msg->evt) {
  case ENTRY_EVT:
    showTime();
    return 0;
  case Watch_DATE_EVT:
    STATE_TRAN(&date);
    printf("Watch::time-DATE;");        
    return 0;
  case Watch_TICK_EVT:
    printf("Watch::time-TICK;");        
    tick();
    showTime();
    return 0;
  } 
  return msg;
}

Msg const *Watch::dateHndlr(Msg const *msg) {
  switch (msg->evt) {
  case ENTRY_EVT:
    showDate();
    return 0;
  case Watch_DATE_EVT:
    STATE_TRAN(&time);
    printf("Watch::date-DATE;");        
    return 0;
  case Watch_TICK_EVT:
    printf("Watch::date-TICK;");        
    tick();
    showDate();
    return 0;
  } 
  return msg;
}

Msg const *Watch::settingHndlr(Msg const *msg) {
  switch (msg->evt) {
  case START_EVT:
    STATE_START(&hour);
    return 0;
  } 
  return msg;
}

Msg const *Watch::hourHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&minute);
    printf("Watch::hour-SET;");
    return 0;
  } 
  return msg;
}

Msg const *Watch::minuteHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&day);
    return 0;
  } 
  return msg;
}

Msg const *Watch::dayHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&month);
    printf("Watch::day-SET;");
    return 0;
  } 
  return msg;
}

Msg const *Watch::monthHndlr(Msg const *msg) {
  switch (msg->evt) {
  case Watch_SET_EVT:
    STATE_TRAN(&timekeeping);
    printf("Watch::month-SET;");
    return 0;
  } 
  return msg;
}

Watch::Watch() 
: Hsm("Watch", (EvtHndlr)topHndlr),
  timekeeping("timekeeping", &top, 
       (EvtHndlr)&Watch::timekeepingHndlr),
  time("time", &timekeeping, (EvtHndlr)&Watch::timeHndlr),
  date("date", &timekeeping, (EvtHndlr)&Watch::dateHndlr),
  setting("setting", &top, (EvtHndlr)&Watch::settingHndlr),
  hour("hour", &setting, (EvtHndlr)&Watch::hourHndlr),
  minute("minute", &setting, (EvtHndlr)&Watch::minuteHndlr),
  day("day", &setting, (EvtHndlr)&Watch::dayHndlr),
  month("month", &setting, (EvtHndlr)&Watch::monthHndlr),
  tsec(0), tmin(0), thour(0), dday(1), dmonth(1)
{
  timekeepingHist = &time; 
}

const Msg watchMsg[] = { 
  Watch_DATE_EVT,
  Watch_SET_EVT,
  Watch_TICK_EVT
};

int main() {
  Watch watch;         
  watch.onStart();
  for (;;)  {
    int i;
    printf("\nEvent<-");
    scanf("%d", &i);
    if (i < 0 || sizeof(watchMsg)/sizeof(Msg) <= i) 
      break;
    watch.onEvent(&watchMsg[i]); 
  }
  return 0;
}
