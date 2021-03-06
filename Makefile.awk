# Create a Makefile out of Makefile.tino
#
# All the magic shall go into here with only a little help from the
# caller side.  Note that this is not completely ready yet, as it does
# not support all the options I want it to support.  However it does a
# good job now.
#
# Read Makefile.proto on details how the template-system works.
#
# Copyright (C)2004-2014 Valentin Hilbig, webmaster@scylla-charybdis.com
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

# Give out a warning sign
BEGIN	{
	print "# Makefile automatically generated, do not edit!";
	print "# This output (only this Makefile) is Public Domain.";
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
	appender=0;
	}

# Count the Definition numbers
# Define rules like #0# (if nothing defined)
# #1# if exactly 1 entry (and so on)
# #5-# (more than 5 expansions)
# etc.
/^#N/	{
	splitcount=0
	delete splitted
	for (i=2; i<=NF; i++)
		splitter($i,substr($1,3));
	delete splitted
	splitted[""]=splitcount
	splitstr[""]=substr($1,3)
	}

# Set the variables to loop over
# directly behind S a variable can follow
# which then is used "indirectly" as a target.
/^#S/	{
	for (i=2; i<=NF; i++)
		splitter($i,substr($1,3));
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

# Expand the rules, see comment on flusher() below
gather!=""	{
	flusher();
	splitcount=0
	delete splitted;
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

# Magic, skip this rule and append the rest to the end
$0=="Makefile::"	{
	appender=1;
	next;
	}

appender==1 && /^[[:print:]]/ { appender=2; append=append "\n#@MD5TINOIGN@ rules from: " FILENAME "\n"; }
appender==2 { append=append "\n" $0 }
appender { next }
	

# Fold comments to one empty line
# compress empty lines to one

/^#/ || /^[[:space:]]*$/	{
	empty=1;
	continuation=0;
	next;
	}

	{ line=$0; }

# Gather the variable defines
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
	print append "# end";
	}

function splitter(v,i,a,k,n)
{
#  print "#" v "=" var[i v]
  n = split(var[i v],a,/[[:space:]]*/);
  for (k=1; k<=n; k++)
    if (a[k] ~ /^[-_.+\/a-zA-Z0-9]+$/)
      if (!splitted[a[k]])
        {
	  splitted[a[k]]= ++splitcount;
	  splitstr[a[k]]= v;
        }
}

# Flush out all those rules we defined.  This is a little bit (black?)
# magic and is far from beeing perfect.  The lines are repeated for
# all the variable content's parts and #v# is replaced by the variable
# part, #p# is the variable part with the path-character / replaced by
# _ (this is for TINOCOPY: tino/lib.h will become tino_lib.h), #c# by
# the replacement count and #range# means, "only expand the given
# range".  Ranges are of the form #[!][-]from[-[to]]# and can be
# added, so #1##3# means first and third part only.  The ! form
# negates.  If missing, the line will be expanded.
function flusher(v,s,p,a,i,f,o,c)
{
  for (v in splitted)
    {
      c = splitted[v]
      s	= gather;

      p	= v;
      sub(/\//,"_",p);

      gsub(/#v#/,v,s);
      gsub(/#p#/,p,s);
      gsub(/#c#/,c,s);
      gsub(/#s#/,splitstr[v],s);
      o	= 0;
      while (match(s,/#(!?)(-?)([0-9]+)(-([0-9]*))?#/,a))
	{
	  s	= substr(s,1,RSTART-1) substr(s,RSTART+RLENGTH+1);
	  # 1	!
	  # 2	- (in this case from is to)
	  # 3	from
	  # 4	-
	  # 5	to

	  # Transform in a-b case, always
	  if (a[4]=="")
	    a[5]	= a[3];
	  if (a[2]!="")
	    a[3]	= "";
	  # Now compare, default is OK
	  f	= 2;
	  if (a[3]!="" && c<a[3])
	    f	= 1;
	  if (a[5]!="" && c>a[5])
	    f	= 1;
	  # negate if needed
	  if (a[1]!="")
	    f	= 3-f;
	  # "or" the values
	  if (o<f) o = f;

	  # we cannot shortcut as we have to elliminate
	  # all the other ranges, too.
	}
      if (o==1)
	continue;
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
