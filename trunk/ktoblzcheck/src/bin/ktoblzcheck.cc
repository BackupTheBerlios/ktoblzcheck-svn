/***************************************************************************
                             -------------------
    cvs         : $Id$
    begin       : Sat Aug 10 2002
    copyright   : (C) 2002 by Fabian Kaiser
    email       : fabian@openhbci.de

 ***************************************************************************
 *                                                                         *
 *   This library is free software; you can redistribute it and/or         *
 *   modify it under the terms of the GNU Lesser General Public            *
 *   License as published by the Free Software Foundation; either          *
 *   version 2.1 of the License, or (at your option) any later version.    *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 59 Temple Place, Suite 330, Boston,                 *
 *   MA  02111-1307  USA                                                   *
 *                                                                         *
 ***************************************************************************/

#include <config.h>
#include <ktoblzcheck.h>
#include <iostream>
#include <fstream>

using namespace std;

string resFile = "";
string method = "";
bool justReturnCode = false;
int nextArg = 1;

void checkArg(string argument) {
  if (argument == "--returncode") {
	nextArg++;
	justReturnCode = true;
  }

  if (argument == "--bankdata-path") {
     std::cout << "BANKDATA_PATH=" << BANKDATA_PATH << std::endl;
     exit(0);
  }
  
  if (argument.length() > string("--file=").length()) {
	if (argument.substr(0, 7) == "--file=") {
	  resFile = argument.substr(7);
	  nextArg++;
	}
  }

  if (argument.length() > string("--method=").length()) {
	if (argument.substr(0, string("--method=").length()) == "--method=") {
	  method = argument.substr(string("--method=").length());
	  nextArg++;
	}
  }
}

int main(int argc, char **argv) {
  string bankId;
  string accountId;

  for (int i=1; i<argc; i++) {
	checkArg(argv[i]);
  }

  if (argc - nextArg < 2) {
	cout << "Usage:" << endl;
	cout << argv[0] 
	     << " [--returncode] [--file=datafile] <bank-id> <account-id>"
	     << endl
	     << "  --file=<resource-file>: The file that contains the bankinformation" 
	     << endl
	     << "  --bankdata-path: No checking, only print the bankdata-path"
	     << endl
	     << "  --returncode: no output, result is returned via the returncode"
	     << endl;

	cout
	    << "                0: Everything is ok: account and bank match" << endl
	    << "                1: Validation algorithm is unknown/unimplemented" << endl
	    << "                2: Account and bank do not match" << endl
	    << "                3: No bank with given bank-id found" << endl 
	    << endl;

	exit(1);
  }

  // Can we open the specified data file? If not, use the installed
  // data file
  AccountNumberCheck *check_ptr;
  if (resFile.empty())
     check_ptr = new AccountNumberCheck();
  else 
  {
     ifstream test(resFile.c_str());
     if (test.fail())
     {
	std::cerr << "ktoblzcheck: main: File '" << resFile
		  << "' could not be opened. Trying default file."
		  << std::endl;
	check_ptr = new AccountNumberCheck();
     } 
     else 
	check_ptr = new AccountNumberCheck(resFile);
  }

  AccountNumberCheck& checker = *check_ptr;

  bankId = argv[nextArg++];
  accountId = argv[nextArg++];
  bool found = true;

  AccountNumberCheck::Record bankData;
  try {
	bankData = checker.findBank(bankId);
  } catch (int i) {
	found = false;
  }

  AccountNumberCheck::Result result =
	checker.check(bankId, accountId, method);

  if (! justReturnCode) {
	cout << "Bank: " 
	     << (found ? 
		 ("'" + bankData.bankName + "' at '" + bankData.location + "'") :
		 "<unknown>") << " (" << bankId << ")" << endl;
	cout << "Account: " << accountId << endl;
	string text;
	if (result == AccountNumberCheck::OK)
	  text = "OK";
	if (result == AccountNumberCheck::UNKNOWN)
	  text = "Sorry, I don't know";
	if (result == AccountNumberCheck::ERROR)
	  text = "ERROR, they do not match";
	if (result == AccountNumberCheck::BANK_NOT_KNOWN)
	  text = "bank is not known";

	cout << "Result is: " << "(" << result << ") " << text << endl << endl;
  }

  delete check_ptr;
  return result;
}

