#ifndef FRAMEWORK_INCLUDE_UTILS_HPP_
#define FRAMEWORK_INCLUDE_UTILS_HPP_

#include <assert.h>
#include <stdint.h>

#ifdef DISPLAY_SYSTEM_ERROR

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

#define CHECKERROR(ret) ({ __typeof__ (ret) errornum = (ret);         \
                       if (__builtin_expect(errornum != 0, 0))    \
                         __assert_perror_fail (errornum, __FILE__, __LINE__, __func__);})

#else
#define CHECKERROR(ret) ({ __typeof__ (ret) errornum = (ret);         \
                       assert(errornum == 0); (void) errornum;})
#endif

template <typename T>
int convert(char buf[], T value);

int convertHex(char buf[], uintptr_t value);

#endif
