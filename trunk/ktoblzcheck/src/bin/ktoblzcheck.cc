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
#include <ktoblzcheck.hh>
#include <iostream>

string resFile = "../bankdata/bankdata.txt";
bool justReturnCode = false;
int nextArg = 1;

void checkArg(string argument) {
  if (argument == "--returncode") {
	nextArg++;
	justReturnCode = true;
  }
  
  if (argument.length() > string("--file=").length()) {
	if (argument.substr(0, 7) == "--file=") {
	  resFile = argument.substr(7);
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
	cout << argv[0] << " [--returncode] [--file=datafile] <bank-id> <account-id>" << endl;
	cout << "  --file=<resource-file>: The file that contains the bankinformation" << endl;
	cout << "  --returncode: no output, result is returned via the returncode";
	cout << endl;
	cout << "                0: account/bank ok" << endl;
	cout << "                1: unknown" << endl;
	cout << "                2: account/bank not ok" << endl;
	cout << "                3: bank not found" << endl << endl;

	exit(1);
  }
  
#ifdef COMPILE_RESOURCE
  AccountNumberCheck::init();
#else
  AccountNumberCheck::init(resFile);
#endif

  bankId = argv[nextArg++];
  accountId = argv[nextArg++];
  bool found = true;

  AccountNumberCheck::Record bankData;
  try {
	bankData = AccountNumberCheck::findBank(bankId);
  } catch (int i) {
	found = false;
  }
  AccountNumberCheck::Result result = 
	AccountNumberCheck::check(bankId, accountId);

  if (! justReturnCode) {
	cout << "Bank: " << (found?(bankData.bankName + " " + bankData.location):"<unknown>") << " (" << bankId << ")" << endl;
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

  AccountNumberCheck::flush();
  return result;
}

