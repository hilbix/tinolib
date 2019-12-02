The `fully-merged` helper branch records all the old development history.
We should not drop it, because some historic `git submodule` references might point
into the development commits instead of the `master` branch.

To unambiguously mark such mergers, this is done as follows:

	git fake-merge -a -c master
	git fake-merge -a -c DEVBRANCH
	git fake-merge -a -c master

Notes:

- Except for this `README.md` this branch is empty, as it is only for recording commits.
- For `git fake-merge` see https://github.com/hilbix/gitstart/
- The `master` branch is often updated from a development branch using following sequence
  (`git ff` and `git fake-merge` are from `gitstart`) to clean up the development autocommits:

	git checkout -b tmp-merge $DEV	# create a new work branch and switch to it
	git rebase master		# rebase the development branch to the new tip of master
	git diff $DEV			# check that everything was correctly applied to master
	git branch tmp-merge-ok		# remember the state
	git rebase -i master		# here the commits are compacted and possibly edited
	git diff tmp-merge-ok		# verify everything still ok
	git branch -D tmp-merge-ok	# remove the checkpoint again
	git checkout master		# if everything is fine switch to master
	git ff tmp-merge		# fast-forward the master to the work branch
	git branch -d tmp-merge		# drop this merger again
	git checkout fake-merge		# now switch to this branch here
	git fake-merge -a -c master	# see above
	git fake-merge -a -c $DEV
	git fake-merge -a -c master

There is not much you can automate in the above sequence, except for the last 3 commands perhaps.

