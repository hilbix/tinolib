/* $Header$
 *
 * If you include this, you know what you are doing:
 *
 * Compile time debugging:
 * DP(("format", args)) prints debug to stderr
 * xDP(("format", args)) prints only if global debugging is on
 *
 * #define TINO_DP_all TINO_DP_ON
 * to enable global debugging
 *
 * #define TINO_DP_main TINO_DP_OFF
 * to disable global debugging (even yours)
 *
 * #define TINO_DP_<module> TINO_DP_ON
 * to activate partial debugging in a certain module
 * (this overrides other defines)
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
 * Revision 1.7  2006-04-11 20:54:43  tino
 * block.h
 *
 * Revision 1.6  2006/02/11 14:36:11  tino
 * 000; is now TINO_XXX;
 *
 * Revision 1.5  2005/12/05 02:11:12  tino
 * Copyright and COPYLEFT added
 *
 * Revision 1.4  2005/12/03 12:48:24  tino
 * sock and buf added
 *
 * Revision 1.3  2005/10/30 03:23:52  tino
 * See ChangeLog
 *
 * Revision 1.2  2004/04/06 04:41:37  tino
 * xml reader
 *
 * Revision 1.1  2004/03/28 01:24:14  tino
 * some more stuff
 */

#ifndef tino_INC_debug_h
#define tino_INC_debug_h

#include <stdio.h>
#include <stdarg.h>

#ifndef	DP
#define	DP	TINO_DP_main
#endif

#ifndef xDP
#define	xDP	TINO_DP_all
#endif

#ifndef TINO_DP_main
#define	TINO_DP_main	TINO_DP_ON
#endif

#ifndef TINO_DP_all
#define	TINO_DP_all	TINO_DP_OFF
#endif

#ifndef	TINO_DP_proc
#define TINO_DP_proc	TINO_DP_all
#endif

#ifndef	TINO_DP_sock
#define TINO_DP_sock	TINO_DP_all
#endif

#ifndef	TINO_DP_buf
#define TINO_DP_buf	TINO_DP_all
#endif

#ifndef	TINO_DP_block
#define TINO_DP_block	TINO_DP_all
#endif

/* This is an ellipsis, marking missing things
 * Define this to "0" to get a compile warning.
 */
#ifndef	TINO_XXX
#define	TINO_XXX
#endif

#define TINO_DP_ON(X)	do { tino_debugprintf X; } while (0)
#define TINO_DP_OFF(X)	do { ; } while (0)

static void
tino_debugprintf(const char *s, ...)
{
  va_list	list;

  fflush(stdout);
  fprintf(stderr, "[");
  va_start(list, s);
  vfprintf(stderr, s, list);
  va_end(list);
  fprintf(stderr, "]\n");
  fflush(stderr);
}

#endif
