<? #-*-c-*-
# $Header$
#
# SQLITE version 3 database wrapper.
# This shall be XHTML 1.0 compliant.
#
# This is GNU GPL v2 or higher.
#
# $Log$
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
};

?>
