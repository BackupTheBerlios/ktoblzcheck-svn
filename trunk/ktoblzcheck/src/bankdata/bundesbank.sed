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

# Only use lines that have a "1" as 9th character (Hauptniederlassungen)
/^.\{8\}1/ {
# Ignore lines starting with 8 zeros (deleted banks)
/^0\{8\}/ b weg
# Divide lines into the different text fields. Print only the required ones
s/^\(.\{8\}\).\{19\}\(.\{58\}\).\{25\}\(.\{29\}\).\{42\}\(.\{2\}\).\{5\}/\1	\4	\2	\3/
# Replace multiple whitespaces with one tab character
s/ \+	/	/g
# Transliterate DOS line endings by one whitespace
y/
/ /
# Remove trailing whitespaces and/or carriage return (ASCII 13) characters
s| \+\r*$||
# Print the result
p
: weg
}
