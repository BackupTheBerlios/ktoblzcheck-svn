#!/bin/sh

cat bankdata.txt | awk -F\\t '{print "ADD_RECORD(" $1 ",\"" $2 "\",\"" $3 "\",\"" $4 "\")"}'
