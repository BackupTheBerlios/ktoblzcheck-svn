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
#include "algorithms.h"
#include <fstream>
#include <iostream>

using namespace std;

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
algo05(string accountId) {
  int account[10];
  int weight[10];
  
  number2Array(accountId, account);

	if (2 < account[0] && 7 > account[0])
	  account[0] = 0;
	if (9 == account[0]) {
	  account[0] = 0;
	  account[1] = 0;
	  account[2] = 0;
	  if (0 == account[3])
		return AccountNumberCheck::ERROR;
	}
	
	// check how many leading "0", shift weight to right
	string weightString = "";
	for (int i=0; i<10; i++) {
	  if (0 == account[i])
		weightString += "0";
	  else
		break;
	}
	     	
	weightString += "123123123";
	weightString = weightString.substr(0, 9) + "0";
	number2Array(weightString, weight);

	// now mult the account with the weight
	int res[10];	
	multArray(account, weight, res);

	// add the weight
	for (int i=0; i<10; i++)
	  res[i] = (res[i] + weight[i]) % 11;

	// sum the stuff
	int result = add(res, 0, 9) % 10;
	
	if (result == account[9])
	  return AccountNumberCheck::OK;
	else
	  return AccountNumberCheck::ERROR;
}
 
AccountNumberCheck::Result
algo06(string accountId) {
  int i = 0;
  int c2 = 0;
  int d2 = 0;
  int a5 = 0;
  int p = 0;
  int account[10];

  int tab1[5] = {0, 4, 3, 2, 6};
  int tab2[5] = {7, 1, 5, 9, 8};

  number2Array(accountId, account);

  i = 4;
  while (0 == account[i-1])
	i++;

  c2 = i % 2;
  d2 = 0;
  a5 = 0;
  
  while (i < 10) {
	switch (account[i-1]) {
	case 0:
	  account[i-1] = 5;
	  break;
	case 1:
	  account[i-1] = 6;
	  break;
	case 5:
	  account[i-1] = 10;
	  break;
	case 6:
	  account[i-1] = 1;
	  break;
	}

	if (c2 == d2) {	  
	  if (account[i-1] > 5) {
		if (0 == c2 && 0 == d2) {
		  c2 = 1;
		  d2 = 1;
		  a5 += 6 - (account[i-1] - 6);
		} else { // 0 == c2 == d2
		  c2 = 0;
		  d2 = 0;
		  a5 += account[i-1];
		} // 0 == c2 == d2
	  } else { // account(i-1) > 5
		if (0 == c2 && 0 == d2) {
		  c2 = 1;
		  a5 += account[i-1];
		} else { // 0 == c2 == d2
		  c2 = 0;
		  a5 += account[i-1];
		} // 0 == c2 == d2
	  } // account(i-1) > 5
	} else { // c2 == d2
	  if (account[i-1] > 5) {
		if (0 == c2) {
		  c2 = 1;
		  d2 = 0;
		  a5 += - 6 + (account[i-1] - 6);
		} else { // 0 == c2
		  c2 = 0;
		  d2 = 1;
		  a5 -= account[i-1];
		} // 0 == c2
	  } else { // account(i-1) > 5
		if (0 == c2) {
		  c2 = 1;
		  a5 -= account[i-1];
		} else { // 0 == c2
		  c2 = 0;
		  a5 -= account[i-1];
		} // 0 == c2
	  } // account(i-1) > 5
	} // c2 == d2
	i++;
  }
  
  while( a5 < 0 || a5 > 4) {
	if (a5 > 4) {
	  a5 -= 5;
	} else {
	  a5 += 5;
	}
	if (0 == d2) {
	  p = tab1[a5];
	} else {
	  p = tab2[a5];
	}
  }

  if (p == account[9]) {
	return AccountNumberCheck::OK;
  } else {
	if (0 == account[3]) {
	  if (p > 4) {
		p -= 5;
	  } else {
		p += 5;
	  }
	  if (p == account[9])
		return AccountNumberCheck::OK;
	}	  
  }
   
  // hugh...
  // such a stupid algorithm...;-(
  return AccountNumberCheck::ERROR;
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

long long number2LongLong(string number) {
  long long result = 0;

  for (unsigned int i=0; i<number.length(); i++) {
	result *= 10;
	result += number[i] - 48;
  }

  return result;
}
