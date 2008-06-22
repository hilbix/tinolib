<? #-*-c-*-
# $Header$
#
# SQLITE database wrapper.
# This shall be XHTML 1.0 compliant.
#
# This is GNU GPL v2 or higher.
#
# $Log$
# Revision 1.1  2008-06-22 11:32:20  tino
# First checkin
#

include("db.php");

class DbSqlite extends Db
{
  function DbSqlite($name)
    {
      $this->type	= "SQLite";
      parent::Db($name);
    }

  function _init()
    {
      sqlite_busy_timeout($this->db, 15000);
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

  function _first($r)
    {
      return @sqlite_next($r);
    }

  function _next($r)
    {
      return @sqlite_next($r);
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
