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
#include <fstream>

string resFile = "../bankdata/bankdata.txt";
string method = "";
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

  // This is the location of the installed data file
  string default_path = BANKDATA_PATH;
  string default_filename = default_path + "/bankdata.txt";

  // Can we open the specified data file? If not, use the installed data file
  ifstream test(resFile.c_str());
  if (test.fail())
      resFile = default_filename;

  // skip the init-stuff, we don't need the banknames
  // if method is forced
  //  if ("" == method) {  
  //#ifdef COMPILE_RESOURCE
  //AccountNumberCheck checker();
  //#else
  AccountNumberCheck checker(resFile);
  //#endif
  //  }

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

  return result;
}

