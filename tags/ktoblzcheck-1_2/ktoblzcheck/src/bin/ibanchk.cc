#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "iban.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    printf("This is only a very initial test program for the Iban class, and it doesn't use the command line arguments yet. Have a look at its source code, please.\n");
    

	// test IBAN
	Iban iban("DE21200500000123456000");
	// check test IBAN
	if (iban.isValid())
	{
		printf("this IBAN is valid\n");
	}
	else
	{
		printf("this IBAN is invalid\n");
	}
	
	// create a new IBAN
	iban = Iban::create(Iban::DE, "50050201", "12345");
	const char *pszIban = iban; // get the IBAN string
	Iban test(pszIban); // validate it again
	// check created IBAN
	if (test.isValid())
	{
		printf("this IBAN is valid\n");
	}
	else
	{
		printf("this IBAN is invalid\n");
	}
	
	return EXIT_SUCCESS;
}
