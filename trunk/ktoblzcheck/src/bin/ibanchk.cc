#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "iban.h"
#include <iostream>

int main(int argc, char *argv[])
{
  if (argc != 2 || argv[1][0] == '-') {
    std::cerr << "Aufruf: ibanchk iban" << std::endl;
    return 2;
  }
  IbanCheck ibanchk;
  if (ibanchk.error()) {
    std::cerr << "Fehler beim Lesen von ibandata.txt" << std::endl;
    return 3;
  }
  Iban iban(argv[1]);
  IbanCheck::Result res = ibanchk.check(iban);
  std::cout << iban.printableForm() << ": "
	    << ibanchk.resultText(res) << std::endl;
  if (res == IbanCheck::OK)
    return 0;
  else
    return 1;
}
