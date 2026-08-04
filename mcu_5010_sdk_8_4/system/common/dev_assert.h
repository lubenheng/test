/*!
 * @file dev_assert.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.9, A function should be used in preference to a function-like macro where they are interchangeable.
 * Use macro to speed up program processing
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 20.10, The # and ## preprocessor operators should not be used.
 * The operator is used to define dynamic implementation of API functions of signal interaction.
 *This way is more efficient
 *
 */

#ifndef DEVASSERT_H
#define DEVASSERT_H


#if defined (CUSTOM_DEVASSERT)
	/* If the CUSTOM_DEVASSERT symbol is defined, then add the custom implementation */
	#include CUSTOM_DEVASSERT
#elif defined (DEV_ERROR_DETECT)
    #define DEV_ASSERT(exp)  \
    if (exp) { ; } \
    else  {\
        ERR("Assert failed: (%s),file %s line %d\n", #exp, __FILE__, (int) __LINE__);\
    while (true){ ; }\
  }
#else
    /* Assert macro does nothing */
    //#define DEV_ASSERT(exp) ((void)0)
    #define DEV_ASSERT(exp) ((void)(exp))
#endif

#endif /* DEVASSERT_H */

