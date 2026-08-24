/*!
 * @file console.h
 *
 * @page misra_violations MISRA-C:2012 violations
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
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 10.3, The value of an expression shall not be assigne d to an object with a
 * narrower essential type or of a different essential type category.
 * correct functionality is verfied by tests.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 17.8, A function parameter should not be modified.
 * correct functionality is verfied by tests.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 15.6, The body of an iteration-statement or a selection-statement shall be a
 * compound-statement.
 * correct functionality is verfied by tests.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 4.6, typedefs that indicate size and signedness should be used in place of
 * the basic numerical types.
 * For structure convenience, basic type names are used.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 10.4, Both operands of an operator in which the usual arithmetic conversions
 * are performed shall have the same essential type category.
 * correct functionality is verfied by tests.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 10.1, Operands shall not be of an inappropriate essential type.
 * correct functionality is verfied by tests.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Dir 4.5, Identifiers in the same name space with overlapping visibility should
 * be typographically unambiguous.
 * Name is easy to understant.
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 20.10, The # and ## preprocessor operators should not be used.
 * The operator is used to define dynamic implementation of API functions of signal interaction.
 *This way is more efficient
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 21.6, The Standard Library input/output functions shall not be used.
 * This way is more efficient
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.11, When an array with external linkage is declared, its size should be
 * explicitly specified.
 * This way is more efficient
 *
 */

#ifndef CTYPE_H
#define CTYPE_H

/*
 * NOTE! This ctype does not handle EOF like the standard C
 * library is required to.
 */

#define _U	0x01	/* upper */
#define _L	0x02	/* lower */
#define _D	0x04	/* digit */
#define _C	0x08	/* cntrl */
#define _P	0x10	/* punct */
#define _S	0x20	/* white space (space/lf/tab) */
#define _X	0x40	/* hex digit */
#define _SP	0x80	/* hard space (0x20) */

extern const unsigned char _ctype[];

#define __ismask(x) (_ctype[(int)(unsigned char)(x)])

#define isalnum(c)	((__ismask(c)&(_U|_L|_D)) != 0)
#define isalpha(c)	((__ismask(c)&(_U|_L)) != 0)
#define iscntrl(c)	((__ismask(c)&(_C)) != 0)
//#define isdigit(c)	((__ismask(c)&(_D)) != 0)
#define isgraph(c)	((__ismask(c)&(_P|_U|_L|_D)) != 0)
#define islower(c)	((__ismask(c)&(_L)) != 0)
#define isprint(c)	((__ismask(c)&(_P|_U|_L|_D|_SP)) != 0)
#define ispunct(c)	((__ismask(c)&(_P)) != 0)
#define isspace(c)	((__ismask(c)&(_S)) != 0)
#define isupper(c)	((__ismask(c)&(_U)) != 0)
#define isxdigit(c)	((__ismask(c)&(_D|_X)) != 0)

/*
 * Rather than doubling the size of the _ctype lookup table to hold a 'blank'
 * flag, just check for space or tab.
 */
#define isblank(c)	(c == ' ' || c == '\t')

#define isascii(c) (((unsigned char)(c))<=0x7f)
#define toascii(c) (((unsigned char)(c))&0x7f)

static inline unsigned char __tolower(unsigned char c)
{
	if (isupper(c))
		c -= 'A'-'a';
	return c;
}

static inline unsigned char __toupper(unsigned char c)
{
	if (islower(c))
		c -= 'a'-'A';
	return c;
}

#define tolower(c) __tolower(c)
#define toupper(c) __toupper(c)

#endif//CTYPE_H

