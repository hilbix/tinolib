/* $Header$
 *
 * Must be included *first*.
 * defines all the "dirty" names I use for my routines.
 * Note that, this way, you can use both, dirty and non-dirty names.
 *
 * $Log$
 * Revision 1.3  2004-03-26 20:23:35  tino
 * still starting fixes
 *
 * Revision 1.2  2004/03/26 20:17:50  tino
 * More little changes
 *
 * Revision 1.1  2004/03/26 20:06:37  tino
 * dirty mode and name fixes
 */
#ifndef tino_INC_dirty_h
#define tino_INC_dirty_h

#define	tino_alloc	alloc
#define	tino_alloc0	alloc0
#define	tino_strdup	stralloc
#define	tino_strncpy0	strxcpy
#define	tino_xd		xd
#define	tino_exit	ex

#define	FATAL(X)	tino_FATAL(X)

#endif
