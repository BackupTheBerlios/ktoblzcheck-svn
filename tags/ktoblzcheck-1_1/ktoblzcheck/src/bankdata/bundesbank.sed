#!/usr/bin/sed -nf
# 
# This script converts the ascii file of German Bank Codes (BLZ-Datei)
# as available from the Deutsche Bundesbank into the bankdata.txt file
# for the ktoblzcheck library.
#
#   Usage: ./bundesbank.sed < blzAABB.txt > bankdata.txt
#
# By Daniel Gloeckner <daniel-gl@gmx.net>
#
/^.\{8\}1/ {
/^0\{8\}/ b weg
s/^\(.\{8\}\).\{19\}\(.\{58\}\).\{25\}\(.\{29\}\).\{42\}\(.\{2\}\).\{5\}/\1	\4	\2	\3/
s/ \+	/	/g
y/
/ /
s| \+\r*$||
p
: weg
}
