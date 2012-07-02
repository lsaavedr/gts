#! /bin/bash

awk 'BEGIN{FS=" |\t|-|>"}
{
    if ($1 == "new")
	new[$3] = $2;
    else if ($1 == "destroy") {
	if (new[$3] != $2)
	    print "ALERT: destroying $2 $3 allocated as new[$3]" > "dev/stderr";
	delete new[$3];
    }
}
END {
    for (i in new)
	print new[i] " " i;
}'
