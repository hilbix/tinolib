# $Header$
#
# Create a Makefile out of Makefile.tino
#
# All the magic shall go into here with only a little help from the caller side.
#
# Note that this is not ready yet,
# as it does not support all the options I want it to support!
#
# $Log$
# Revision 1.7  2004-09-29 23:32:33  tino
# md5 checksum issues in Makefile generation fixed (hopefully)
#
# Revision 1.6  2004/09/04 14:25:20  tino
# typos corrected
#
# Revision 1.5  2004/09/04 14:12:15  tino
# Automated dependencies added and other make improvements.
#
# Revision 1.4  2004/08/24 23:49:36  tino
# Feature MD5TINOIGN to ignore ever changing output lines from MD5 checks.
#
# Revision 1.3  2004/08/22 05:47:34  Administrator
# Now Makefile generator understands to look into variables set in Makefile.
# This way Auto-Dependencies can be improved a little step and more important,
# the "TINOCOPY" functionality was added, such that I can copy files from
# elsewhere to the distribution and keep that files in sync.
#
# Revision 1.2  2004/07/28 03:42:19  tino
# -
#
# Revision 1.1  2004/07/21 13:29:14  tino
# Creation of standard Makefile from Makefile.tino added

# Give out a warning sign
BEGIN	{
	print "# Makefile automatically generated, do not edit!";
	print "#";
	print "#@MD5TINOIGN@ Creation date: " strftime();
	print "#";
	print "# This file is based on following files:";
	for (i=1; i<ARGC; i++)
		print "#@MD5TINOIGN@ " i ": " ARGV[i];
	empty=1;
	}

# Print the file read
FILENAME!=lastfile	{
	lastfile=FILENAME;
	print "";
	print "#";
	print "#@MD5TINOIGN@ included: " FILENAME;
	print "#";
	empty=1;
	continuation=0;
	}

# Set the variables to loop over
/^#S/	{
	delete splitted;
	for (i=2; i<=NF; i++)
		splitter($i);
	gather="";
	}

# Gather all the rulesets 
/^#R/	{
	if (/^#R /)
		gather=gather substr($0,4) "\n";
	else
		gather=gather substr($0,3) "\n";
	next;
	}

# Expand rules with #v# replaced by the variable part..
# The lines are repeated for all the variable content's parts.
gather!=""	{
	flusher();
	}

# copy CVS tags as comments
/#.*\$[A-Z][a-z]*: .*\$/	{
	sub(/^.*#[^$]*\$/,"");
	sub(/[[:space:]]*\$.*$/,"");
	print "# CVS " $0;
	empty=1;
	next;
	}

# Print out selected comments
/^#C/	{
	print substr($0,3);
	if ($0=="#C")
		{
		empty=0;
		continuation=0;
		next;
		}
	}

# Magic, stop this file and go to the next
/^##END##/	{
	nextfile;
	}

# Fold comments to one empty line
# compress empty lines to one

/^#/ || /^[[:space:]]*$/	{
	empty=1;
	continuation=0;
	next;
	}

	{ line=$0; }

/^[[:space:]]*[A-Z_0-9]*=/	{
	gsub(/^[[:space:]]*/,"");
	n=index($0,"=");
	name=substr($0,1,n-1);
	sub(/^[^=]*=/,"");
	if (had[name])
		{
		igncont=1;
		}
	else
		{
		had[name]=1;
	        var[name]="";
		igncont=0;
		}
	continuation=1;
	}
continuation {
	gsub(/^[[:space:]]*/,"");
        continuation = /\\$/;
	gsub(/[[:space:]]*\\?$/,"");
	if (igncont)
		{
		if (empty)
			print "";
		empty=0;
		print "#" line;
		next;
		}
        var[name]=var[name] " " $0;
	}

# Print all other lines unchanged
	{
	if (empty)
		print "";
	empty=0;
	print line;
	}

# Write the basic dependencies and end marker
END	{
	flusher();
	print "# end";
	}

function splitter(v,a,k)
{
#  print "#" v "=" var[v]
  split(var[v],a,/[[:space:]]*/);
  for (k in a)
    if (a[k] ~ /^[-./a-zA-Z0-9]+$/)
      splitted[a[k]]=1;
}

function flusher(v,s,p,a,i,f)
{
  for (v in splitted)
    {
      s=gather;
  
      p=v;
      sub(/\//,"_",p);
  
      gsub(/#v#/,v,s);
      gsub(/#p#/,p,s);
      while (i=match(s,/#include\(([^)]+)\)/,a))
	{
	  f	= a[1];
	  printf "%s", substr(s,1,i-1);
	  s	= substr(s,i+10+length(f));
	  while ((getline l < f)>0)
	    print l;
	  close(f);
	}
      printf "%s", s;
      empty=1;
    }
  gather="";
}
