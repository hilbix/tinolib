/* $Header$
 *
 * Fatal error handlers
 *
 * Copyright (C)2004-2005 Valentin Hilbig, webmaster@scylla-charybdis.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Log$
 * Revision 1.12  2007-08-08 11:26:13  tino
 * Mainly tino_va_arg changes (now includes the format).
 * Others see ChangeLog
 *
 * Revision 1.11  2006/10/04 02:00:56  tino
 * changed va_* routines to tino_va_* routines
 *
 * Revision 1.10  2006/01/24 22:41:26  tino
 * see changelog (changes for socklinger)
 *
 * Revision 1.9  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.8  2005/04/10 00:36:22  tino
 * TINO_FATAL_IF
 *
 * Revision 1.7  2005/01/26 10:51:57  tino
 * Changes for updated exception.h
 *
 * Revision 1.6  2005/01/25 22:14:51  tino
 * exception.h now passes include test (but is not usable).  See ChangeLog
 *
 * Revision 1.5  2004/03/28 00:08:21  tino
 * Some more added, bic2sql works now
 *
 * Revision 1.4  2004/03/26 20:23:35  tino
 * still starting fixes
 *
 * Revision 1.3  2004/03/26 20:17:50  tino
 * More little changes
 *
 * Revision 1.2  2004/03/26 20:06:37  tino
 * dirty mode and name fixes
 *
 * Revision 1.1  2004/03/23 21:19:51  tino
 * Scratch area
 */

#ifndef tino_INC_fatal_h
#define tino_INC_fatal_h

#include "ex.h"

/* This can be overwritten by exception.h
 */
#ifndef TINO_FATAL
#define	TINO_FATAL(X)	TINO_ERROR_PREFIX(fatal,X)
#define	TINO_VFATAL(X)	TINO_ERROR_PREFIX(vfatal,X)
#endif

/* This is never overwritten, it really terminates.
 */
#define	tino_FATAL(X)	do { if (X) { tino_fatal_gen(#X, __FILE__, __LINE__, __FUNCTION__); } } while(0)
#define TINO_FATAL_IF(X)	do { if (X) { TINO_FATAL(("condition: %s", #X)); } } while (0)

static void
tino_pvfatal(const char *t, TINO_VA_LIST list)
{
  tino_verror(t, list, 0);
  TINO_ABORT(-2);
}

static void
tino_vfatal(TINO_VA_LIST list)
{
  tino_pvfatal("fatal error", list);
}

static void
tino_fatal(const char *s, ...)
{
  tino_va_list	list;

  tino_va_start(list, s);
  tino_vfatal(&list);
}

static void
tino_fatal_gen(const char *s, const char *file, int line, const char *function)
{
  tino_fatal("%s:%d:%s: %s", file, line, function, s);
}

#endif
