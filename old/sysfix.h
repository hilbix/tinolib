/* $Header$
 *
 * Various system fixes I don't want to do all the time again and again
 *
 * $Log$
 * Revision 1.1  2004-07-05 23:52:19  tino
 * added
 *
 */

#ifndef tino_INC_sysfix_h
#define tino_INC_sysfix_h

/* Some define it as __va_copy (pre C99)
 * or va_copy (C99)
 */
#ifdef __va_copy
#ifndef va_copy
#define TINO_VA_COPY	__va_copy
#endif
#endif
#ifndef TINO_VA_COPY
#define TINO_VA_COPY	va_copy
#endif

#endif
