/* Some generic data types for the case, you need types of certain width.
 * This can be easily adjusted here if needed.
 *
 * This is release early code.  Use at own risk.
 *
 * This Works is placed under the terms of the Copyright Less License,
 * see file COPYRIGHT.CLL.  USE AT OWN RISK, ABSOLUTELY NO WARRANTY.
 */

#ifndef tino_INC_type_h
#define tino_INC_type_h

#include <stdint.h>

/* I need some signed and unsigned types of which the storage size is
 * definitively known.
 */
typedef uint8_t		tino_u8_t;
typedef uint16_t	tino_u16_t;
typedef uint32_t	tino_u32_t;
typedef uint64_t	tino_u64_t;
typedef int8_t		tino_i8_t;
typedef int16_t		tino_i16_t;
typedef int32_t		tino_i32_t;
typedef int64_t		tino_i64_t;

#endif
