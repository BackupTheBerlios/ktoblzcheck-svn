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
AccountNumberCheck::check(string bankId, string accountId) {
  string method;
  int account[10] = {9,1,3,0,0,0,0,2,0,1};
  int weight[10]  = {0,0,0,0,0,0,0,0,0,0};

  try {
	Record rec = findBank(bankId);
	method = rec.method;
  } catch (int i) {
	// bank was not found
	return BANK_NOT_KNOWN;
  }

  number2Array(accountId, account);

  if ("00" == method) {
	number2Array("2121212121", weight);
	return algo01(10, weight, true, 10, account);	
  }
  if ("01" == method) {
	number2Array("1731731731", weight);
	return algo01(10, weight, false, 10, account);
  }
  if ("02" == method) {
	number2Array("2987654321", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("03" == method) {
	number2Array("2121212121", weight);
	return algo01(10, weight, false, 10, account);	
  }
  if ("04" == method) {
	number2Array("4327654321", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("05" == method) {
	number2Array("1371371371", weight);
	return algo01(10, weight, false, 10, account);
  }
  if ("06" == method) {
	number2Array("4327654321", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("07" == method) {
	number2Array("987654321", weight);
	weight[0] = 10;
	int tmp = algo02(11, weight, false, account) % 10;
	if (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("08" == method) {
	if (atol(array2Number(account).c_str()) < 60000)
	  return OK;
	number2Array("2121212121", weight);
	return algo01(10, weight, true, 10, account);
  }
  if ("09" == method) {
	// no calculation
	return UNKNOWN;
  }
  if ("10" == method) {
	number2Array("987654321", weight);
	weight[0] = 10;
	int tmp = algo02(11, weight, false, account) % 10;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("11" == method) {
	number2Array("987654321", weight);
	weight[0] = 10;
	int tmp = algo02(11, weight, false, account);
	if (10 == tmp)
	  tmp == 9;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("12" == method) {
	number2Array("7317317311", weight);
	return algo01(10, weight, false, 10, account);
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
	number2Array("0007654321", weight);//"0002345671", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if (tmp == account[9])
	  return OK;	
	else 	
	  return ERROR;
  }
  if ("15" == method) {
	number2Array("54321", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("16" == method) {
	number2Array("4327654321", weight);
	int tmp = algo02(11, weight, false, account);
	if (10 == tmp && 0 == account[8])
	  return OK;
	else		
	  if  (tmp == account[9])
		return OK;
	  else 
		return ERROR;
  }
  if ("17" == method) {
	// FIXME
  }
  if ("18" == method) {
	number2Array("3179317931", weight);
	return algo01(10, weight, false, 10, account);
  }
  if ("19" == method) {
	number2Array("1987654321", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("20" == method) {
	number2Array("3987654321", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("28" == method) {
	int weight[10] = {8,7,6,5,4,3,2,0,0,1};
	int tmp = algo02(11, weight, false, account) % 10;
	if  (tmp == account[7])
	  return OK;
	else 
	  return ERROR;
  }
  if ("32" == method) {
	number2Array("0007654321", weight);
	int tmp = algo02(11, weight, false, account);
	if (10 == tmp)
	  tmp == 9;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;
  }
  if ("34" == method) {
	int weight[10] = {7,9,10,5,8,4,2,0,0,1};
	int tmp = algo02(11, weight, false, account) % 10;
	if (tmp == account[7])
	  return OK;
	else
	  return ERROR;
  }
  if ("63" == method) {
	number2Array("0121212000", weight);
	int tmp = (10 - algo03(10, weight, true, account, 1, 6)) % 10;
	if (tmp == account[7])
	  return OK;
	else {
	  // shift left, add 00 as subaccount id and try again
	  number2Array(array2Number(account).substr(2) + "00", account);
	  tmp = (10 - algo03(10, weight, true, account, 1, 6)) % 10;
	  if (tmp == account[7])
		return OK;
	}
	return ERROR;
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
  if ("88" == method) {
	number2Array("0007654321", weight);
	if (9 == account[2])
	  number2Array("0087654321", weight);
	int tmp = algo02(11, weight, false, account) % 10;
	if  (tmp == account[9])
	  return OK;
	else 
	  return ERROR;	
  }
  return UNKNOWN;
}

int algo02(int modulus, int weight[10], bool crossfoot, int accountId[10]) {
  int result = algo03(modulus, weight, crossfoot, accountId, 0, 8);

  // and calc the check number
  result = modulus - result;

  if (result == modulus)
	result = 0;

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
  result = result % modulus;

  return result;
}

AccountNumberCheck::Result
algo01(int modulus, int weight[10], bool crossfoot, 
	   int checkIndex, int accountId[10]) {

  int result = algo02(modulus, weight, crossfoot, accountId);

  // compare the result with the real check number
  if (accountId[checkIndex - 1] == result) {
	return AccountNumberCheck::OK;
  }
  else 
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
