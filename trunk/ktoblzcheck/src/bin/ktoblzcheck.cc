/***************************************************************************
                             -------------------
    cvs         : $Id$
    begin       : Sat Aug 10 2002
    copyright   : (C) 2002 by Fabian Kaiser
    email       : fabian@openhbci.de
   modified     : m.plugge@fh-mannheim.de for file check

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

#include <stdio.h>
#include <ktoblzcheck.h>
#include <iostream>
#include <fstream>

using namespace std;

typedef enum _Outformat { MULTILINE, ONELINE } Outformat;
string bankId = "";
string accountId = "";
string bankdataFile = "";
string method = "";
string infile = "";
string outfile = "";
bool justReturnCode = false;
int nextArg = 1;
Outformat outformat = MULTILINE;

void printUsage()
{
      cout << "Usage:" << endl;
      cout << PACKAGE
	   << " [--returncode] [--file=datafile] <bank-id> <account-id>"
	   << endl
	   << "  --help: This help message"
	   << endl
	   << "  --bankdata-path: No checking, only print the default bankdata-path"
	   << endl
	   << "  --file=<bankdata-file>: Read bankdata from the specified file"
	   << endl
	   << "  --returncode: no output, result is returned via the returncode"
	   << endl
	   << "  --outformat=<format>: multiline (default), or oneline (tab-delimited) output"
	   << endl
	   << "  --infile=<filename>: Read many BLZ/accounts from filename, not cmdline"
	   << endl
	   << "  --outfile=<filename>: Write results to filename instead of stdout"
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
   string argn4 = "--infile=";
   string argn5 = "--outfile=";

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
   else if (argument.length() > string(argn3).length()
	    && argument.substr(0, string(argn3).length()) == argn3) {
      bankdataFile = argument.substr(string(argn3).length());
      nextArg++;
   }
   else if (argument.length() > string(argn4).length() && 
	    argument.substr(0, string(argn4).length()) == argn4) {
      infile = argument.substr(string(argn4).length());
      nextArg++;
   }
   else if (argument.length() > string(argn5).length() && 
	    argument.substr(0, string(argn5).length()) == argn5) {
      outfile = argument.substr(string(argn5).length());
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

   // Can we open the specified bankdata file? If not, use the
   // installed data file
   AccountNumberCheck *check_ptr;
   if (bankdataFile.empty())
      check_ptr = new AccountNumberCheck();
   else 
   {
      ifstream test(bankdataFile.c_str());
      if (test.fail())
      {
	 std::cerr << PACKAGE ": Warning: Specified bankdata file '"
		   << bankdataFile
		   << "' could not be opened. Trying default file."
		   << std::endl;
	 check_ptr = new AccountNumberCheck();
      } 
      else 
	 check_ptr = new AccountNumberCheck(bankdataFile);
   }

   AccountNumberCheck& checker = *check_ptr;
   AccountNumberCheck::Result finalresult = AccountNumberCheck::OK;

   if (!infile.empty())
   {
      // Read BLZ/Account from input file
      FILE *in,*out;
      if (infile == "-")
	 in = stdin;
      else
	 if (!(in=fopen(infile.c_str(),"r"))) {
	    fprintf(stderr,"kann infile \"%s\" nicht zum Lesen öffnen; Abbruch\n",infile.c_str());
	    exit(1);
	 }
      if (outfile.empty())
	 out = stdout;
      else
      {
	 if (!(out=fopen(outfile.c_str(),"w"))){
	    fprintf(stderr,"kann outfile \"%s\" nicht zum Schreiben öffnen; Abbruch\n",outfile.c_str());
	    exit(1);
	 }
      }
      AccountNumberCheck::Result result;
      string text;
      char *blz;
      char *kto;
      char *ptr;
#define LINEBUFSIZE 1024
      char linebuffer[LINEBUFSIZE];
      while(!feof(in)){
	 if (!fgets(linebuffer,LINEBUFSIZE,in)) break;
	 for(ptr=linebuffer;isspace(*ptr);ptr++);
	 blz=ptr;
	 while(isdigit(*ptr))ptr++;
	 *ptr++=0;
	 kto=ptr;
	 while(isdigit(*ptr))ptr++;
	 *ptr=0;
	 result = checker.check(blz,kto, method);
	 finalresult = (result==AccountNumberCheck::OK ? finalresult : result);
	 if (! justReturnCode) {
	    text = AccountNumberCheck::resultToString(result);
	    // FIXME: The output should be unified with the lower
	    // "oneline" output format!
	    fprintf(out,"%d blz: %s, kto: %s ==> %s\n",result,blz,kto,text.c_str());
	 }
      }
   }
   else
   {
      // Read BLZ/Account from cmdline
      bool found = true;
      AccountNumberCheck::Record bankData;
      try {
	 bankData = checker.findBank(bankId);
      } catch (int i) {
	 found = false;
      }

      finalresult = checker.check(bankId, accountId, method);

      if (! justReturnCode) {
	 switch (outformat) 
	 {
	    case MULTILINE:
	       cout << "Bank: " 
		    << (found ? 
			("'" + bankData.bankName + "' at '" + bankData.location + "'") :
			"<unknown>") << " (" << bankId << ")" << endl
		    << "Account: " << accountId << endl
		    << "Result is: " << "(" << finalresult << ") " 
		    << AccountNumberCheck::resultToString(finalresult) << endl
		    << endl;
	       break;
	    case ONELINE:
	       // FIXME: The output should be unified with the
	       // file-based output above!
	       string unknown("<unknown>"); 
	       cout << (found ? bankData.bankName : unknown) << "\t"
		    << (found ? bankData.location : unknown) << "\t"
		    << AccountNumberCheck::resultToString(finalresult) << "\t"
		    << bankId << "\t"
		    << accountId << "\t"
		    << (found ? bankData.method : unknown)
		    << endl;
	 }
      }
   }
   
   delete check_ptr;
   return finalresult;
}
