/** hsm.h -- Hierarchical State Machine interface
 */
#ifndef hsm_h
#define hsm_h

typedef int Event;
typedef struct {
    Event evt;
} Msg;

typedef struct Hsm Hsm;
typedef Msg const *(*EvtHndlr)(Hsm*, Msg const*);

typedef struct State State;
struct State {
    State *super;                                  /* pointer to superstate */
    EvtHndlr hndlr;                             /* state's handler function */
    char const *name;
};

void StateCtor(State *me, char const *name, State *super, EvtHndlr hndlr);
#define StateOnEvent(me_, ctx_, msg_) \
    (*(me_)->hndlr)((ctx_), (msg_))

struct Hsm {                       /* Hierarchical State Machine base class */
    char const *name;                             /* pointer to static name */
    State *curr;                                           /* current state */
    State *next;                  /* next state (non 0 if transition taken) */
    State *source;                   /* source state during last transition */
    State top;                                     /* top-most state object */
};

void HsmCtor(Hsm *me, char const *name, EvtHndlr topHndlr);
void HsmOnStart(Hsm *me);                  /* enter and start the top state */
void HsmOnEvent(Hsm *me, Msg const *msg);                   /* "HSM engine" */

/* protected: */
unsigned char HsmToLCA_(Hsm *me, State *target);
void HsmExit_(Hsm *me, unsigned char toLca);
                                                       /* get current state */
#define STATE_CURR(me_) (((Hsm *)me_)->curr)
                     /* take start transition (no states need to be exited) */
#define STATE_START(me_, target_) (((Hsm *)me_)->next = (target_))
                     /* take a state transition (exit states up to the LCA) */
#define STATE_TRAN(me_, target_) if (1) { \
    static unsigned char toLca_ = 0xFF; \
    assert(((Hsm *)me_)->next == 0); \
    if (toLca_ == 0xFF) \
        toLca_ = HsmToLCA_((Hsm *)(me_), (target_)); \
    HsmExit_((Hsm *)(me_), toLca_); \
    ((Hsm *)(me_))->next = (target_); \
} else ((void)0)

#define START_EVT ((Event)(-1))
#define ENTRY_EVT ((Event)(-2))
#define EXIT_EVT  ((Event)(-3))

#endif /* hsm_h */
