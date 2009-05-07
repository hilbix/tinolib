<? #-*-c-*-
# $Header$
#
# SQLITE version 3 database wrapper.
# This shall be XHTML 1.0 compliant.
#
# This is GNU GPL v2 or higher.
#
# $Log$
# Revision 1.2  2009-05-07 00:09:04  tino
# Do not make indexes too long
#
# Revision 1.1  2009-02-01 21:06:38  tino
# MySQL support added
#

include("db.php");

class DbMysql extends Db
{
  var $user, $pass;

  function DbMysql($name, $user, $pw)
    {
      $this->type	= "MySQL";
      $this->user	= $user;
      $this->pass	= $pw;
      parent::Db($name);
    }

  function _err()
    {
      return mysql_error()." (".mysql_errno().")";
    }

  function _init()
    {
      $this->types	= array(
	"INT"		=> "BIGINT",
	"FLOAT"		=> "DOUBLE",
	"VARCHAR"	=> "VARCHAR(195)",
	"TEXT"		=> "TEXT",
	"BLOB"		=> "BLOB",
	"TIMESTAMP"	=> "TIMESTAMP",
	"TS()"		=> "UNIX_TIMESTAMP()",
	"DATETIME"	=> "DATETIME",
	"NOW()"		=> "NOW()",
	);
    }

  function _c($r)
    {
      if ($r!==false && $r!==true)
        mysql_free_result($r);
    }

  function _open($name)
    {
      $db	= mysql_connect('localhost', $this->user, $this->pass);
      if ($db && !mysql_select_db($name, $db))
        {
          mysql_close($db);
          $db	= false;
        }
      return $db;
    }

  function _escape($s)
    {
      /* How to make this backward compatible to old insecure PHP?
       *
       * Actually this function has a wrong and misleading name, it's not only for strings!
       */
      return mysql_real_escape_string($s, $this->db);
    }

  function _query($q)
    {
      return mysql_query($q, $this->db);
    }

  function _row($r)
    {
      return mysql_fetch_row($r);
    }
};

?>
