#define SET_FLAG(flags, flag) ((flags) |= (flag))
#define CLEAR_FLAG(flags, flag) ((flags) &= ~(flag))
#define HAS_FLAG(flags, flag) ((flags) & (flag))
#define FLAG(flag) (1 << (flag))