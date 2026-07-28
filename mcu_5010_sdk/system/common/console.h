/*!
 * @file console.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.6, typedefs that indicate size and signedness should be used in place of
 * the basic numerical types
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 21.1, #define and #undef shall not be used on a reserved identifier or
 * reserved macro name.
 * This is exactly its intended purpose: to provide a thread-safe errno implementation.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.9, A function should be used in preference to a function-like macro where they are interchangeable.
 * Use macro to speed up program processing
 *
 */

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#if defined (USE_GCC)

#ifdef MSG_INFO
#define INFO	sy_printf
#else
#define INFO(...)
#endif

#ifdef ERR_INFO
#define ERR 	sy_printf
#else
#define ERR(...)
#endif


#elif defined (USE_GHS)

#ifdef MSG_INFO
#define INFO	(void)sy_printf
#else
#define INFO(...)
#endif

#ifdef ERR_INFO
#define ERR 	(void)sy_printf
#else
#define ERR(...)
#endif

#else

#ifdef MSG_INFO
#define INFO(...) 	(void)printf(__VA_ARGS__)
#else
#define INFO(...)
#endif

#ifdef ERR_INFO
#define ERR(...) 	(void)printf(__VA_ARGS__)
#else
#define ERR(...)
#endif

#endif


void print_array(const char *log_header, void *data, uint32_t len);


#endif//_CONSOLE_H_

