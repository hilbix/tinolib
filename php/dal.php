<?
# $Header$
#
# Database abstraction layer.
#
# This layer allows database abstraction to a high degree.
# The idea behind this are as follows:
# - All database changes are logged in growing text files.
# - No updates are done through other interfaces.
# - Query is done ISAM like on keys usually, so you normally do not need SQL.
# - There can be more than one source of database updates.
# - You can easily replace the database backend through by something else.
# - You can have the same data in several types of databases.
#
# $Log$
# Revision 1.1  2004-10-20 21:09:54  tino
# first version (not yet working)
#

# Works as follows:
#
# You have a database backend which is the pure database driver.
# You have the database abstraction layer which funnels everything to the backend.
# You have multiple database frontends, where your query routines are situated.
# Your code then uses these frontends for data manipulation.
#
# For SQL databases the frontend usually is one table or one view.
# For DBM databases the frontend usually gives you one file.
#
# Create as follows:
# class tbl_a extends tino_dal_frontend {
#   function tbl_a(&$dal) { parent::tino_dal_frontend($dal, "a"); }
#   ... }
# class db_b extends tino_dal_backend { ... }
# $myob=new tbl_a(new tino_dal(new db_b()));
#
# If you think this is bullshit, sorry, this only deals with the poblems of
# "References inside constructors" of PHP.  Sorry ;)

# Warning, this implementation can change without notice
# Only the bare interface stays the same!
class tino_dal_query
  {
    var $back, $q, $results;

    /* Again, the $q parameter is to ease your implementation.
     * In real OO this is bad design and you would need to extend this here.
     * (Perhaps I will make a clean tino_dal_query_base class sometime.)
     */
    function tino_dal_query(&$back, &$q)
      {
	$this->back	=& $back;
	$this->q	=& $q;
	$this->results	=  null;
      }

    /* Free memory.
     *
     * You need to call this if you want to end a query before you reach the end.
     */
    function end()
      {
	if ($this->back)
	  $this->back->query_end($this, $this->q);
	$this->back	= null;
	$this->q	= null;
	$this->results	= null;
      }

    /* returns (key,value) or null
     */
    function next()
      {
	if ($results)
	  $r	= array_pop($results);
	else if ($this->back)
	  $r	= $this->back->query_next($this, $this->q);
	else
	  return null;
	if ($r==null && $this->back)
	  $this->end();
	return $r;
      }

    /* This returns the array of all results (from current position).
     * Afterwards you can forget (set =null) this class to free memory.
     * Note that "next" still works afterwards.
     * This does not return a reference, so you can re-iterate.
     *
     * This can be called from tino_dal_backend, too:
     * Often ODBC etc. cannot handle selects in selects.
     * In this case you need to call all()
     * which fetches all results from the query,
     * and thus ends the query.
     * Note that with cursors you don't need this in SQL databases.
     * However cursors are not yet implemented here.
     */
    function all()
      {
	/* Were we called already?
	 */
	if (!$this->back || $this->result)
	  return $this->result;
	$this->result	= array();
	while ($r=$this->back->query_next($this, $this->q))
	  $this->results[]	= $r;
	$this->back->query_end($this, $this->q);
	$this->back	= null;
	$this->q	= null;
	return $this->results;
      }
  }

