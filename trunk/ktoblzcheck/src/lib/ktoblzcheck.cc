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

#include "config.h"

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
    : 
    data() // std::map doesn't take size as argument
{
  // Disabled COMPILE_RESOURCE because the big list cannot be handled by the compiler anyway.

  string data_path = BANKDATA_PATH;
  string filename = data_path + "/bankdata.txt";
  readFile(filename);
}

AccountNumberCheck::AccountNumberCheck(const string& filename) {
  // I guess it is okay to simply accept this function call,
  // even if COMPILE_RESOURCE was activated.
  readFile(filename);
}

AccountNumberCheck::~AccountNumberCheck() 
{
  deleteList();
}


#define LINEBUFFER_SIZE 200
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
AccountNumberCheck::findBank(const string& bankId) const {  

  unsigned long lbankId = atol(bankId.c_str());
  banklist_type::const_iterator iter;
  iter = data.find(lbankId);

  if (iter != data.end()) {
    return *(iter->second);
  }
  
  throw -1;
}



AccountNumberCheck::Result 
AccountNumberCheck::check(const string& bankId, const string& accountId, 
			  const string& given_method) const 
{
  int account[10] = {9,1,3,0,0,0,0,2,0,1};
  int weight[10]  = {0,0,0,0,0,0,0,0,0,0};
  int transform[6][10] = 
      { {0,0,0,0,0,0,0,0,0,0}, // zero for non-transform [0][x]
	{0,1,5,9,3,7,4,8,2,6}, // first transform line   [1][x]
	{0,1,7,6,9,8,3,2,5,4}, // second transform line  [2][x]
	{0,1,8,4,6,2,9,5,7,3}, // third transform line   [3][x]
	{0,1,2,3,4,5,6,7,8,9}, // fourth transform line  [4][x]
	{1,4,3,2,1,4,3,2,1,0} }; // which transform line

  string method = given_method;

  if (method.empty()) {
    try {
      Record rec = findBank(bankId);
      method = rec.method;
    } catch (int i) {
	// bank was not found, return error if not forced to use a spec. method
	return BANK_NOT_KNOWN;
    }
  }

  //  method = "39";
  number2Array(accountId, account);

  if ("00" == method) {
	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);	
  }
  if ("01" == method) {
	number2Array("1731731730", weight);
	return algo01(10, weight, false, 10, account);
  }
  if ("02" == method) {
	number2Array("2987654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("03" == method) {
	number2Array("2121212120", weight);
	return algo01(10, weight, false, 10, account);	
  }
  if ("04" == method) {
	number2Array("4327654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("05" == method) {
	number2Array("1371371370", weight);
	return algo01(10, weight, false, 10, account);
  }
  if ("06" == method) {
	number2Array("4327654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("07" == method) {
	number2Array("987654320", weight);
	weight[0] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("08" == method) {
	if (atol(array2Number(account).c_str()) < 60000)
	  return OK;
	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);
  }
  if ("09" == method) {
	// no calculation -> no error
	return OK;
  }
  if ("10" == method) {
	number2Array("987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("11" == method) {
	number2Array("987654320", weight);
	weight[0] = 10;
	int tmp = algo03(11, weight, false, account, 0, 9);
	tmp = (11 - tmp) % 10;
	if (10 == tmp)
	  tmp = 9;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("12" == method) {
	// not used
  }
  if ("13" == method) {
	number2Array("121212000", weight);
	if (algo01(10, weight, true, 8, account) != OK) {
	  // shift left, add 00 as subaccount id and try again
	  number2Array(array2Number(account).substr(2) + "00", account);
	  if (OK != algo01(10, weight, true, 8, account))
		return ERROR;
	} 
	return OK;
  }
  if ("14" == method) {
	number2Array("0007654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("15" == method) {
	number2Array("54320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("16" == method) {
	number2Array("4327654320", weight);
	int tmp = algo03(11, weight, false, account, 0, 9);
	tmp = tmp % 11;
	if (1 == tmp && account[8] == account[9])
	  return OK;
	else
	  return algo01(11, weight, false, 10, account);
  }
  // Added by Jens Gecius, validated with one test accountID
  if ("17" == method) {
	number2Array("0121212000", weight);
        int tmp = algo03a(weight, true, account, 1, 6);
        tmp = (tmp - 1) % 11;
        tmp = 10 - tmp;
        if (10 == tmp)
            tmp = 0;
        if ( account[7] == tmp )
            return OK;
        else
            return ERROR;
        // FIXME
  }
  if ("18" == method) {
	number2Array("3179317930", weight);
	return algo01(10, weight, false, 10, account);
  }
  if ("19" == method) {
	number2Array("1987654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("20" == method) {
	number2Array("3987654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("21" == method) {
	int res[10];
	number2Array("2121212120", weight);

	// mult the weight with the account id
	multArray(account, weight, res);

	// for 00,...: calc the crossfoot for each value
	crossFoot(res, res, 0, 9);

	// add all values
	int result = add(res, 0, 9);

	// crossfoot for the result until only one digit is left
	while (result > 9) {
	  int tmp = result % 10;
	  result /= 10;
	  result += tmp;
	}	
	result = 10 - result;

	if (result == account[9])
	  return OK;
	else
	  return ERROR;
  }
  if ("22" == method) {
	number2Array("3131313130", weight);
	return algo01(10, weight, false, 10, account);
  }
  if ("23" == method) {
	number2Array("7654320", weight);
	int tmp = algo03(11, weight, false, account, 0, 9);
	tmp = tmp % 11;
	if (0 == tmp && 0 == account[6])
	  return OK;
	if (1 == tmp && account[5] == account[6])
	  return OK;
	if (1 < tmp && account[6] == (11 - tmp))
	  return OK;
	
	return ERROR;
  }
  if ("24" == method) {
	return algo05(accountId); 
  }
  // Added by Jens Gecius, validated with one test accountID
  if ("25" == method) {
        number2Array("987654320", weight);
        int result = algo03(11, weight, false, account, 1, 8);
        result = 11 - result;
        if (result == 11)
            result = 0;
        if (result == 10 && account[9] == 0 && (account[1] == 8 || account[1] == 9))
            return OK;
        if (result == account[9])
            return OK;
        return ERROR;
  }
  // Added by Jens Gecius, validated with three test accountIDs and one own accountID
  if ("26" == method) {
        if (account[0] == 0 && account[1] == 0)
            number2Array(array2Number(account).substr(2) + "00", account);
        number2Array("2765432000", weight);
        return algo01(11, weight, false, 8, account);
  }
  // Added by Jens Gecius, validated with nine own accountIDs + testaccountID (six > 1000000000)
  if ("27" == method) {
        if (atol(array2Number(account).c_str()) < 1000000000) {
            number2Array("2121212120", weight);
            return algo01(10, weight, true, 10, account);
        } else {
        // iterated transformation
            return algo07(account, transform);
        }
        return ERROR;
  }
  if ("28" == method) {
	number2Array("8765432000", weight);
	return algo01(11, weight, false, 8, account);
  }
  // Added by Jens Gecius, validated with test accountID
  if ("29" == method) {
        return algo07(account, transform);
  }
  if ("30" == method) {
	number2Array("2000012120", weight);
	return algo01(10, weight, false, 10, account);
  }
  // Changed by Jens Gecius, validated with two test accountIDs
  if ("31" == method) {
	number2Array("1234567890", weight);
	int result = algo03(11, weight, false, account, 0, 9);

	// no rest after division by modulus (in algo03)?
	// then the the result will be 0! (as it is... removed next two lines
        // as there is no further calculation
        // if (0 == result)
	//  result = 11;

        // next two lines removed, as 10 can never be a digit in accountID
	// if (10 == result)
	//  return ERROR;

	if (result == account[9])
	  return OK;
        // to stay in line with convention so far, remove next line
	// else
        return ERROR;
  }
  if ("32" == method) {
	number2Array("0007654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("33" == method) {
	number2Array("0000654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("34" == method) {
	number2Array("7905842000", weight); weight[2] = 10;
	return algo01(11, weight, false, 8, account);
  }
  // Added by Jens Gecius, not validated due to missing bank using this algo
  if ("35" == method) {
        number2Array("0987654320",weight); weight[0] = 10;
        int result = algo03(11, weight, false, account, 0, 9);
        if (result == 10 && account[9] == account[8])
            return OK;
        if (result == account[9])
            return OK;
        return ERROR;
  }
  if ("36" == method) {
	number2Array("0000058420", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("37" == method) {
	number2Array("0000058420", weight); weight[4] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("38" == method) {
	number2Array("0009058420", weight); weight[4] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("39" == method) {
	number2Array("0079058420", weight); weight[4] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("40" == method) {
	number2Array("6379058420", weight); weight[4] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("41" == method) {
	number2Array("2121212120", weight);
	if (9 == account[3]) {
	  weight[0] = 0; 
	  weight[1] = 0;
	  weight[2] = 0;
	}
	return algo01(10, weight, true, 10, account);	
  }
  if ("42" == method) {
	number2Array("0987654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  // Added by Jens Gecius, validated with two test accountIDs
  if ("43" == method) {
        number2Array("9876543210", weight);
        return algo01(10, weight, false, 10, account);
  }
  if ("44" == method) {
	number2Array("0000058420", weight); weight[4] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("45" == method) {
	// some numbers do not have a checksum
	if (0 == account[0] || 1 == account[4])
	  return OK;

	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);	
  }
  if ("46" == method) {
	number2Array("0065432000", weight);
	return algo01(11, weight, false, 8, account);
  }
  if ("47" == method) {
	number2Array("0006543200", weight);
	return algo01(11, weight, false, 9, account);
  }
  if ("48" == method) {
	number2Array("0076543200", weight);
	return algo01(11, weight, false, 9, account);
  }
  if ("49" == method) {
	// try with method == 00
	number2Array("2121212120", weight);
	if (OK == algo01(10, weight, true, 10, account))
	  return OK;
	else {
	  // on error try with method == 01
	  number2Array("1731731730", weight);
	  return algo01(10, weight, false, 10, account);
	}
  }
  if ("50" == method) {
	number2Array("7654320000", weight);
	Result tmp = algo01(11, weight, false, 7, account);
	if (OK == tmp)
	  return OK;
	else {
	  // on error, shift left by 3 and try again
	  number2Array(array2Number(account).substr(3) + "000", account);
	  return algo01(11, weight, false, 7, account);
	}
  }
  // Added by Jens Gecius, validated with six test accountIDs
  if ("51" == method) {
        number2Array("0007654320", weight);
        if (OK == algo01(11, weight, false, 10, account))
            return OK;	// Method A
        number2Array("0000654320", weight);
        if (OK == algo01(11, weight, false, 10, account))
            return OK;  // Method B
        if (7 == account[9] || 8 == account[9] || 9 == account[9])
            return ERROR;  // Invalid IDs
        if (9 == account[2] && 9 == account[3]) {
            	number2Array("987654320", weight); weight[0] = 10;
                return algo01(11, weight, false, 10, account);
        }		   // Exception to method C
        return algo01(7, weight, false, 10, account); // Method C
  }
  if ("52" == method) {
        // Changed by Jens Gecius, only 10 digit accountIDs starting with "9",
        // *NOT* validated
        if (10 == accountId.length() && 9 == account[0]) {
            number2Array("3987654320", weight);
            return algo01(11, weight, false, 10, account);
        }
        return algo04(bankId, accountId);
	// return UNKNOWN;
	//	return algo04(bankId, accountId);
  }
  // Added by Jens Gecius, *NOT* validated
  if ("53" == method) {
        // Changed by Jens Gecius, only 10 digit accountIDs starting with "9",
        // *NOT* validated
        if (10 == accountId.length() && 9 == account[0]) {
            number2Array("3987654320", weight);
            return algo01(11, weight, false, 10, account);
        }
        return algo04(bankId, accountId);
  }
  if ("54" == method) {
	// ids must start with 49
	if (! (4 == account[0] && 9 == account[1]))
	  return ERROR;

	number2Array("0027654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("55" == method) {
	number2Array("8787654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  // Added by Jens Gecius, validated with two test accountIDs
  if ("56" == method) {
        number2Array("4327654320", weight);
        int result = 11 - algo03(11, weight, false, account, 0, 9);
        if (result == account[9])
            return OK;
        return ERROR;
  }
  if ("57" == method) {	// validated with 8 test accountIDs (4 with + 4 w/o checksum)
	// many numbers do not have a checksum:
	int firstTwo = atoi(array2Number(account).substr(0, 2).c_str());
	int firstSix = atoi(array2Number(account).substr(0, 6).c_str());
	if ((51 > firstTwo) || (91 == firstTwo) || (95 < firstTwo) ||
		(777777 == firstSix) || (888888 == firstSix))
	  return OK;

	number2Array("1212121210", weight);
	return algo01(10, weight, true, 10, account);
  }
  // Added by Jens Gecius, validated with four test accountIDs
  if ("58" == method) {
        if (0 == atoi(array2Number(account).substr(0,5).c_str()))
            return ERROR;
        number2Array("0000654320", weight);
        return algo01(11, weight, false, 10, account);
  }
  if ("59" == method) {
	// ids less than 9 digits can not be checked
	if (0 == account[0] && 0 == account[1])
	  return OK;

	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);
  }
  if ("60" == method) {
	number2Array("0021212120", weight);
	return algo01(10, weight, true, 10, account);
  }
  if ("61" == method) {
	number2Array("2121212000", weight);
	if (8 == account[8])
	  number2Array("2121212012", weight);
	return algo01(10, weight, true, 8, account);
  }
  // Added by Jens Gecius, validated with one test accountID
  if ("62" == method) {
        number2Array("0021212000", weight);
        return algo01(10, weight, true, 8, account);
  }
  if ("63" == method) {
        if (0 != account[0])
            return ERROR; // Added by Jens Gecius, check on invalid accountIDs
	number2Array("0121212000", weight);
	Result tmp = algo01(10, weight, true, 8, account);
	if (OK == tmp)
	  return OK;
	else {
	  // shift left, add 00 as subaccount id and try again
	  number2Array(array2Number(account).substr(2) + "00", account);
	  return algo01(10, weight, true, 8, account);
	}
  }
  if ("64" == method) {
	number2Array("9058420000", weight); weight[1] = 10;
	return algo01(11, weight, false, 7, account);
  }
  if ("65" == method) {
	number2Array("2121212000", weight);
	if (9 == account[8]) {
	  weight[8] = 1;
	  weight[9] = 2;
	}
	return algo01(10, weight, true, 8, account);
  }
  // Added by Jens Gecius, validated with five test accountIDs
  if ("66" == method) {
        if (0 != account[0])
            return ERROR;
        number2Array("700654320", weight);
        int result = algo03(11, weight, false, account, 0, 9);
        if (result == 0)
            result = 1;
        else if (result == 1)
            result = 0;
        else
            result = 11 - result;
        if (account[9] == result)
            return OK;
        return ERROR;
  }
  if ("67" == method) {
	number2Array("2121212000", weight);
	return algo01(10, weight, true, 8, account);	
  }
  if ("68" == method) {
        // Modified by Jens Gecius, verified with three test accountIDs
	// size=10
	if (0 != account[0]) {
	  // and digit 4 is not "9"? error
	  if (9 != account[3])
		return ERROR;
	  number2Array("0001212120", weight);
	  if (OK == algo01(10, weight, true, 10, account))
		return OK;
	} else { // size != 10
            // some accountIDs can not be checked:
            if ("400000000" <= array2Number(account) &&
                array2Number(account) <= "499999999")
                return OK;
            number2Array("0121212120", weight);
            if (OK == algo01(10, weight, true, 10, account))
                // variant 1
		return OK;
            else {
                // variant 2
		number2Array("0100212120", weight);
		if (OK == algo01(10, weight, true, 10, account))
                    return OK;
            }
	}

	return ERROR;
  }
  // Added by Jens Gecius, validated with three test accountIDs
  if ("69" == method) {
        long int lastnine = atol(array2Number(account).substr(1).c_str());
        if (account[0] == 9 && lastnine >= 300000000 && lastnine <= 399999999)
            return OK; // these do not get checked
        if ((account[0] == 9 && (lastnine <= 700000000 || lastnine >= 799999999)) ||
            account[0] != 9) {
            // variant 1
            number2Array("8765432000", weight);
            if (OK == algo01(11, weight, false, 8, account))
                return OK;
        }
        // variant 2
        return algo07(account, transform);
  }
  if ("70" == method) {
	number2Array("4327654320", weight);
	if (5 == account[3] ||
		(6 == account[3] && 9 == account[4]))
	  number2Array("0007654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("71" == method) {
	number2Array("0654321000", weight);	
	int tmp = algo03(11, weight, false, account, 0, 9);

	// no rest after division by modulus (in algo03)?
	if (0 == tmp)
          // Changed by Jens Gecius, validated with one test accountID
	  // this way we get 0 as final result
	  tmp = 11;
        // if 1 rest, keep it! 
        if (1 == tmp)
            tmp = 10;

	// and calc the check number
	tmp = 11 - tmp;
	if (tmp == account[9])
	  return OK;
	return ERROR;
  }
  if ("72" == method) {
	number2Array("0001212120", weight);
	return algo01(10, weight, true, 10, account);
  }
  // Added by Jens Gecius, validated with two test accountIDs
  if ("73" == method) {
        if (account[2] == 9) {
            number2Array("0987654320", weight); weight[0] = 10;
            return algo01(11, weight, false, 10, account);
        }
        number2Array("0001212120", weight);
        return algo01(10,weight, true, 10, account);
  }
  // Added by Jens Gecius, validated with one (double) test accountID
  if ("74" == method) {
        number2Array("2121212120", weight);
        if (OK == algo01(10, weight, true, 10, account))
            return OK;
        if (accountId.length() < 7) {
            if (account[9] == (5 - (algo03a(weight, true, account, 0, 9) % 5)))
                return OK;
        }
        return ERROR;
  }
  if ("75" == method) {
  // Modified by Jens Gecius
	int checkIndex;
	if ("000" == array2Number(account).substr(0, 3)) {
	  checkIndex = 10;
	  number2Array("0000212120", weight);
	} else if ("09" == array2Number(account).substr(0, 2)) {
	  checkIndex = 8;
	  number2Array("0021212000", weight);
	} else {
	  checkIndex = 7;
	  number2Array("0212120000", weight);
	}
	return algo01(10, weight, true, checkIndex, account);
  }
  if ("76" == method) {
        // Modified by Jens Gecius, validated with four test accountIDs
	number2Array("0765432000", weight);
	int tmp = algo03(11, weight, false, account, 0, 6);

	if (tmp == account[7] && (account[0] == 0 || account[0] == 4 || account[0] > 5) )
	  return OK;
	else if (account[0] == 0 && account[1] == 0) {
	  // shift left if two lefts are 0, add 00 as subaccount id and try again
	  number2Array(array2Number(account).substr(2) + "00", account);
	  tmp = algo03(11, weight, false, account, 0, 6);
	  if (tmp == account[7] && (account[0] == 0 || account[0] == 4 || account[0] > 5) )
		return OK;
	}
	return ERROR;
  }
  // Added by Jens Gecius, validated with four test accountIDs
  if ("77" == method) {
        number2Array("0000054321", weight);
        if (0 == algo03(11, weight, false, account, 0, 9))
            return OK;
        weight[8] = 4; weight[9] = 5;
        if (0 == algo03(11, weight, false, account, 0, 9))
            return OK;
        return ERROR;
  }
  if ("78" == method) {
	if (0 == account[0] && 0 == account[1])
	  return OK;
	
	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);	
  }
  if ("79" == method) {
        // Modified by Jens Gecius, not verified as no bank seems to use it...
	if (0 == account[0])
	  return ERROR;
	int checkIndex = 10;

	if (2 < account[0] && 9 > account[0])
	  number2Array("2121212120", weight);
	else {
	  checkIndex--;
	  number2Array("1212121200", weight);
	}

	return algo01(10, weight, true, checkIndex, account);
  }
  if ("80" == method) {
	// exception:
	if (9 == account[2] && 9 == account[3]) {
	  number2Array("987654320", weight); weight[0] = 10;
	  return algo01(11, weight, false, 10, account);
	}

	// var 1
	number2Array("0000212120", weight);
	if (OK == algo01(10, weight, true, 10, account))
	  return OK;

	// var 2:
	return algo01(7, weight, true, 10, account);
  }
  if ("81" == method) {
        // Modified by Jens Gecius, validated with three test accountIDs
	number2Array("0987654320", weight); weight[0] = 10;
	if (9 != account[2]) {
	  weight[1] = 0; weight[2] = 0;
	}
	return algo01(11, weight, false, 10, account);
  }
  if ("82" == method) {
	if (9 == account[2] && 9 == account[3]) {
	  number2Array("987654320", weight); weight[0] = 10;
	  return algo01(11, weight, false, 10, account);
	} else {
	  number2Array("0000654320", weight);
	  return algo01(11, weight, false, 10, account);
	}
  }
  if ("83" == method) {
	// var a:
	number2Array("0007654320", weight);
	if (OK == algo01(11, weight, false, 10, account))
	  return OK;

	// var b:
	number2Array("0000654320", weight);
	if (OK == algo01(11, weight, false, 10, account))
	  return OK;

	// var c:
	if (account[9] > 6)
	  return ERROR;
	if (OK == algo01(7, weight, false, 10, account))
	  return OK;

	// non-customer accounts
	if (9 != account[2] || 9 != account[3])
	  return ERROR;

	number2Array("0087654320", weight);
	return algo01(11, weight, false, 10, account);	
  }
  if ("84" == method) {
	// exception:
	if (9 == account[2] && 9 == account[3]) {
	  number2Array("987654320", weight); weight[0] = 10;
	  return algo01(11, weight, false, 10, account);	  	  
	}

	// var 1
	number2Array("0000654320", weight);
	if (OK == algo01(11, weight, false, 10, account))
	  return OK;

	// var2
	return algo01(7, weight, false, 10, account);
  }
  if ("85" == method) {
	if (9 == account[2] && 9 == account[3]) {
	  number2Array("0087654320", weight);
	  return algo01(11, weight, false, 10, account);
	}

	// var a:
	number2Array("0007654320", weight);
	if (OK == algo01(11, weight, false, 10, account))
	  return OK;

	// var b:
	number2Array("0000654320", weight);
	if (OK == algo01(11, weight, false, 10, account))
	  return OK;

	// var c:
	if (account[9] > 6)
	  return ERROR;

	number2Array("0000654320", weight);
	return algo01(7, weight, false, 10, account);
  }
  if ("86" == method) {
	// exception:
	if (9 == account[2]) {
	  number2Array("987654320", weight); weight[0] = 10;
	  return algo01(11, weight, false, 10, account);	  
	}

	// var 1
	number2Array("0001212120", weight);
	if (OK == algo01(10, weight, true, 10, account))
	  return OK;

	// var2: on error try with 32
	number2Array("0007654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("87" == method) {
	if (9 == account[2]) {
	  // calc with method 10
	  number2Array("987654320", weight); weight[0] = 10;
	  return algo01(11, weight, false, 10, account);
	}

	// method a
	if (OK == algo06(accountId))
	  return OK;

	// method b
	number2Array("0000654320", weight);
	if (OK == algo01(11, weight, false, 10, account))
	  return OK;

	// method c
	return algo01(7, weight, false, 10, account);
  }
  if ("88" == method) {
	number2Array("0007654320", weight);
	if (9 == account[2])
	  number2Array("0087654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("89" == method) {
	// some numbers can not be checked
	string acc10 = array2Number(account);
	if (acc10 < "0000999999" || 0 != account[0])
	  return OK;

	// those with length 7
	if (0 == account[0] && 0 == account[1] && 0 == account[2]) {
	  number2Array("0007654320", weight);
	  return algo01(11, weight, true, 10, account);
	}

	// length 8 or 9? check with method 10
	number2Array("987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("90" == method) {
	// variant a
	number2Array("0007654320", weight);	
	if (OK == algo01(11, weight, false, 10, account))
	  return OK;

	// variant b
	number2Array("0000654320", weight);
	if (OK == algo01(11, weight, false, 10, account))
	  return OK;
	
	// variant c
	if (OK == algo01(07, weight, false, 10, account))
	  return OK;
	
	// variant d
	if (OK == algo01(9, weight, false, 10, account))
	  return OK;

	// variant e
	number2Array("0000212120", weight);
	if (OK == algo01(10, weight, false, 10, account))
	  return OK;

	// non-customer-accounts
	number2Array("0087654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("91" == method) {
	number2Array("7654320000", weight);
	Result tmp = algo01(11, weight, false, 7, account);
	if (OK == tmp)
	  return OK;
	else {	
	  number2Array("2345670000", weight);
	  tmp = algo01(11, weight, false, 7, account);
	  if (OK == tmp)
		return OK;
	  else {	
		number2Array("0987650432", weight); weight[0] = 10;	
		return algo01(11, weight, false, 7, account);
	  }
	}
  }
  if ("92" == method) {
	number2Array("0001731730", weight);
	return algo01(10, weight, false, 10, account);
  }
  if("93" == method) {
        // Modified by Jens Gecius, indirectly validated via method A4,
        // as no bank seems to use it...
	int checkIndex = 6;
	number2Array("6543200000", weight);
	if ("0000" == array2Number(account).substr(0, 4)) {
	  checkIndex = 10;
	  number2Array("0000654320", weight);	  
	}

	if (OK == algo01(11, weight, false, checkIndex, account))
	  return OK;
	int result = algo03(7, weight, false, account, 0, 9);
        if (result == 0)
            result = 7;
        if (account[checkIndex] == (7 - result))
            return OK;
        return ERROR;
  }
  if ("94" == method) {
	number2Array("1212121210", weight);
	return algo01(10, weight, true, 10, account);
  }
  if ("95" == method) {
	// some numbers do not get checked
	string accNumber = array2Number(account);
	if (("0000000001" <= accNumber && accNumber <= "0001999999") ||
            ("0009000000" <= accNumber && accNumber <= "0025999999") ||
            ("0396000000" <= accNumber && accNumber <= "0499999999") ||
            ("0700000000" <= accNumber && accNumber <= "0799999999"))
	  return OK;

	number2Array("4327654320", weight);

	return algo01(11, weight, false, 10, account);
  }
  if ("96" == method) {
        // Modified by Jens Gecius
	number2Array("1987654320", weight);
	if (OK == algo01(11, weight, false, 10, account))
            return OK;
        // check with 00
        number2Array("2121212120", weight);
        if (OK == algo01(10, weight, true, 10, account))
            return OK;
        string acc10 = array2Number(account);
        if (acc10 > "0001300000" || acc10 < "0099399999")
            return OK;
	return ERROR;
  }
  if ("97" == method) {
	string tmp = array2Number(account).substr(0, 9);
	long long valueX = number2LongLong(tmp);
	valueX -= (valueX / 11) * 11;
	valueX = valueX % 10;
	
	if (valueX == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("98" == method) {
	number2Array("0037137130", weight);
	if (OK == algo01(10, weight, false, 10, account))
	  return OK;

	// on error run method 32
	number2Array("0007654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("99" == method) {
        // some numbers do not get checked
	if ("0396000000" <= array2Number(account) && 
		array2Number(account) <= "0499999999")
	  return OK;
	number2Array("4327654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("A0" == method) {
	// some numbers are not checked
	if ("0000000" == array2Number(account).substr(0, 7))
	  return OK;
        // Modified by Jens Gecius, NOT validated as no bank seems to use it
	number2Array("0000058420", weight); weight[4] = 10;
	int result = algo03a(weight, false, account, 0, 9);
        result = result % 11;
        if (result == 0 || result == 1)
            result = 11;
        if (account[9] == result)
            return OK;
        return ERROR;
  }
  if ("A1" == method) {
        // Modified by Jens Gecius as of recent changes, validated with six test accountIDs
        // three positives and three negatives (first algo to check negatives)
        string acc10 = array2Number(account);
        if ((acc10 < "1000000000" && acc10 > "0099999999") ||
             acc10 < "0010000000")
                return ERROR;
	number2Array("0021212120", weight);
	return algo01(10, weight, true, 10, account);
  }
  // Added by Jens Gecius, validated with six test accountIDs (4 positives, 2 negatives)
  if ("A2" == method) {
        number2Array("2121212120", weight);
        if (OK == algo01(10, weight, true, 10, account))
            return OK;
        number2Array("4327654320", weight);
        return algo01(11, weight, false, 10, account);
  }
  // Added by Jens Gecius, NOT validated as no bank seems to use it...
  if ("A3" == method) {
        number2Array("2121212120", weight);
        if (OK == algo01(10, weight, true, 10, account))
            return OK;
        number2Array("0987654320", weight); weight[0] = 10;
        return algo01(11, weight, false, 10, account);
  }
  // Added by Jens Gecius, validated with 16 test accountIDs (11 pos
  // and 5 negs). The test accountIDs chosen in the Bundesbank paper
  // are stupid, as some of the false ones are actually correct as of
  // other variants also relevant for them!!!  costed me another hour
  // calculating all of them by hand! grr.
  if ("A4" == method) {
        number2Array("0000654320", weight);
        if (account[2] == 9 && account[3] == 9) { // variant 3
            if (OK == algo01(11, weight, false, 10, account))
                return OK;
        } else {
            number2Array("0007654320", weight);
            if (OK == algo01(11, weight, false, 10, account)) // variant 1
                return OK;
            int result = algo03(7, weight, false, account, 0, 9); // variant 2
            if (result == 0)
                result = 7;
            if (account[9] == (7 - result))
                return OK;
        }
	int checkIndex = 6;		// variant 4
	number2Array("6543200000", weight);
	if ("0000" == array2Number(account).substr(0, 4)) {
	  checkIndex = 10;
	  number2Array("0000654320", weight);	  
	}

	if (OK == algo01(11, weight, false, checkIndex, account))
	  return OK;
	int result = algo03(7, weight, false, account, 0, 9);
        if (result == 0)
            result = 7;
        if (account[checkIndex] == (7 - result))
            return OK;
        return ERROR;
  }
  // Added by Jens Gecius, NOT validated as no bank seems to use it...
  if ("A5" == method) {
        number2Array("2121212120", weight);
        if (OK == algo01(10, weight, true, 10, account))
            return OK;
        if (account[0] == 9)
            return ERROR;
        number2Array("0987654320", weight); weight[0] = 10;
        return algo01(11, weight, false, 10, account);
  }
  if ("A6" == method) {
        if (account[1] == 8) {
            number2Array("2121212120", weight);
            return algo01(10, weight, true, 10, account);
        } else {
            number2Array("1731731730", weight);
            return algo01(10, weight, false, 10, account);
        }
  }
  if ("A7" == method) {
        number2Array("2121212120", weight);
        if (OK == algo01(10, weight, true, 10, account))
            return OK;
        return algo01(10, weight, false, 10, account);
  }
  if ("A8" == method) {
	number2Array("0987654320", weight); weight[0] = 10;
	if (9 != account[2])
	  weight[1] = weight[2] = 0;
	if (OK == algo01(11, weight, false, 10, account))
	    return OK;
        if (account[2] == 9)
            return ERROR;
        number2Array("0001212120", weight);
        return algo01(10,weight, true, 10, account);
  }
  std::cerr << "AccountNumberCheck::check: Specified method '" << method 
	    << "' is unknown." << std::endl;
  return UNKNOWN;
}


