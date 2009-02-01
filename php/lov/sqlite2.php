<? #-*-c-*-
# $Header$
#
# SQLITE version 2 database wrapper.
# This shall be XHTML 1.0 compliant.
#
# This is GNU GPL v2 or higher.
#
# $Log$
# Revision 1.2  2009-02-01 23:27:18  tino
# SQLite2 corrected to work with new binding (untested).
#
# Revision 1.1  2008-06-22 11:32:20  tino
# First checkin

include("db.php");

class DbSqlite2 extends Db
{
  function DbSqlite2($name)
    {
      $this->type	= "SQLite2";
      parent::Db($name);
    }

  function _init()
    {
      sqlite_busy_timeout($this->db, 15000);
      $this->types	= array(
	"INT"		=> "NUMERIC",
	"FLOAT"		=> "NUMERIC",
	"VARCHAR"	=> "TEXT",
	"TEXT"		=> "TEXT",
	"BLOB"		=> "BLOB",
	"TIMESTAMP"	=> "NUMERIC",
	"TS()"		=> "datetime('now')",
	"DATETIME"	=> "NUMERIC",
	"NOW()"		=> "datetime('now')",
	);
    }

  function _open($name)
    {
      return sqlite_open($name, 0600);
    }

  function _escape($s)
    {
      return sqlite_escape_string($s);
    }

  function _query($q)
    {
      return sqlite_query($this->db, $q);
    }

  function _single($q)
    {
      return sqlite_fetch_single($q);
    }

  function _row($r)
    {
      return sqlite_fetch_array($r,SQLITE_NUM);
    }

  function _all($r)
    {
      return sqlite_fetch_all($r,SQLITE_NUM);
    }
};

?>