# Warning, this implementation can change without notice
# Only the bare interface stays the same!
class tino_dal_backend
  {
    var	$q;	// current query going on

    function tino_dal_backend()
      {
	$q	= null;
      }

    # Return an object to handle everything.
    # The object then is passed from the above layers down here.
    # I know this is bad OO design, however this makes it just too easy
    # to write new generic adapters this way.
    function &table(&$t)
      {
	/* In the simple case we make a *copy* of the object.
         * Note that the object is passed as a reference from above.
	 * I could write $t above, but I dislike unclean interfaces:
	 * The table function always shall get a reference to the object
	 * and return a reference to an object which contains the
	 * referenced information.  Assume following sequence:
	 * $what="this";
	 * $my = new tino_dal_frontend($dal,$what);
	 * $my->get("1");	// access this::1
	 * $what="that";
	 * $my->get("1");	// access that::1
	 * With the implementation below this::1 is accessed two times.
	 *
	 * If you replace the two lines with "return $t" then
 	 * this::1 and that::1 will be accessed.
	 * But beware, this nearly certainly will break the replay feature below
	 * if table_name does not return a static value then.
	 * And from_string and to_string will become somewhat tricky.
	 * I dislike such type of sideeffects, however it might
	 * make you driver more easy this way.
	 */
	var $c = $t;
        return $c;
      }
    /* Return text representation of the table.
     * Usually this will be the table name or something globally unique.
     * Remember that you need it to replay the information later on.
     */
    function table_string(&$t)
      {
	return $t;
      }
    /* Return text representation of the argument.
     * Usually the argument will be text.
     * However in a SQL environment, you will see arrays here, see put below.
     */
    function to_string(&$t,$s)
      {
	return $s;
      }
    function from_string(&$t,$s)
      {
	return $s;
      }

    /* For SQL you will make it an array like this:
     * ->get($t,32)
     * which will do:
     * select c_name,c_email from $t where c_id=32
     * THIS ALWAYS ONLY RETURNS THE FIRST LINE!
     */
    function get(&$t,&$k) { }
    /* This starts a search with the given parameters.
     * The parameters are dependent of the backend!
     * This returns a tino_dal_query object.
     * With "null" it is expected to return all information in the table.
     */
    function &query_imp(&$t,$k)	{ return null; }	// overload this function!
    function &query(&$t,$k=null)
      {
	$o	=& query_imp($t,$k);
	$this->q	=& $o;
	return $o;
      }
    /* Used internally to return the next object on the query
     * This gets the ref to the query object and the set query parameter.
     */
    function query_next_imp(&$o,&$q)	{ return null; }	// overload this function!
    function query_next(&$o,&$q)
      {
	// Free the current query to start a new one.
	// We must check this here, as only here we see the correct reference
        // the user does.  Not doing it here would impose a high discipline
	// in getting references right.
	if ($this->q && this->q!==$o)
	  $this->q->all();	// This calls query_end.
	$this->q	=& $o;
	return $this->query_next_imp(&$q);
      }
    function query_end_imp(&$o,&$q)	{ }	// overload this function!
    function query_end(&$o,&$q)
      {
	if ($this->q===$o)
	  $this->q	= null;
	query_end_imp($o,$q);
      }

    /* Usually you will call it like that:
     * frontend->put($t, 32, array("name"=>"tino","email"=>"webmaster@"));
     * which will transform into
     * backend->put($t, "32", "c_name='tino',c_email='webmaster@'")
     * which will be transformed into SQL by your backend:
     * replace t_$t set c_id=32,c_name='tino',c_email='webmaster@'
     * I hope you get it ;)
     */
    function put(&$t,$k,$v)	{ }	// $k and $v are strings from to_string!
    /* This is "insert" instead of "replace"
     */
    function add(&$t,$k,$v)	{ }	// $k and $v are strings from to_string!
    /* This is "update"
     * update t_$t set c_name='tino',c_email='webmaster@' where c_id=32
     */
    function upd(&$t,$k,$v)	{ }	// $k and $v are strings from to_string!
    /* Delete a key
     */
    function del(&$t,$k)	{ }	// $k is a string from to_string!
  };

# Warning, this implementation can change without notice
# Only the bare interface stays the same!
class tino_dal
  {
    class tino_dal_backend	$back;
    class tino_dal_recorder	$rec;

    function tino_dal(&$backend, $recorder=null)
      {
        $this->back	=& $backend;
	if ($recorder==null)
	  {
	    $u	= posix_uname();
	    $n	= $u['nodename'];
	    $d	= $u['domainname'];
	    if ($d!='') $n="$n.$d.";
	    $this->rec	=& new tino_dal_recorder($n);
	  }
	else if (is_string($recorder))
	  {
	    $this->rec	=& new tino_dal_recorder($recorder);
	  }
	else
	    $this->rec	=& $recorder;
      }

    function &table(&$t)
      {
	/* return the backend object unchanged	*/
	$r	=& $back->table($v);
	/* replay all new information found in the recorder	*/
	$s	=  $back->table_string($r);
	while (list($k,$v)=$this->rec->replay($s))
	  $back->put($r,$k,$v);
	return $r;
      }

    function get(&$t, $key)
      {
	return $back->get($t, $key);
      }

    function put(&$t, $key, $val)
      {
	$k	= $back->to_string($t,$key);
	$v	= $back->to_string($t,$val);
	$rec->store($back->table_name($t),$k,$v);
        $back->put($r,$k,$v);
      }
  };

# Warning, this implementation can change without notice
# Only the bare interface stays the same!
class tino_dal_frontend
  {
    class tino_dal	$dal;
    var			$table;

    function tino_dal_frontend(&$dal, &$table)
      {
	$this->dal	=& $dal;
	$this->table	=& $dal->table($table);
      }

    /* Interface: */

    function get($key)
      {
	return $dal->get($this->table, $key);
      }

    function put($key, $value)
      {
	return $dal->put($this->table, $key, $value);
      }
  };
?>
