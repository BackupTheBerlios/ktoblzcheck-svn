all-local:
	if test bankdata.txt -nt ../lib/data.cc; then \
	sh transform.sh >../lib/data.cc; \
	fi

install:
	if ! [ -d ${prefix}/ktoblzcheck ]; then mkdir ${prefix}/ktoblzcheck; fi
	if ! [ -d ${prefix}/ktoblzcheck/bankdata ]; then mkdir ${prefix}/ktoblzcheck/bankdata; fi
	cp bankdata.txt ${prefix}/ktoblzcheck/bankdata
