/* $Header$
 *
 * Manage exceptions !!!UNTESTED!!!
 *
 * If you want to use exceptions with general file IO, then do:
 * #include "tino/file.h"	// don't include something before!
 * #include "tino/exception.h"	// don't include something between!
 * #include ...			// now include the rest
 *
 * This is only a little bit different from other implementations.  I
 * dislike to use exception types, as type checking is error prone in
 * C.  I use error numbers instead, as well as extended pointers under
 * the exception system's control.
 *
 * If you have problems, try to define TINO_TRY_DUMMY and recompile.
 * Exceptions are not working then, however the statements are dummy
 * statements then, such that you can find errors more easily.  If you
 * want to see debug statements, then define TINO_TRY_DEBUG.
 *
 * Hints:
 *
 * Just use one unique number for each throw, such that you can catch
 * exactly what you want.  Associate each number with some additional
 * information, such that you can give feedback to the user what this
 * exception has to say exactly to them.
 *
 * Warning!  Never return from the subrountine within a TRY! Instead use:
 *
 *	TINO_TRY_RETURN(X);
 *
 * Now how to do:
 *
 * TINO_TRY
 *   {
 *     what to try;
 *     break; // never forget this!
 *   }
 * TINO_CATCH(X) TINO_CATCH(Y) ...
 *   {
 *     what to do when this is catched;
 *     break; // without falls through to next catch
 *   }
 * TINO_CATCH_ANY
 *   {
 *     what to catch if not catched anywhere else;
 *     break; // without falls through to next catch
 *   }
 * TINO_FINALLY
 *   {
 *     To execute for cleanup;
 *     Note that this is not executed in case of an rethrow!
 *   }
 * TINO_ENDTRY;
 *
 * Now you can use:
 *
 *	tino_throw(ex, ptr);
 *
 * and within an exception:
 *
 *	TINO_RETHROW;
 *
 * to throw an exception (ex>0) with an associated information pointer
 * (usually something static, as it is not freed).
 *
 * Copyright (C)2004 by Valentin Hilbig
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 * $Log$
 * Revision 1.1  2004-10-10 12:13:17  tino
 * added as idea - BUT IS NOT USABLE YET
 *
 */

#ifndef tino_INC_exception_h
#define tino_INC_exception_h

#include <setjmp.h>

#ifdef TINO_EXIT
#error ""
#else
#define	TINO_EXIT(NR,ARGS)	tino_throw(TINO_EX_##NR, tino_throw_str ARGS)

static void	tino_throw_str(const char *s, ...);
static void	tino_throw(int ex, const void *ptr);

#include "fatal.h"

typedef struct tino_exception_ctx_struct tino_exception_ctx;

struct tino_exception_ctx_struct
  {
    struct tino_exception_ctx_struct	*next;
    const char				*file;
    int					line;
    int					ex;
    const void				*ptr;
    jmp_buf				jmp;
  };

static tino_exception_ctx *tino_exception_ctx_list;

static void
tino_exception_push(tino_exception_ctx *p, const char *file, int line)
{
  p->next			= tino_exception_ctx_list;
  tino_exception_ctx_list	= p;
  p->file			= file;
  p->line			= line;
  p->ex				= 0;
  p->ptr			= 0;
}

static void
tino_exception_pull(tino_exception_ctx *p)
{
  if (tino_exception_ctx_list==p)
    tino_exception_ctx_list	= p->next;
}

/* This routine is designed to be used inside a tino_trow()
 * exclusively.
 */
static const char *
tino_throw_str(const char *s, ...)
{
  static char	*buf;
  char		*tmp;
  int		n;

  n	= BUFSIZ;
  do
    {
      va_list	list;

      tmp	= malloc(n);
      if (!tmp)
	{
	  000;
	}
      va_start(list, s);
      n	= vsnprintf(tmp, s, list);
      va_end(list);
    }
  if 
  ;
}

static void
tino_throw(int ex, const void *ptr)
{
  tino_exception_ctx *tmp;

  if (ex<=0)
    ex	= -2;
  tmp				= tino_exception_ctx_list;
  if (!tmp)
    tino_fatal("unhandled exception %d", ex);
  tino_exception_ctx_list	= tmp->next;
  tmp->ex			= ex;
  tmp->ptr			= ptr;
  longjmp(tino_exception_ctx_list->jmp, ex);
}

#define TINO_TRY						\
  do								\
    {								\
      tino_exception_ctx tino_try_ctx;				\
								\
      tino_exception_push(&tino_try_ctx, __FILE__, __LINE__);	\
      switch (setjmp(tino_try_ctx.jmp))				\
	{							\
	case 0:
	  /* Code to execute comes here	*/

#define TINO_CATCH(X)				\
	case (X):
	  /* reached when an exception is thrown */

#define TINO_CATCH_ANY				\
	default:				\
	  if (tino_try_ctx.exception==-1)	\
	    break;
	  /* reached when any uncatched exception is thrown */

#define TINO_FINALLY				\
	}					\
	{
	  /* end the switch and start the "break" block to clean up */

#define TINO_ENDTRY				\
	}					\
      tino_exception_pull(&tino_try_ctx);	\
    } while (0)

#define TINO_TRY_RETURN(X)					\
	  { tino_exception_pull(&tino_try_ctx); return X; }

#define TINO_RETHROW					\
	  tino_thow(tino_try_ctx.ex, tino_try_ctx.ptr)

#endif
