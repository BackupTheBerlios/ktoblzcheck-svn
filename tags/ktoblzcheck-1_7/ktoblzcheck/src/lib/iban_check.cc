#include "iban.h"
#include <stdlib.h>

int main(int argc, char **argv)
{
  bool successful = true;
  char *givenpath = getenv("BANKDATA_SRCPATH");
  std::string filename = 
    givenpath ? std::string(givenpath)+"/ibandata.txt" : "";
  // std::cout << "using " << filename << std::endl;
  IbanCheck ck(filename);
  if (ck.selftest())
    std::cout << "selftest ok" << std::endl;
  else {
    std::cout << "selftest failed" << std::endl;
    successful = false;
  }
  std::string s = " iban fr14 2004 1010 0505 0001 3m02 606";
  std::cout << "test for iban    : " << s << std::endl;
  Iban iban = s;
  std::cout << "transmission form: " << iban.transmissionForm() << std::endl;
  IbanCheck::Result res = ck.check(iban);
  if (res != IbanCheck::OK) successful = false;
  std::cout << "check result     : " << res
	    << " (" << ck.resultText(res) << ")" << std::endl;
  std::cout << "printable form   : " << iban.printableForm()
	    << std::endl;
  s = "FR1420041010050500013X02606";
  std::cout << "expect incorrect checksum:" << std::endl;
  res = ck.check(s);
  if (res != IbanCheck::BAD_CHECKSUM) successful = false;
  std::cout << s << ": " << res
	    << " (" << ck.resultText(res) << ")" << std::endl;

  return successful ? 0 : 1;
}

