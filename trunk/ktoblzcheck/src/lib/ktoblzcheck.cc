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
#include "ktoblzcheck.h"
#include <fstream>
#include <iostream>

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

// a macro to ease the loading of inline-data
//#define ADD_RECORD(id, meth, name, loc) data.push_back(new Record(id, meth, name, loc));

AccountNumberCheck::AccountNumberCheck() {
  // Disabled COMPILE_RESOURCE because the big list cannot be handled by the compiler anyway.

  string data_path = BANKDATA_PATH;
  string filename = data_path + "/bankdata.txt";
  readFile(filename);

  //#endif
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

	data.push_back(newRecord);
  }
}

void 
AccountNumberCheck::deleteList() 
{
  for (list<Record*>::iterator iter = data.begin(); iter != data.end(); iter++)
    delete (*iter);
}


unsigned int AccountNumberCheck::bankCount() const {
  return data.size();
}

void AccountNumberCheck::createIndex() {
  // not yet implemented
}

const AccountNumberCheck::Record& 
AccountNumberCheck::findBank(const string& bankId) const {  
  list<Record*>::const_iterator iter = data.begin();

  unsigned long lbankId = atol(bankId.c_str());
  while (iter != data.end()) {
	if ((*iter)->bankId == lbankId)
	  return *(*iter);
	iter++;
  }

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
  if ("17" == method) {
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
  if ("28" == method) {
	number2Array("8765432000", weight);
	return algo01(11, weight, false, 8, account);
  }
  if ("30" == method) {
	number2Array("2000012120", weight);
	return algo01(10, weight, false, 10, account);
  }
  if ("31" == method) {
	number2Array("1234567890", weight);
	int result = algo03(11, weight, false, account, 0, 9);

	// no rest after division by modulus (in algo03)?
	// then the the result will be 0!
	if (0 == result)
	  result = 11;

	if (10 == result)
	  return ERROR;

	if (result == account[9])
	  return OK;
	else
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
  if ("36" == method) {
	number2Array("0000058420", weight);
	return algo01(11, weight, false, 10, account);
  }
  if ("37" == method) {
	number2Array("0000058420", weight); weight[4] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("38" == method) {
	number2Array("0009058420", weight);
	weight[4] = 10;
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
  if ("44" == method) {
	number2Array("0000058420", weight); weight[4] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("45" == method) {
	// some numbers can not be checked
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
  if ("52" == method) {
	// FIXME
	return UNKNOWN;
	//	return algo04(bankId, accountId);
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
  if ("57" == method) {	
	// many numbers can not be checked:
	int firstTwo = atoi(array2Number(account).substr(0, 2).c_str());
	int firstSix = atoi(array2Number(account).substr(0, 6).c_str());
	if ((51 > firstTwo) || (91 == firstTwo) || (95 < firstTwo) ||
		(777777 == firstSix) || (888888 == firstSix))
	  return OK;

	number2Array("1212121210", weight);
	return algo01(10, weight, true, 10, account);
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
  if ("63" == method) {
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
  if ("67" == method) {
	number2Array("2121212000", weight);
	return algo01(10, weight, true, 7, account);	
  }
  if ("65" == method) {
	number2Array("2121212000", weight);
	if (9 == account[8]) {
	  weight[8] = 1;
	  weight[9] = 2;
	}
	return algo01(10, weight, true, 8, account);
  }
  if ("67" == method) {
	number2Array("2121212000", weight);
	return algo01(10, weight, true, 8, account);	
  }
  if ("68" == method) {
	// size=10
	if (0 != account[0]) {
	  // and digit 3 is not "9"? error
	  if (9 != account[3])
		return ERROR;

	  number2Array("0001212120", weight);
	  if (OK == algo01(10, weight, true, 10, account))
		return OK;	  
	} else { // size = 10
	  // some account id can not be checked:
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
	  // this way we get 1 as final result
	  tmp = 10;

	// and calc the check number
	tmp = (11 - tmp) % 10;
	if (tmp == account[9])
	  return OK;
	else
	  return ERROR;
  }
  if ("72" == method) {
	number2Array("0001212120", weight);
	return algo01(10, weight, true, 10, account);
  }
  if ("75" == method) {
	int checkIndex;
	if ("000" == array2Number(account).substr(0, 3)) {
	  checkIndex = 9;
	  number2Array("0000212120", weight);
	} else if ("09" == array2Number(account).substr(0, 2)) {
	  checkIndex = 7;
	  number2Array("0021212000", weight);
	} else {
	  checkIndex = 6;
	  number2Array("0212120000", weight);
	}

	return algo01(10, weight, true, checkIndex + 1, account);
  }
  if ("76" == method) {
	number2Array("0765432000", weight);
	int tmp = algo03(11, weight, false, account, 0, 6);

	if (tmp == account[7])
	  return OK;
	else {
	  // shift left, add 00 as subaccount id and try again
	  number2Array(array2Number(account).substr(2) + "00", account);
	  tmp = algo03(11, weight, false, account, 0, 6);
	  if (tmp == account[7])
		return OK;
	}
	return ERROR;
  }
  if ("78" == method) {
	if (0 == account[0] && 0 == account[1])
	  return OK;
	
	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);	
  }
  if ("79" == method) {
	if (0 == account[0])
	  return ERROR;
	int checkIndex = 9;

	if (2 < account[0] && 9 > account[0])
	  number2Array("2121212120", weight);
	else {
	  checkIndex--;
	  number2Array("1212121200", weight);
	}

	return algo01(10, weight, true, checkIndex + 1, account);
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
	number2Array("0987654320", weight); weight[0] = 10;
	if (9 != account[2] && 9 != account[3]) {
	  weight[0] = 0; weight[1] = 0; weight[2] = 0;
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
	int checkIndex = 5;
	number2Array("6543200000", weight);
	if ("0000" == array2Number(account).substr(0, 4)) {
	  checkIndex = 9;
	  number2Array("0000654320", weight);	  
	}

	if (OK == algo01(11, weight, false, checkIndex + 1, account))
	  return OK;
	else
	  return algo01(07, weight, false, checkIndex + 1, account);
  }
  if ("94" == method) {
	number2Array("1212121210", weight);
	return algo01(10, weight, true, 10, account);
  }
  if ("95" == method) {
	// some numbers can not be checked
	string accNumber = array2Number(account);
	if (("0000000001" <= accNumber && accNumber <= "0001999999") ||
		("0090000000" <= accNumber && accNumber <= "0025999999") ||
		("0396000000" <= accNumber && accNumber <= "0499999999") ||
		("0700000000" <= accNumber && accNumber <= "0799999999"))
	  return OK;

	number2Array("4327654320", weight);

	return algo01(11, weight, false, 10, account);
  }
  if ("96" == method) {
	number2Array("1987654320", weight);
	if (ERROR == algo01(11, weight, false, 10, account)) {
	  // check with 00
	  number2Array("2121212120", weight);
	  if (ERROR == algo01(10, weight, true, 10, account)) {
		string acc10 = array2Number(account);
		if (acc10 < "0001300000" || acc10 > "0099399999")
		  return ERROR;
	  }
	}
	return OK;
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

	number2Array("0000058420", weight); weight[4] = 0;
	return algo01(11, weight, false, 10, account);
  }
  if ("A1" == method) {
	number2Array("0001212120", weight);
	return algo01(10, weight, true, 10, account);
  }
  std::cerr << "AccountNumberCheck::check: Specified method '" << method 
	    << "' is unknown." << std::endl;
  return UNKNOWN;
}


