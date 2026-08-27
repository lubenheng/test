/*!
 * @file version.h
 *
 * @page misra_violations MISRA-C:2012 violations
 *
 * @section [global]
 * Violates MISRA 2012 Advisory Rule 8.11, When an array with external linkage is declared, its size should be
 * explicitly specified
 * The number of configurations/callback can be zero on the other side C language forbids declaring array of size zero.
 *
 */

#ifndef VERSION_H
#define VERSION_H

extern const char FW_VER[];
extern const char COMPILER_DATE[];
extern const char COMPILER_TIME[];

#endif /*VERSION_H*/

