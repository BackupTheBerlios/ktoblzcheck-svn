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

#ifdef HAVE_CONFIG_H
# include <config.h>
#else
# define VERSION ""
# define PACKAGE "ktoblzcheck"
#endif

#include <ktoblzcheck.h>
#include <iostream>
#include <fstream>

using namespace std;

typedef enum _Outformat { MULTILINE, ONELINE } Outformat;
string bankId = "";
string accountId = "";
string resFile = "";
string method = "";
bool justReturnCode = false;
int nextArg = 1;
Outformat outformat = MULTILINE;

void printUsage()
{
      cout << "Usage:" << endl;
      cout << PACKAGE
	   << " [--returncode] [--file=datafile] <bank-id> <account-id>"
	   << endl
	   << "  --file=<resource-file>: The file that contains the bankinformation" 
	   << endl
	   << "  --bankdata-path: No checking, only print the bankdata-path"
	   << endl
	   << "  --returncode: no output, result is returned via the returncode"
	   << endl
	   << "  --outformat=<format>: multiline (default), or oneline (tab-delimited) output"
	   << endl;

      cout
	 << "                0: Everything is ok: account and bank match" << endl
	 << "                1: Validation algorithm is unknown/unimplemented" << endl
	 << "                2: Account and bank do not match" << endl
	 << "                3: No bank with given bank-id found" << endl 
	 << endl;

      exit(1);
}

void checkArg(string argument) {
   string argn1 = "--method=";
   string argn2 = "--outformat=";
   string argn3 = "--file=";

   if (argument == "--version" || argument == "-V")
   {
      cout << PACKAGE " " VERSION << endl;
      exit(0);
   }
   else if (argument == "--help" || argument == "-h")
      printUsage();

   else if (argument == "--bankdata-path") {
      std::cout << "BANKDATA_PATH=" << BANKDATA_PATH << std::endl;
      exit(0);
   }
  
   else if (argument == "--returncode") {
      nextArg++;
      justReturnCode = true;
   }

   else if (argument.length() > string(argn3).length()
	    && argument.substr(0, string(argn3).length()) == argn3) {
      resFile = argument.substr(7);
      nextArg++;
   }

   else if (argument.length() > string(argn1).length() && 
	    argument.substr(0, string(argn1).length()) == argn1) {
      method = argument.substr(string(argn1).length());
      nextArg++;
   }

   else if (argument.length() > string(argn2).length() && 
	    argument.substr(0, string(argn2).length()) == argn2) {
      string argvalue = argument.substr(string(argn2).length());
      if (argvalue == "multiline")
	 outformat = MULTILINE;
      else if (argvalue == "oneline")
	 outformat = ONELINE;
      else
	 std::cerr << PACKAGE ": The parameter --outformat has an unknown value. Allowed values are \"multiline\" and \"oneline\"." << endl;
      
      nextArg++;
   }
   else 
   {
      if (bankId.empty())
	 bankId = argument;
      else if (accountId.empty())
	 accountId = argument;
      else
	 std::cerr << PACKAGE ": Warning: extra command line arguments ignored" << endl;
   }
}

int main(int argc, char **argv) {

   for (int i=1; i<argc; i++) {
      checkArg(argv[i]);
   }

   if (argc - nextArg < 2) 
      printUsage();

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
	 std::cerr << PACKAGE ": main: Specified file '" << resFile
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
      switch (outformat) 
      {
	 case MULTILINE:
	    cout << "Bank: " 
		 << (found ? 
		     ("'" + bankData.bankName + "' at '" + bankData.location + "'") :
		     "<unknown>") << " (" << bankId << ")" << endl
		 << "Account: " << accountId << endl
		 << "Result is: " << "(" << result << ") " 
		 << AccountNumberCheck::resultToString(result) << endl
		 << endl;
	    break;
	 case ONELINE:
	    string unknown("<unknown>"); 
	    cout << (found ? bankData.bankName : unknown) << "\t"
		 << (found ? bankData.location : unknown) << "\t"
		 << AccountNumberCheck::resultToString(result) << "\t"
		 << bankId << "\t"
		 << accountId << "\t"
		 << (found ? bankData.method : unknown)
		 << endl;
      }
   }

   delete check_ptr;
   return result;
}

