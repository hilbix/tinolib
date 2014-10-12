#-*-text-*- (I dislike the emacs AWK formatting)
#
# This formats "cvs status -v" to something comprehensible.
# It relies on a lot of CVS magic, so other versions of CVS might break it.
# CVS version it was tested with: 1.11.2 (see "cvs version")
#
# Does not know about branches yet
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

BEGIN		{
		want["Working","revision:"]	= 1;
		want["Repository","revision:"]	= 2;
		want["Sticky","Tag:"]		= 3;
		want["Sticky","Date:"]		= 99;
		want["Sticky","Options:"]	= 99;
		want["Commit","Identifier:"]	= 99;
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
  if (stat!="Up-to-date" && stat!="Needs Patch")
    {
      modified++;
      modifies=modifies " " file
      mod=1;
    }
  if (parm[3]!="(none)")
    {
      stickycount++;
      sticks=sticks " " file;
      sticky[file]=parm[3];
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
/^cvs (server|status): Examining /	{ dir=$4"/"; if (dir=="./") dir=""; next; }
/^cvs (server|status):/	{ print; next; }
/^\? /		{ print "missing:  " $2; missing++; missings=missings " " $2; next; }
/^=+$/		{ state=1; next; }
state==1 && /^File:.+Status:/ 	{
		finish();
		files++
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
		out="sort | tail -15 | tac"
		for (a in tags)
		  printf("%4d times tag %s\n", tags[a], a) | out;
		close(out);
		if (untagged)
		  printf "%4d files, %d untagged (%s)\n", files, untagged, show(untag, 50)
                else
		  printf "%4d files\n", files
		print ""
		if (quirx)
		  printf "WARNING! problematic: %d (%s)\n", quirx, show(quirxes, 51)
		if (missing)
		  printf "WARNING! missing are: %d (cvs add %s)\n", missing, show(missings, 43)
		if (updated)
		  printf "Updated files in CVS: %d (cvs update %s)\n", updated, show(updates, 40)
		if (modified)
		  printf "Modified files found: %d (cvs commit %s)\n", modified, show(modifies, 40)
		if (stickycount)
		  {
		    printf "Sticky files found:   %d (%s)\n", stickycount, show(sticks, 51)
		    for (a in stickies)
		      if (a!~/^[0-9.]*$/)
		        printf("sticky tag %s used %d times\n", a, stickies[a]);
		  }
		if (!modified && !stickycount && !updated && !missing)
		  print "All in sync."
		print "";
		}
