#ifndef INCLUDE_CURRENTTHREAD_HPP_
#define INCLUDE_CURRENTTHREAD_HPP_

#include <assert.h>

#ifdef NDEBUG
__BEGIN_DECLS
extern void __assert_perror_fail(
    int errnum,
    const char *file,
    unsigned int line,
    const char *function)
__THROW __attribute__ ((__noreturn__));
__END_DECLS
#endif

#ifdef DISPLAY_SYSTEM_ERROR
#define CHECKERROR(ret) ({ __typeof__ (ret) errornum = (ret);         \
                       if (__builtin_expect(errornum != 0, 0))    \
                         __assert_perror_fail (errornum, __FILE__, __LINE__, __func__);})

#else
#define CHECKERROR(ret) ({ __typeof__ (ret) errornum = (ret);         \
                       assert(errornum == 0); (void) errornum;})
#endif


#endif
