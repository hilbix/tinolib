/* $Header$
 *
 * Must be included *first*.
 * defines all the "dirty" names I use for my routines.
 * Note that, this way, you can use both, dirty and non-dirty names.
 *
 * $Log$
 * Revision 1.7  2004-10-16 21:48:56  tino
 * dev.h enabled, tino_trim added
 *
 * Revision 1.6  2004/04/13 00:29:12  tino
 * A lot of changes to do compile.  Far from ready yet.
 *
 * Revision 1.5  2004/04/07 02:22:48  tino
 * Prototype for storing data in gff_lib done (untested)
 *
 * Revision 1.4  2004/03/28 00:08:21  tino
 * Some more added, bic2sql works now
 *
 * Revision 1.3  2004/03/26 20:23:35  tino
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

/* The right hand will become inline wrappers in future.  For now we
 * redefine the right side (which will be impossible as soon as
 * tinolib becomes a real library).
 */
#define	tino_uni2prn	uni2prn
#define	tino_alloc	alloc
#define	tino_alloc0	alloc0
#define	tino_strdup	stralloc
#define	tino_strncpy0	strxcpy
#define	tino_xd		xd
#define	tino_exit	ex
#define	tino_fatal	fatal
#define	tino_err	error
#define	tino_verror	verror
#define	tino_realloc	re_alloc
#define	tino_warn	warn
#define	tino_vwarn	vwarn
#define	tino_trim	trim

#define	FATAL(X)	tino_FATAL(X)

#define	tino_global_error_count	global_error_count

#endif
