#error "Don't include this yet"
/* UNIT TEST FAILS *
 *
 * Virtual Data Layer
 *
 * Designed to be able to handle 2^70 byte (1 Zetta).
 * Data IO is done on pages.
 * Pages are of size 2^X where X=8..20, default X=12 (4 KB)
 *
 * This is not designed to work over applications!
 * It only works within a threaded application.
 *
 * Copyright (C)2005-2014 Valentin Hilbig <webmaster@scylla-charybdis.com>
 *
 * This is release early code.  Use at own risk.
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA.
 */

#ifndef tino_INC_vdl_h
#define tino_INC_vdl_h

typedef struct tino_vdl *TINO_VDL;
typedef unsigned long long TINO_VDL_PAGE;

/* Initialize the virtual data layer
 *
 * params are key,value pairs for initialization of the layer code.
 * returns a pointer to the descriptor block.
 * raises an exception on error (never returns NULL).
 *
 * There can be different implementations of a VDL.  Parameters can be:
 * type     memory (default), file (if parameter file is present)
 * file     filename
 * create   create mode (octal), if missing is not created
 */
TINO_VDL tino_vdl_open(char **params);
void tino_vdl_close(TINO_VDL);

/* Alter some parameters of the VDL.
 *
 * Raises exceptions on non recoverable errors.
 * Returns 0 on success
 * Returns 1 on parameter errors
 * Returns 2 if parameter cannot be altered
 * Returns -1 if operation failed for some reason
 */
int tino_vdl_alter(const char **params);

/* Get the size of a page.  Usually this is 4KB.
 * Note that you can only set the page size on creation of the VDL.
 */
const int   tino_vdl_pagesize(TINO_VDL);

/* Read and write pages
 * Note that page==0 is a reserved page.
 *
 * In case of an unrecoverable error, an exception is raised.
 */
void  tino_vdl_read(TINO_VDL, TINO_VDL_PAGE, void *, int pages);
void  tino_vdl_write(TINO_VDL, TINO_VDL_PAGE, const void *, int pages);

/* Perform reorganization step
 *
 * Reorganizes the data in the VDL if needed.
 * Returns the estimated amount of reorganization left, where 100000 = 100%
 * Returns 0 if reorganized
 */
int tino_vdl_reorg(TINO_VDL);

/* Perform verification step.
 *
 * Verifies the contents of the VDL.  If you want to force a restart, use start=1 else start=0
 * Returns the estimated amount of verification left, where 100000 = 100%.
 * Returns 0 if reorganized.
 *
 * Raises exceptions on unrecoverable errors.
 * You should run this when
 */
int tino_vdl_verify(TINO_VDL, int start);

/* Returns status key,value pairs of information about the VDL.
 *
 * This contains information like errors in the VDL etc.
 * This is a static buffer returned, destroyed on the next call to this routine!
 * Full can be:
 * 0  only changed information is listed
 * 1  all dynamic information is listed
 * 2  all information is listed (slow and big)
 * -1 a help on all possible parameters is listed
 * -2 a help on all possible raised exceptions is given (big!)
 */
const char * const * tino_vdl_status(TINO_VDL, int full);

/* VDL Backup implementation
 *
 * The VDL keeps a list of altered pages.
 * You can access these list and clear the flag with following commands.
 * This is race-condition free and thread safe (there must be only one backup thread).
 * You can backup a VDL while it is altered.
 *
 * Do it as follows:
 *
 * #define P_MAX 100
 * TINO_VDL_PAGE p;
 * char buf[tino_vdl_pagesize(v)*P_MAX];
 *
 * while (p=tino_vdl_dirty(v))
 *   {
 *     int  n = tino_vdl_backup(v, p);
 *     fwrite(backup_fd, p, buf, P_MAX);
 *     // error handling here!  Don't do next if error!
 *     tino_vdl_clean(v, p, n);
 *   }
 *
 * If you want to be protected in case the whole backup fails, use a transaction instead!
 */
TINO_VDL_PAGE tino_vdl_dirty(TINO_VDL);
int   tino_vdl_backup(TINO_VDL, TINO_VDL_PAGE, void *buf, int max);
tino_vdl_clean(TINO_VDL, TINO_VDL_PAGE, int pages);

/*VDL Transactions
 *
 * !NOT YET IMPLEMENTED!
 *
 * Transactions are "branches in time".
 * You can implement snapshots with this as well.
 * You must tino_vdl_close() the openened transaction!
 *
 * A transaction starts with a virtual duplication of the VDL.
 * You then can alter etc. the VDL.
 * Afterwards you can commit the changes or reject the changes.
 * This also closes the duplicate again.
 *
 * Note that tino_vdl_close() automaticaly commits the closed VDL
 * and automatically rejects and closes all branches.
 * VDLs opened with tino_vdl_open() are in "autocommit" mode,
 * which means, all transactions are automatically committed.
 *
 * Transactions can be nested.  You can open an transaction on a
 * transaction, but beware of close() of the "mother transaction"!
 *
 * This is thread safe.  You can do a relieable online backup this way.
 * If the backup fails, use reject(), else use commit().
 *
 * You can always do a commit() in a safe state.
 * Calling reject() on a VDL in autocommit mode does nothing.
 */
TINO_VDL tino_vdl_open_transaction(TINO_VDL);
void tino_vdl_commit(TINO_VDL);
void tino_vdl_reject(TINO_VDL);

#endif
