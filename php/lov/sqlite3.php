<? #-*-c-*-
# $Header$
#
# SQLITE version 3 database wrapper.
# This shall be XHTML 1.0 compliant.
#
# This is GNU GPL v2 or higher.
#
# $Log$
# Revision 1.1  2009-02-01 23:28:16  tino
# added
#

include("db.php");

class DbSqlite3 extends Db
{
  var $sq;

  function DbSqlite3($name)
    {
      $this->type	= "SQLite3";
      parent::Db($name);
    }

  function _init()
    {
      sqlite_busy_timeout($this->db, 15000);
      $this->types	= array(
	"INT"		=> "INTEGER",
	"FLOAT"		=> "REAL",
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
      return new SQLite3($name);
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
