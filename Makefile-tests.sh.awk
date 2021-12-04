#!/usr/bin/gawk -f
#
# Transform Output:-lines into suitable RUN lines
# Just paste lines on STDIN to output the proper result

	{
	sub(/^Output:/, "")
	sub(/^[[:space:]]*/, "")
	sub(/^\$'/, "")
	sub(/'$/, "")
	gsub(/@/, "@-")
	gsub(/\\\\/, "@\\")
	gsub(/\\n/, "@_")
	gsub(/\\t/, "@t")
	gsub(/  /, " @+")
	}
	{
	print ""
	print
	print ""
	}

