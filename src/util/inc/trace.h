#ifndef _TRACE_H
#define _TRACE_H

#define DISABLE_TRACE         0
#define NETWORK_TRACE         1
#define UTIL_TRACE            1
#define DISPLAY_TRACE         1
#define GAME_LOGIC_TRACE      1

#define TRACE_PRINT(str, ...)   fprintf(stderr, str "\n", ##__VA_ARGS__)
#define TRACE_NONE              (void)0

/* Trace enabled. */
#if DISABLE_TRACE == 0

#define TRACE(TRACE_TYPE, TRACE_ACTION) \
  do {\
    if ((TRACE_TYPE) == 1) {\
      TRACE_PRINT(#TRACE_TYPE " (%s @ %d)", __FILE__, __LINE__);\
      TRACE_ACTION;\
    }\
  } while(0)

/* Trace disabled. */
#else

#define TRACE(TRACE_TYPE, TRACE_ACTION)   (void) 0

#endif

#endif /* _TRACE_H */
