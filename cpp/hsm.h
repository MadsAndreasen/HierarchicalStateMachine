/** hsm.h -- Hierarchical State Machine interface
 */
#ifndef hsm_h
#define hsm_h

typedef int Event;
struct Msg {
    Event evt;
};

class Hsm;                                           /* forward declaration */
typedef Msg const *(Hsm::*EvtHndlr)(Msg const *);

class State {
    State *super;                                  /* pointer to superstate */
    EvtHndlr hndlr;                             /* state's handler function */
    char const *name;
  public:
    State(char const *name, State *super, EvtHndlr hndlr);
  private:
    Msg const *onEvent(Hsm *ctx, Msg const *msg) {
        return (ctx->*hndlr)(msg);
    }
    friend class Hsm;
};

class Hsm {                        /* Hierarchical State Machine base class */
    char const *name;                             /* pointer to static name */
    State *curr;                                           /* current state */
protected:
    State *next;                  /* next state (non 0 if transition taken) */
    State *source;                   /* source state during last transition */
    State top;                                     /* top-most state object */
public:
    Hsm(char const *name, EvtHndlr topHndlr);                       /* Ctor */
    void onStart();                        /* enter and start the top state */
    void onEvent(Msg const *msg);                 /* "state machine engine" */
protected:
    unsigned char toLCA_(State *target);
    void exit_(unsigned char toLca);
    State *STATE_CURR() { return curr; }
    void STATE_START(State *target) {
        assert(next == 0);
        next = target;
    }
# define STATE_TRAN(target_) if (1) { \
    static unsigned char toLca_ = 0xFF; \
    assert(next == 0); \
    if (toLca_ == 0xFF) \
        toLca_ = toLCA_(target_); \
    exit_(toLca_); \
    next = (target_); \
} else ((void)0)
}; 

#define START_EVT ((Event)(-1))
#define ENTRY_EVT ((Event)(-2))
#define EXIT_EVT  ((Event)(-3))

#endif /* hsm_h */
