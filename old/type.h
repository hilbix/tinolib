/* $Header$
 *
 * $Log$
 * Revision 1.2  2004-09-04 20:17:23  tino
 * changes to fulfill include test (which is part of unit tests)
 *
 * Revision 1.1  2004/04/18 14:19:49  tino
 * Sigh.  Why are there no globally universal standard data types? (K&R-C)
 */

#ifndef tino_INC_type_h
#define tino_INC_type_h

#include <sys/types.h>

/* I need some signed and unsigned types of which the storage size is
 * definitively known.
 */
typedef u_int8_t	tino_u8_t;
typedef u_int16_t	tino_u16_t;
typedef u_int32_t	tino_u32_t;
typedef u_int64_t	tino_u64_t;
typedef int8_t		tino_i8_t;
typedef int16_t		tino_i16_t;
typedef int32_t		tino_i32_t;
typedef int64_t		tino_i64_t;

#endif
