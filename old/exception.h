/* $Header$
 *
 * Manage exceptions !!!UNTESTED!!!
 *
 * If you want to use exceptions with general file IO, then do:
 * #define TINO_FILE_EXCEPTIONS
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
 * Revision 1.8  2006-01-29 17:50:58  tino
 * changes due to strprintf.h
 *
 * Revision 1.7  2005/12/08 01:41:52  tino
 * TINO_VEXIT changed
 *
 * Revision 1.6  2005/03/04 00:51:01  tino
 * typo fixed
 *
 * Revision 1.5  2005/01/26 10:51:27  tino
 * Improvements to sometimes reach usablility
 *
 * Revision 1.4  2005/01/25 22:14:51  tino
 * exception.h now passes include test (but is not usable).  See ChangeLog
 *
 * Revision 1.3  2005/01/04 13:23:49  tino
 * see ChangeLog, mainly changes for "make test"
 *
 * Revision 1.2  2004/10/10 12:44:37  tino
 * exception.h and file.h interaction updated
 *
 * Revision 1.1  2004/10/10 12:13:17  tino
 * added as idea - BUT IS NOT USABLE YET
 */

#ifndef tino_INC_exception_h
#define tino_INC_exception_h

#ifdef tino_INC_file_h
#ifndef tino_INC_file_h_override
#error "#define TINO_FILE_EXCEPTION, include file.h, then exception.h, then other includes"
#endif
#endif

#ifdef TINO_EXIT
#error "include exception.h first"
#endif

#include "strprintf.h"

#include <setjmp.h>
#include <stdarg.h>

#define TINO_EXIT(ARGS)		TINO_THROW(EXIT, ARGS)
#define	TINO_THROW(NR,ARGS)	tino_throw(TINO_EX_##NR, __FILE__,__LINE__,__FUNCTION__, tino_throw_str ARGS)
#define TINO_VEXIT(STR,LIST)	TINO_VTHROW(EXIT,STR,LIST)
#define	TINO_VTHROW(NR,STR,LST)	tino_throw(TINO_EX_##NR, __FILE__,__LINE__,__FUNCTION__, tino_throw_vstr(STR,LST))

enum tino_exceptions
  {
    TINO_EX_NONE,
    TINO_EX_EXIT,
    TINO_EX_ABORT,
    /* more to show up here	*/
    TINO_EX_LAST,
    TINO_EX_F=100000,	/* TINO_EX_Fxxx, free for your use, fatals	*/
    TINO_EX_E=200000,	/* TINO_EX_Exxx, free for your use, errors/other exceptions	*/
  };

static const char *	tino_throw_str(const char *s, ...);
static void		tino_throw(const char *file, int line, const char *fn, int ex, const void *ptr);

#include "str.h"

/* This must be saved in case of threads
 */
typedef struct tino_exception_ctx_struct tino_exception_ctx;

struct tino_exception_pos
  {
    const char				*file;
    int					line;
    const char				*fn;
  };
struct tino_exception_ctx_struct
  {
    struct tino_exception_ctx_struct	*next;
    const char				*file;
    struct tino_exception_pos		pos_try, pos_throw, pos_rethrow;
    int					ex;
    const void				*ptr;
    jmp_buf				jmp;
  };

static tino_exception_ctx *tino_exception_ctx_list;

static void
tino_exception_push(tino_exception_ctx *p, const char *file, int line, const char *fn)
{
  p->next			= tino_exception_ctx_list;
  tino_exception_ctx_list	= p;
  p->pos_try.file		= file;
  p->pos_try.line		= line;
  p->pos_try.fn			= fn;
  p->pos_throw.file		= 0;
  p->pos_rethrow.file		= 0;
  p->ex				= 0;
  p->ptr			= 0;
}

static void
tino_exception_pull(tino_exception_ctx *p)
{
  if (tino_exception_ctx_list==p)
    tino_exception_ctx_list	= p->next;
}

/* This routine is designed to be used inside a tino_throw()
 * exclusively.
 * It caches the arguments into a string buffer,
 * this way we are able to track the throw stack.
 *
 * In future this shall become a huge circular buffer
 * instead of allocated memory.
 */
static const char *
tino_throw_vstr(const char *s, va_list list)
{
  char	*tmp;

  tmp	= tino_str_vprintf_null(s, list);
  if (!tmp)
    {
      /* In this case, some preallocated buffer shall be used,
       * so that exceptions can still be thrown in out of memory cases.
       * In case this buffer runs out, old information shall become
       * truncated, so still
       */
      000;
      tino_pvfatal("fatal error, out of memory in exception processing", s, list);
    }
  return tmp;
}

static const char *
tino_throw_str(const char *s, ...)
{
  va_list	list;
  char		*tmp;

  va_start(list, s);
  tmp	= tino_throw_vstr(s, list);
  va_end(list);
  return tmp;
}

static void
tino_throw(const char *file, int line, const char *fn, int ex, const void *ptr)
{
  tino_exception_ctx *tmp;

  if (ex<=0)
    ex	= -2;
  tmp				= tino_exception_ctx_list;
  if (!tmp)
    tino_fatal("%s:%d:%s: unhandled exception %d: %s", file, line, fn, ex, ptr);
  tino_exception_ctx_list	= tmp->next;
  tmp->ex			= ex;
  tmp->ptr			= ptr;
  longjmp(tmp->jmp, ex);
}

#define TINO_TRY								\
  do										\
    {										\
      tino_exception_ctx tino_try_ctx;						\
										\
      tino_exception_push(&tino_try_ctx, __FILE__, __LINE__, __FUNCTION__);	\
      switch (setjmp(tino_try_ctx.jmp))						\
	{									\
	case 0:
	  /* Code to execute comes here	*/

#define TINO_CATCH(X)					\
	  break;					\
          TINO_CATCH_FT(X)
#define TINO_CATCH_FT(X)				\
	case (X):
	  /* reached when an exception is thrown */

#define TINO_CATCH_ANY					\
	  break;					\
          TINO_CATCH_ANY_FT
#define TINO_CATCH_ANY_FT				\
	default:
	  /* reached when any uncatched exception is thrown */

#define TINO_FINALLY					\
	}						\
	{
	  /* end the switch and start the "break" block to clean up */

#define TINO_ENDTRY					\
	}						\
      tino_exception_pull(&tino_try_ctx);		\
    } while (0)

#define TINO_TRY_RETURN(X)					\
	  { tino_exception_pull(&tino_try_ctx); return X; }

#define TINO_RETHROW						\
	  tino_rethow(&tino_try_ctx, __FILE__,__LINE__,__FUNCTION__)

#endif
