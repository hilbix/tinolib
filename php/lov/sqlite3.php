<? #-*-c-*-
# $Header$
#
# SQLITE version 3 database wrapper.
# This shall be XHTML 1.0 compliant.
#
# This is GNU GPL v2 or higher.
#
# $Log$
# Revision 1.3  2009-06-04 05:09:18  tino
# Added special transactions
#
# Revision 1.2  2009-02-11 14:50:10  tino
# SQLite3 now working via PDO
#
# Revision 1.1  2009-02-01 23:28:16  tino
# added

include("pdo.php");

class DbSqlite3 extends DbPDO
{
  var $sq;

  function DbSqlite3($name)
    {
      parent::DbPDO($name, "sqlite");
    }

  function _init()
    {
#      sqlite_busy_timeout($this->db, 15000);
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
      return $this->PdoOpen($name);
    }

# SQLite has issues with transactions:
# NEVER promote shared locks to exclusive ones.
#
# This happens when you start a write transaction without "begin exclusive".
# There are SERIOUS BUGS if you don't do this:
# - An elder SQLite version corrupts it's internal cache in such a case,
#   which might lead to a corrupted database if you do not abort the connection.
# - Timeout values are not in effect then, so SQLite immediately fails.
#   This is due to the SQLite standard deadlock protection.
#   This means there is no timeout on concurrent read access.
# Therefor we cannot rely on the PDO way to start a transaction
# and have to do this directly in SQLite.

  function beginTransaction($type="exclusive")
    {
      $this->qok("begin $type");
    }
  function rollbackTransaction()
    {
      $this->qok("rollback");
    }
  function commitTransaction()
    {
      $this->qok("end");
    }
};

?>
