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

int main(int argc, char **argv) {
  bool justReturnCode = false;
  int nextArg = 1;
  string bankId;
  string accountId;

  if (argc > 1 && string(argv[1]) == "--returncode") {
	justReturnCode = true;
	nextArg++;
  }
	
  if ((justReturnCode && argc < 4) || (!justReturnCode && argc < 3)) {
	cout << "Usage:" << endl;
	cout << "./ktoblzcheck --returncode <bank-id> <account-id>" << endl;
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
  AccountNumberCheck::init("../bankdata/bankdata.txt");
#endif

  bankId = argv[nextArg++];
  accountId = argv[nextArg++];

  AccountNumberCheck::Result result = 
	AccountNumberCheck::check(bankId, accountId);

  if (! justReturnCode) {
	cout << "Bank-id " << bankId << " and Account-id " << accountId << endl;
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

