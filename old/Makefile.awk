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
# Revision 1.1  2004-07-21 13:29:14  tino
# Creation of standard Makefile from Makefile.tino added
#

# Give out a warning sign
BEGIN	{
	print "# Makefile automatically generated, do not edit!"
	print "#"
# This conflicts with the compare/md5 check for now
#	print "# Creation date: " strftime()
#	print "#"
	print "# This file is based on following files:"
	for (i=1; i<ARGC; i++)
		print "# " i ": " ARGV[i]
	empty=1
	}

# Print the file read
FILENAME!=lastfile	{
	lastfile=FILENAME;
	print ""
	print "#"
	print "# included: " FILENAME
	print "#"
	empty=1
	next
	}

# copy CVS tags as comments
/#.*\$[A-Z][a-z]*: .*\$/	{
	sub(/^.*#.*\$/,"");
	sub(/^\$.*$/,"");
	print "# CVS " $0
	empty=1
	next;
	}

# Magic, stop this file and go to the next
/^##END##/	{
	empty=1
	nextfile;
	}

# Fold comments to one empty line
/^#/	{
	empty=1;
	next;
	}

# compress empty lines to one
/^[[:space:]]*$/	{ empty=1; next; }

# Print all other lines unchanged
	{
	if (empty) print "";
	empty=0;
	print
	}

# Write the basic dependencies and end marker
END	{
	print ""
	print "#"
	print "# end of includes"
	print "#"
	print "# Automatically created dependencies:"
	print ""
	print "# (not yet implemented)"
	}
