/***************************************************************************
                             -------------------
    cvs         : $Id$
    begin       : Sat Aug 10 2002
    copyright   : (C) 2002, 2003 by Fabian Kaiser and Christian Stimming
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
# include "config.h"
#endif

#include "ktoblzcheck.h"
#include <fstream>
#include <iostream>
#include <algorithm>

// The actual algorithms for number checking are there
#include "algorithms.h"

// This file has the implementations for the class methods. C wrappers
// are in accnum.cc.

using namespace std;


AccountNumberCheck::Record::Record()
{
}
AccountNumberCheck::Record::Record(unsigned long id, 
				   const string& meth, 
				   const string& name, 
				   const string& loc)
  : bankId(id)
    , method(meth)
    , bankName(name)
    , location(loc)
{
}


AccountNumberCheck::AccountNumberCheck() 
    : data() // std::map doesn't take size as argument
{
   string registry_path = accnum_getRegKey("datadir");
   string data_path = BANKDATA_PATH;
   string filename = (registry_path.empty() ? data_path : registry_path)
      + 
#if OS_WIN32
      "\\"
#else
      "/"
#endif
      + std::string("bankdata.txt");
   readFile(filename);
}

AccountNumberCheck::AccountNumberCheck(const string& filename) 
    : 
    data() // std::map doesn't take size as argument
{
  readFile(filename);
}

AccountNumberCheck::~AccountNumberCheck() 
{
  deleteList();
}


#define LINEBUFFER_SIZE 250
void 
AccountNumberCheck::readFile(const string &filename) 
{
  // First clear existing data 
  if (data.size() > 0)
    deleteList();

  // Now read file
  ifstream file(filename.c_str());
  // FIXME: Do a lot of error checking here.
  if (file.fail())
    {
      std::cerr << "AccountNumberCheck::readFile: File " << filename 
		<< " could not be opened. "
	"AccountNumberCheck could not obtain bank data." << std::endl;
      return;
    }
  
  char *buffer = new char[LINEBUFFER_SIZE];
  string line;
  while (file) {
        // get line from file
	file.getline(buffer, LINEBUFFER_SIZE);
	// copy line to string variable
	line.assign(buffer);
	if (line.length() < 10)
	  break;

	Record *newRecord = new Record;

	// bankId
	unsigned int pos1;
	unsigned int pos2;
	pos1 = line.find('\t');
	newRecord->bankId = atol(line.substr(0, pos1).c_str());
	pos1++;

	// check-type
	newRecord->method = line.substr(pos1, 2);
	pos1 += 3;

	// bank name
	pos2 = line.find('\t', pos1);
	newRecord->bankName = line.substr(pos1, pos2 - pos1);
	pos1 = pos2 + 1;

	// bank location
	newRecord->location = line.substr(pos1);

	data.insert(banklist_type::value_type(newRecord->bankId, newRecord));
  }
}

void 
AccountNumberCheck::deleteList() 
{
  for (banklist_type::iterator iter = data.begin(); iter != data.end(); iter++) {
    delete iter->second;
  }
}


unsigned int AccountNumberCheck::bankCount() const {
  return data.size();
}

void AccountNumberCheck::createIndex() {
  // not yet implemented; for std::map this isn't necessary anyway.
}



// Function object for the predicate of matching a job result
class MatchBlz {
    unsigned long blz;
  public:
    MatchBlz(unsigned long bankId) 
	: blz(bankId) { };
    bool operator()(const AccountNumberCheck::Record *r) { 
	return r && (r->bankId == blz);
    }
};

const AccountNumberCheck::Record& 
AccountNumberCheck::findBank(const string& bankId) const 
{
  unsigned long lbankId = atol(bankId.c_str());
  banklist_type::const_iterator iter;

  // Lookup the object
  iter = data.find(lbankId);

  // Did we find it? Yes, return the reference.
  if (iter != data.end()) {
    return *(iter->second);
  }
  else
    throw -1;
}



AccountNumberCheck::Result 
AccountNumberCheck::check(const string& bankId, const string& accountId, 
			  const string& given_method) const 
{
  int account[10] = {9,1,3,0,0,0,0,2,0,1};
  int weight[10]  = {0,0,0,0,0,0,0,0,0,0};

  string method = given_method;

  if (method.empty()) {
    try {
      Record rec = findBank(bankId);
      method = rec.method;
    } catch (int) {
	// bank was not found, return error if not forced to use a spec. method
	return BANK_NOT_KNOWN;
    }
  }

  // Convert string to array of integers
  number2Array(accountId, account);

  // Note: Originally, the program code for the method was chosen by
  // the "giant if-statement". I thought that this is quite a
  // performance bottleneck, so I changed the code to make one small
  // function for each method, and its lookup is done through a
  // std::map hash table. However, this sped up the number checking
  // only by roughly 15%, so it probably wasn't worth the effort :-(
  // -- cstim, 2005-01-16

  // Initialize method map, if it has been empty
  if (method_map.empty())
     const_cast<AccountNumberCheck*>(this)->initMethodMap();

  // Find method function
  method_map_t::const_iterator iter = method_map.find(method);
  // Did we find it? Yes, use the returned function.
  if (iter != method_map.end()) {
      return (iter->second)(account, weight);
  }

  // If it hasnt been found, try the second map for methods that also
  // want the accountId string.
  method_map2_t::const_iterator iter2 = method_map2.find(method);
  // Did we find it? Yes, use the returned function.
  if (iter2 != method_map2.end()) {
     return (iter2->second)(account, weight, accountId, bankId);
  }

  std::cerr << "AccountNumberCheck::check: Specified method '" << method 
	    << "' is unknown." << std::endl;
  return UNKNOWN;
}


// The code for the actual methods is now in methods.cc. For adding a
// method, see the comment at the beginning of that file.
