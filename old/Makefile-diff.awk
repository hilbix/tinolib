#-*-text-*- (I dislike the emacs AWK formatting)
# $Header$
#
# This formats "cvs status -v" to something comprehensible.
# It relies on a lot of CVS magic, so other versions of CVS might break it.
# CVS version it was tested with: 1.11.2 (see "cvs version")
#
# Does not know about branches yet
#
# $Log$
# Revision 1.5  2004-06-12 08:53:53  tino
# bugfix + warning now prints files
#
# Revision 1.4  2004/06/12 08:45:29  tino
# untagged files skipped if there are none
#
# Revision 1.3  2004/06/12 08:41:29  tino
# bugfix
#
# Revision 1.2  2004/06/12 08:39:46  tino
# make diff now knows about "No Tags Exist"
#
# Revision 1.1  2004/06/12 08:34:25  tino
# "make diff" added

BEGIN		{
		want["Working","revision:"]	= 1;
		want["Repository","revision:"]	= 2;
		want["Sticky","Tag:"]		= 3;
		want["Sticky","Date:"]		= 99;
		want["Sticky","Options:"]	= 99;
		print ""
		}

function finish(s)
{
  if (file=="")
    return;

  if (tag=="")
    {
      untagged++;
      untag=untag " " file
    }
  mod=0;
  if (stat!="Up-to-date")
    {
      modified++;
      modifies=modifies " " file
      mod=1;
    }
  if (parm[3]!="(none)")
    {
      stickycount++;
      sticky[file]=parm[3]
      stickies[parm[3]]++;
      if (mod)
        {
          print "sticky(" parm[3] ")+modified: " file;
	  quirx++;
	  quirxes=quirxes " " file;
        }
      else
        print "sticky(" parm[3] "): " file;
    }
  else
    {
      upd=0;
      # Treat added files as modified, saves a lot of confusing output
      if (stat!="Locally Added" && "x"parm[1]!="x"parm[2])
        {
	  updated++;
	  updates=updates " " file;
	  upd=1;
	}
      if (upd && mod)
        {
	  print "updated(" parm[1] ")+modified(" parm[2] "): " file
	  quirx++;
	  quirxes=quirxes " " file;
	}
      else if (upd)
	print "updated:  " file
      else if (mod)
	print "modified: " file
    }
  file	= "";
  stat	= "";
  tag	= "";
  delete parm;
}

function show(s,n)
{
  if (substr(s,1,1)==" ")
    s=substr(s, 2)
  if (length(s)<=n)
    return s;
  return substr(s,1,n-3)"..."
}

/^$/		{ next; }
/^cvs server: Examining /	{ dir=$4"/"; if (dir=="./") dir=""; next; }
/^cvs server:/	{ print; next; }
/^\? /		{ print "missing:  " $2; missing++; missings=missings " " $2; next; }
/^=+$/		{ state=1; next; }
state==1 && /^File:.+Status:/ 	{
		finish();
		file=$0
		stat=$0
		sub(/^File: */,"",file);
		sub(/[[:space:]]*Status:.*$/,"",file);
		sub(/.*Status: */,"", stat);
		file=dir file
		state++
		next
		}
!/^[[:space:]]/	{ state=0; }
state==2 && want[$1,$2] {
		parm[want[$1,$2]]=$3
		next
		}
state==2 && $1=="Existing" && $2=="Tags:" {
		state++
		next
		}
state==3 && /No Tags Exist/	{ next }
state==3 && match($0,/[[:space:]]\(revision: ([0-9.]*)\)$/,arr)	{
		rev=arr[1];
		if (rev==parm[1])
		  {
		    tag=tag "," $1
		    tags[$1]++
		  }
#		  else print file "-" rev "-" parm[1] "-" $0
		next
		}
		{ print "Cannot grok:" file ": " $0; state=0; }

END		{
		finish();
		print "";
		for (a in tags)
		  printf("tag %s used %d times\n", a, tags[a]);
		if (untagged)
		  printf "untagged files:       %d (%s)\n", untagged, show(untag, 40)
		print ""
		if (quirx)
		  printf "WARNING! problematic: %d (%s)\n", quirx, show(quirxes, 40)
		if (missing)
		  printf "WARNING! missing are: %d (cvs add %s)\n", missing, show(missings, 40)
		if (updated)
		  printf "Updated files in CVS: %d (cvs update %s)\n", updated, show(updates, 40)
		if (modified)
		  printf "Modified files found: %d (cvs commit %s)\n", modified, show(modifies, 40)
		if (stickycount)
		  {
		    printf "Sticky files found:   %d\n", stickycount
		    for (a in stickies)
		      if (a!~/^[0-9.]*$/)
		        printf("sticky tag %s used %d times\n", a, stickies[a]);
		  }
		if (!modified && !stickycount && !updated && !missing)
		  print "All in sync."
		print "";
		}
