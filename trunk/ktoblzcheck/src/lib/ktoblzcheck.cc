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
#include "ktoblzcheck.hh"
#include <fstream>

list<AccountNumberCheck::Record*> AccountNumberCheck::data;

// forward declarations
void   multArray(int a[10], int b[10], int dest[10]);
void   crossFoot(int source[10], int dest[10], int start, int stop);
int    add(int source[10], int start, int stop);
string array2Number(int a[10]);
void   number2Array(string number, int a[10]);
int    algo02(int modulus, int weight[10], bool crossfoot, int accountId[10]);
int    algo03(int modulus, int weight[10], bool crossfoot, int accountId[10], 
			  int startAdd, int stopAdd);
AccountNumberCheck::Result 
       algo04(string bankId, string accountId);
AccountNumberCheck::Result 
       algo01(int modulus, int weight[10], bool crossfoot, 
			  int checkIndex, int accountId[10]);


// a macro to ease the loading of inline-data
#define ADD_RECORD(id, meth, name, loc) {\
  newRecord = new Record; \
  newRecord->bankId = id;\
  newRecord->method = meth;\
  newRecord->bankName = name;\
  newRecord->location = loc;\
  data.push_back(newRecord);\
}

#ifdef COMPILE_RESOURCE
void AccountNumberCheck::init() {
  Record *newRecord = NULL;
#include "data.cc"
}
#else /* COMPILE_RESOURCE */
void AccountNumberCheck::init(string filename) {
  ifstream file(filename.c_str());
  char *buffer = new char[200];
  string line;
  while (file) {
	file.getline(buffer, 200);
	line = (const char*) buffer;
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
#endif /* COMPILE_RESOURCE */

void AccountNumberCheck::flush() {
  for (list<Record*>::iterator iter = data.begin(); iter != data.end(); iter++)
	delete (*iter);
}

int AccountNumberCheck::count() {
  return data.size();
}

void AccountNumberCheck::createIndex() {
  // not yet implemented
}

const AccountNumberCheck::Record 
AccountNumberCheck::findBank(string bankId) {  
  list<Record*>::iterator iter = data.begin();

  unsigned long lbankId = atol(bankId.c_str());
  while (iter != data.end()) {
	if ((*iter)->bankId == lbankId)
	  return *(*iter);
	iter++;
  }

  throw -1;
}

const AccountNumberCheck::Result 
AccountNumberCheck::check(string bankId, string accountId, string method="") {
  int account[10] = {9,1,3,0,0,0,0,2,0,1};
  int weight[10]  = {0,0,0,0,0,0,0,0,0,0};

  try {
	Record rec = findBank(bankId);
	method = rec.method;
  } catch (int i) {
	// bank was not found, return error if not forced to use a spec. method
	if ("" == method)
	  return BANK_NOT_KNOWN;
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
	number2Array("987654320", weight);
	weight[0] = 10;
	return algo01(11, weight, false, 10, account);
  }
  if ("11" == method) {
	number2Array("987654320", weight);
	weight[0] = 10;
	int tmp = algo03(11, weight, false, account, 0, 9);
	tmp = (11 - tmp) % 10;
	if (10 == tmp)
	  tmp == 9;
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
  if ("28" == method) {
	number2Array("8765432000", weight);
	return algo01(11, weight, false, 8, account);
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
  if ("81" == method) {
	number2Array("0987654320", weight); weight[0] = 10;
	if (9 != account[2] && 9 != account[3]) {
	  weight[0] = 0; weight[1] = 0; weight[2] = 0;
	}
	return algo01(11, weight, false, 10, account);
  }
  if ("88" == method) {
	number2Array("0007654320", weight);
	if (9 == account[2])
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
  if ("99" == method) {
	if ("0396000000" <= array2Number(account) && 
		array2Number(account) <= "0499999999")
	  return OK;
	number2Array("4327654320", weight);
	return algo01(11, weight, false, 10, account);
  }
  return UNKNOWN;
}


AccountNumberCheck::Result
algo01(int modulus, int weight[10], bool crossfoot, 
	   int checkIndex, int accountId[10]) {

  int result = algo02(modulus, weight, crossfoot, accountId);

  // compare the result with the real check number
  if (accountId[checkIndex - 1] == result)
	return AccountNumberCheck::OK;
  else 
	return AccountNumberCheck::ERROR;
}

// will be removed!!!
int algo02(int modulus, int weight[10], bool crossfoot, int accountId[10]) {
  int result = algo03(modulus, weight, crossfoot, accountId, 0, 9);

  //  cout << "res%mod: " << result << endl;
  // no rest after division by modulus (in algo03)?
  // then the the result will be 0!
  if (0 == result)
	result = modulus;

  // and calc the check number
  result = (modulus - result) % 10;

  return result;
}

int algo03(int modulus, int weight[10], bool crossfoot, int accountId[10], 
		   int startAdd, int stopAdd) {
  int res[10];
  // mult the weight with the account id
  multArray(accountId, weight, res);

  // for 00,...: calc the crossfoot for each value
  if (crossfoot)
	crossFoot(res, res, 0, 9);

  // add all values
  int result = add(res, startAdd, stopAdd);
  //  cout << "add: " << result << endl;
  result = result % modulus;

  return result;
}

AccountNumberCheck::Result 
algo04(string bankId, string accountId) {

  if (8 != accountId.length())
	return AccountNumberCheck::ERROR;
  if (5 > bankId.length())
	return AccountNumberCheck::ERROR;

  // the last 4 digits of the bankid (without leading "0")
  string bankPart = bankId.substr(bankId.length() - 4);
  while ('0' == bankPart[0])
	bankPart = bankPart.substr(1);
  cout << "bankpart: " << bankPart << endl;
  // the first 2 digits of the accountid
  string checkPart = accountId.substr(0, 2);
  cout << "checkPart: " << checkPart << endl;
  string accountPart = accountId.substr(2);
  // skip leading "0" in the the accountPart
  while (accountPart.length() > 0 && '0' == accountPart[0])
  	accountPart = accountPart.substr(1);
  cout << "accoutPart: " << accountPart << endl;
  string eser12unpadded = bankPart + checkPart + accountPart;
  cout << "eser12unpadded: " << eser12unpadded << endl;
  // concat and padd to 12 byte
  string eser12 = eser12unpadded;
  while (eser12.length() < 12)
	eser12 = "0" + eser12;
  cout << "eser12: " << eser12 << endl;

  // special handling for eser12 that start with "009"
  if (eser12.substr(0, 3) == "009") {
	// run method 2
	int weight[10];
	int account[10];
	number2Array(accountId, account);
	number2Array("2987654321", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if (tmp == account[9])
	  return AccountNumberCheck::OK;
	else 
	  return AccountNumberCheck::ERROR;
  }

  // spilit so we can use the int[10]-methods we already have
  // xx xxxxxxxxxx (eser02 and eser10)
  //  string eser10 = eser12.substr(2);
  //  string eser02 = eser12.substr(0, 2);
  int eser10[10];
  int eser02[10];
  number2Array(eser12.substr(2), eser10);
  number2Array(eser12.substr(0, 2), eser02);
  cout << "eser02: " << array2Number(eser02) << endl;
  cout << "eser10: " << array2Number(eser10) << endl;

  int weight02[10];
  int weight10[10];
  number2Array("0000000042", weight02);
  number2Array("1637905842", weight10);
  // weight10[5] should be 10. but for the multArray, we need =0 because
  // this is the position of the ceck-digit (which must not be included)
  // weight10[5] = 10;

  int res02[10];
  int res10[10];
  // mult the weight
  multArray(eser02, weight02, res02);
  multArray(eser10, weight10, res10);

  int result = (add(res02, 0, 9));
  cout << "result add1: " << result << endl;
  cout << "result add2: " << add(res10, 0, 9) << endl;
  result += (add(res10, 0, 9));
  cout << "result: " << result << endl;
  result = result % 11;
  cout << "result: " << result << endl;
  // now after calculating, we can set the weight in weight10 correct
  // we may need it
  weight10[5] = 10;
  cout << "weight02: " << array2Number(weight02) << endl;
  cout << "weight10: " << array2Number(weight10) << endl;
  // the weight for the check-digit
  int realWeight = weight10[9 - accountPart.length()];
  cout << "realWeight: " << realWeight << endl;

  // find a multiple of realWeight so that
  // (result + i*realWeight) % 11 = 10
  int i = 0;
  while (i < 12) {
	int tmp = (result + (i * realWeight));
	cout << "tmp: " << tmp << endl;
	tmp = tmp % 11;
	if (10 == tmp) 
	  break;
	  i++;
  }
  cout << "i: " << i << endl;
  // none found?
  if (12 == i)
	return AccountNumberCheck::ERROR;

  // if asc(i)=checkdigit -> ok
  if (i+48 == checkPart[1])
	return AccountNumberCheck::OK;

  return AccountNumberCheck::ERROR;
}

void multArray(int a[10], int b[10], int dest[10]) {
  for (int i=0; i<10; i++) {
	dest[i] = a[i] * b[i];
  }
}

void crossFoot(int source[10], int dest[10], int start, int stop) {
  for (int i=start; i<stop+1; i++) {
	int t = source[i] / 10;
	int o = source[i] % 10;

	dest[i] = t + o;
  }
}

int add(int source[10], int start, int stop) {
  int result = 0;
  for (int i=start; i<stop+1; i++) {
	result += source[i];
  }

  return result;
}

string array2Number(int a[10]) {
  string result;
  char c;

  for (int i=0; i<10; i++) {
	c = 48 + a[i];
	result += c;
  }

  return result;
}

void number2Array(string number, int a[10]) {
  while (number.length() < 10)
	number = "0" + number;
	
  for (int i=9; i>-1; i--) {
	a[i] = number[i] - 48;
  }
}
