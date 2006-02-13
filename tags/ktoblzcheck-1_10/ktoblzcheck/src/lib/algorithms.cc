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
# include "config.h"
#endif
#include "algorithms.h"
#include <fstream>
#include <iostream>
#include <assert.h>


#if OS_WIN32
# include <windows.h>
# include <shlobj.h>
# undef ERROR
#endif // OS_WIN32

using namespace std;



/* Divisions-Makros (copied from Michael Plugge
   <m.plugge@fh-mannheim.de> and his konto_check package. */

/* Makros zur Modulo-Bildung über iterierte Subtraktionen.
 * auf Intel-Hardware ist dies schneller als eine direkte Modulo-Operation;
 * Auf Alpha ist Modulo allerdings schneller (gute FPU).
 */
#ifdef __ALPHA
#   define SUB_352(a)   (a)%=11
#   define SUB_176(a)   (a)%=11
#   define SUB_88(a)   (a)%=11
#   define SUB_44(a)   (a)%=11
#   define SUB_22(a)   (a)%=11
#   define SUB_11(a)   (a)%=11

#   define SUB_320(a)   (a)%=10
#   define SUB_160(a)   (a)%=10
#   define SUB_80(a)   (a)%=10
#   define SUB_40(a)   (a)%=10
#   define SUB_20(a)   (a)%=10
#   define SUB_10(a)   (a)%=10

#   define SUB_288(a)   (a)%=9
#   define SUB_144(a)   (a)%=9
#   define SUB_72(a)   (a)%=9
#   define SUB_36(a)   (a)%=9
#   define SUB_18(a)   (a)%=9
#   define SUB_9(a)   (a)%=9

#   define SUB_224(a)   (a)%=7
#   define SUB_112(a)   (a)%=7
#   define SUB_56(a)   (a)%=7
#   define SUB_28(a)   (a)%=7
#   define SUB_14(a)   (a)%=7
#   define SUB_7(a)   (a)%=7
#else
#   define SUB(a,x) if((a)>=(x)) (a)-=(x)
#   define SUB_352(a) SUB((a),352); SUB_176(a)
#   define SUB_176(a) SUB((a),176); SUB_88(a)
#   define SUB_88(a)  SUB((a),88);  SUB_44(a)
#   define SUB_44(a)  SUB((a),44);  SUB_22(a)
#   define SUB_22(a)  SUB((a),22);  SUB_11(a)
#   define SUB_11(a)  SUB((a),11)

#   define SUB_320(a) SUB((a),320); SUB_160(a)
#   define SUB_160(a) SUB((a),160); SUB_80(a)
#   define SUB_80(a)  SUB((a),80);  SUB_40(a)
#   define SUB_40(a)  SUB((a),40);  SUB_20(a)
#   define SUB_20(a)  SUB((a),20);  SUB_10(a)
#   define SUB_10(a)  SUB((a),10)

#   define SUB_288(a) SUB((a),288); SUB_144(a)
#   define SUB_144(a) SUB((a),144); SUB_72(a)
#   define SUB_72(a)  SUB((a),72); SUB_36(a)
#   define SUB_36(a)  SUB((a),36); SUB_18(a)
#   define SUB_18(a)  SUB((a),18); SUB_9(a)
#   define SUB_9(a)   SUB((a),9)

#   define SUB_224(a) SUB((a),224); SUB_112(a)
#   define SUB_112(a) SUB((a),112); SUB_56(a)
#   define SUB_56(a)  SUB((a),56);  SUB_28(a)
#   define SUB_28(a)  SUB((a),28);  SUB_14(a)
#   define SUB_14(a)  SUB((a),14);  SUB_7(a)
#   define SUB_7(a)   SUB((a),7)
#endif

// Alternative macros
#ifdef __ALPHA
#else
#define SUBIP(a,x)  (a>=x ? a-x : a)
#define SUBIP_10(a) SUBIP(a,10)
#define SUBIP_20(a) SUBIP_10(SUBIP(a,20))
#define SUBIP_40(a) SUBIP_20(SUBIP(a,40))
#define SUBIP_80(a) SUBIP_40(SUBIP(a,80))
#endif
// However, it is difficult to evaluate the performance improvement
// since especially valgrind doesn't show the extra cost of the modulo
// operation. These macros are therefore not used so far.


// ////////////////////

std::string accnum_getRegKey(const char *value_name) 
{
#if OS_WIN32
   HKEY hkey; 
   TCHAR nbuffer[MAX_PATH];
   BYTE vbuffer[MAX_PATH];
   DWORD nsize;
   DWORD vsize;
   DWORD typ;
   int i;

   snprintf(nbuffer, sizeof(nbuffer), "Software\\Ktoblzcheck\\Paths");

   /* open the key */
   if (RegOpenKey(HKEY_LOCAL_MACHINE, nbuffer, &hkey)){
      // std::cerr << "_getRegKey: Key does not exist in the registry" << std::endl;
      return "";
   }

   for (i=0;; i++) {
      nsize=sizeof(nbuffer);
      vsize=sizeof(vbuffer);
      if (ERROR_SUCCESS!=RegEnumValue(hkey,
				      i,    /* index */
				      nbuffer,
				      &nsize,
				      0,       /* reserved */
				      &typ,
				      vbuffer,
				      &vsize))
	 break;
      if (strcasecmp(nbuffer, value_name)==0 &&
	  typ==REG_SZ) {
	 /* variable found */
	 RegCloseKey(hkey);

	 // WATCH OUT: The string length of the string is one less than
	 // the buffer length! You *have* to think of that or otherwise
	 // the resulting string will (silently) contain a zero byte.
	 std::string result((char*)vbuffer, vsize-1);

	 // std::cerr << "_getRegKey: Got resulting value \"" << result << "\"" << std::endl;
	 return result;
      }
   } /* for */

   RegCloseKey(hkey);

   // std::cerr << "_getRegKey: No value for \"" << value_name
   //     << "\" found in the registry" << std::endl;
   return "";
#else // OS_WIN32
   return "";
#endif // OS_WIN32
}

// ////////////////////

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
	crossFoot(res);

  // add all values
  int result = add(res, startAdd, stopAdd);
  //  cout << "add: " << result << endl;
  result = result % modulus;

  return result;
}

int algo03a(int weight[10], bool crossfoot, int accountId[10], 
		   int startAdd, int stopAdd) {
  int res[10];
  // same as algo03, but return result before modulus
  // mult the weight with the account id
  multArray(accountId, weight, res);

  // for 00,...: calc the crossfoot for each value
  if (crossfoot)
	crossFoot(res);

  // add all values
  int result = add(res, startAdd, stopAdd);
  //  cout << "add: " << result << endl;
  return result;
}

// Algorithm used in methods 24 and B9 variant 1.
int algo05(int modulus1, int modulus2, int weight[10], int accountId[10], 
		   int startAdd, int stopAdd) {
  int res[10];
  // mult the weight with the account id
  multArray(accountId, weight, res);

  // add the weight, individual modulus
  for (int i=startAdd; i<=stopAdd; i++)
    res[i] = (res[i] + weight[i]) % modulus1;

  // sum the stuff, final modulus
    int result = add(res, startAdd, stopAdd) % modulus2;

  return result;
}
 
AccountNumberCheck::Result
algo06(const std::string& accountId) {
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
  }
  // Fixed wrong ending of while-loop; by Erik Kerger <erik@kedo.com>

  if (0 == d2) {
      p = tab1[a5];
  } else {
      p = tab2[a5];
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
algo04(const std::string& bankId, std::string accountId) {

  while (8 < accountId.length() && '0' == accountId[0])
    accountId = accountId.substr(1);

  if (8 != accountId.length())
	return AccountNumberCheck::ERROR;
  if (5 > bankId.length())
	return AccountNumberCheck::ERROR;

  // hmm, here the spec is not clear:
  // as far as i unserstand, the 4. digit from rigth must not be '0'
  // because a '0' would be skipped and thus p would'nt be on index 6 of the
  // eser alt number
  string bankPart = bankId.substr(bankId.length() - 4);

  string accountPart = accountId.substr(2);
  // strip leading '0's
  while (0 < accountPart.length() && '0' == accountPart[0])
    accountPart = accountPart.substr(1); 

  string checkPart = accountId.substr(0, 2);
  // backup the check number
  char checkNumber = checkPart[1];
  // the checknumber must not be used when multiplying -> set to 0 
  string eserUnpadded = bankPart + checkPart[0] + '0' + accountPart;
  string eser12 = string(12 - eserUnpadded.length(), '0') + eserUnpadded;


  // map the 12 digit eser-number to our 10 digit arrays so we can 
  // use the std multiplication
  int eser02[10];
  int eser10[10];
  number2Array(eser12.substr(0, 2), eser02);
  number2Array(eser12.substr(2), eser10);
  int weight02[10];
  int weight10[10];
  number2Array("0000000042", weight02);
  number2Array("1637905842", weight10);
  weight10[5] = 10;

  // perform the multiplication
  int res02[10];
  int res10[10];
  multArray(eser02, weight02, res02);
  multArray(eser10, weight10, res10);

  // add-mod
  int result = add_10(res02) + add_10(res10);
  result = result % 11;

  // find a multiple of weight[checkNumberIndex] that fits
  // 10 == (result + multiple) % 11
  int i=0;
  int pWeight = weight10[9 - accountPart.length()];
  while (i < 11) {
    if (10 == (((i*pWeight) + result) % 11))
      break;
    i++;
  }

  // multiple found, does it match?
  // if none was found (11 == i), the next condition will evaluate to false
  if (checkNumber - '0' == i)
    return AccountNumberCheck::OK;

  return AccountNumberCheck::ERROR;
}

// Added by Jens Gecius
// Some corrections by Erik Kerger
// same as algo04, but for nine digit accountIDs with
// a little different eser12 system.
AccountNumberCheck::Result 
algo04a(const std::string& bankId, std::string accountId) {

  while (9 < accountId.length() && '0' == accountId[0])
    accountId = accountId.substr(1);

  if (9 != accountId.length())
	return AccountNumberCheck::ERROR;
  if (5 > bankId.length())
	return AccountNumberCheck::ERROR;

  // the middle 2 digits of the bankid
  string bankPart = bankId.substr(bankId.length() - 4, 2);
  //while (bankPart.length() > 0 && '0' == bankPart[0])
            //bankPart = bankPart.substr(1);
  string bankPart2 = bankId.substr(bankId.length() - 1);
  // cout << "bankpart: " << bankPart << endl;
  // the second digit of the accountid
  string checkPart = accountId.substr(2, 1);
  // cout << "checkPart: " << checkPart << endl;
  string accountPart1 = accountId.substr(0, 1);
  string accountPart2 = accountId.substr(1, 1);
  string accountPart = accountId.substr(3);
  // skip leading "0" in the the accountPart
  while (accountPart.length() > 0 && '0' == accountPart[0])
  	accountPart = accountPart.substr(1);
  // cout << "accoutPart: " << accountPart << endl;
  string eser12unpadded = bankPart + accountPart2 + bankPart2
    + accountPart1 + checkPart + accountPart;
  // cout << "eser12unpadded: " << eser12unpadded << endl;
  // concat and padd to 12 byte
  string eser12 = eser12unpadded;
  while (eser12.length() < 12)
	eser12 = "0" + eser12;

  // slit so we can use the int[10]-methods we already have
  // xx xxxxxxxxxx (eser02 and eser10)
  //  string eser10 = eser12.substr(2);
  //  string eser02 = eser12.substr(0, 2);
  int eser10[10];
  int eser02[10];
  number2Array(eser12.substr(2), eser10);
  number2Array(eser12.substr(0, 2), eser02);
  // cout << "eser02: " << array2Number(eser02) << endl;
  // cout << "eser10: " << array2Number(eser10) << endl;

  int weight02[10];
  int weight10[10];
  number2Array("0000000042", weight02);
  number2Array("1637905842", weight10);
  // weight10[5] must be 10. 
  weight10[5] = 10;

  int res02[10];
  int res10[10];
  // mult the weight
  multArray(eser02, weight02, res02);
  multArray(eser10, weight10, res10);

  int result = add_10(res02);
  // cout << "result add1: " << result << endl;
  // cout << "result add2: " << add(res10, 0, 9) << endl;
  result += add_10(res10);
  // cout << "result: " << result << endl;
  result = result % 11;
  // cout << "result: " << result << endl;
  // cout << "weight02: " << array2Number(weight02) << endl;
  // cout << "weight10: " << array2Number(weight10) << endl;
  // the weight for the check-digit
  // int realWeight = weight10[9 - accountPart.length()];
  // cout << "realWeight: " << realWeight << endl;
  // the "Pruefziffernberechnung" doku shows a way to calculate the
  // check num. As we have to check only, so only have to check the
  // result we calculated including the check digit is 10.
  if (result == 10)
	return AccountNumberCheck::OK;

  return AccountNumberCheck::ERROR;
}

// iterated transformation
AccountNumberCheck::Result 
algo07(int accountId[10], int transform[6][10]) {
  int result = 0;
  for (int i=0; i<10; i++) {
        result = result + transform[transform[5][i]][accountId[i]];
  }
  result = 10 - (result % 10);
  if (result == 10)
        result = 0;
  if (result == accountId[9])
        return AccountNumberCheck::OK;
  return AccountNumberCheck::ERROR;
}

void multArray(const int *a, const int *b, int dest[10]) {
#define MULTARRAY(i) dest[i] = a[i] * b[i]
    MULTARRAY(0);
    MULTARRAY(1);
    MULTARRAY(2);
    MULTARRAY(3);
    MULTARRAY(4);
    MULTARRAY(5);
    MULTARRAY(6);
    MULTARRAY(7);
    MULTARRAY(8);
    MULTARRAY(9);
    // Sigh -- writing it as above greatly speeds up runtime compared
    // to a conventional for-loop:
//     for (int i=0; i<10; i++) {
// 	dest[i] = a[i] * b[i];
//     }
}

// This one is unused
void crossFoot(const int *source, int dest[10], int start, int stop) {
    for (int i=start; i<stop+1; i++) {
	int elem = source[i];
	dest[i] = elem / 10  +  elem % 10;
  }
}


void crossFoot(int *source) {
#define CROSSFOOTOP(i)  source[i] = source[i] / 10 + source[i] % 10
    CROSSFOOTOP(0);
    CROSSFOOTOP(1);
    CROSSFOOTOP(2);
    CROSSFOOTOP(3);
    CROSSFOOTOP(4);
    CROSSFOOTOP(5);
    CROSSFOOTOP(6);
    CROSSFOOTOP(7);
    CROSSFOOTOP(8);
    CROSSFOOTOP(9);
}

int add(const int *source, int start, int stop) {
    const int *sp = source + start;
    switch (stop-start) {
    case 0:
	return sp[0];
    case 1:
	return sp[0] + sp[1];
    case 2:
	return sp[0] + sp[1] + sp[2];
    case 3:
	return sp[0] + sp[1] + sp[2] + sp[3];
    case 4:
	return sp[0] + sp[1] + sp[2] + sp[3] + sp[4];
    case 5:
	return sp[0] + sp[1] + sp[2] + sp[3] + sp[4] 
	    + sp[5];
    case 6:
	return sp[0] + sp[1] + sp[2] + sp[3] + sp[4] 
	    + sp[5] + sp[6];
    case 7:
	return sp[0] + sp[1] + sp[2] + sp[3] + sp[4] 
	    + sp[5] + sp[6] + sp[7];
    case 8:
	return sp[0] + sp[1] + sp[2] + sp[3] + sp[4] 
	    + sp[5] + sp[6] + sp[7] + sp[8];
    case 9:
	return sp[0] + sp[1] + sp[2] + sp[3] + sp[4] 
	    + sp[5] + sp[6] + sp[7] + sp[8] + sp[9];
    default:
	assert(0);
    }
}

string array2Number(int a[10]) {
    string result("0000000000");
    for (unsigned i=0; i<10; i++) {
	result[i] = '0' + a[i];
    }

    return result;
}

void number2Array(const string& number, int a[10]) {
    if (number.size() == 10) {
#define ASSIGNARRAY(i)  a[i] = number[i] - '0'
	ASSIGNARRAY(0);
	ASSIGNARRAY(1);
	ASSIGNARRAY(2);
	ASSIGNARRAY(3);
	ASSIGNARRAY(4);
	ASSIGNARRAY(5);
	ASSIGNARRAY(6);
	ASSIGNARRAY(7);
	ASSIGNARRAY(8);
	ASSIGNARRAY(9);
    } else {
	unsigned padd_size = 10 - number.size();
	unsigned i = 0;
	for ( ; i < padd_size; i++)
	    a[i] = 0;
	for ( ; i < 10; i++) 
	    a[i] = number[i-padd_size] - '0';
    }
}

long_long number2LongLong(const string& number) {
  long_long result = 0;

  for (unsigned int i=0; i<number.length(); i++) {
	result *= 10;
	result += number[i] - '0';
  }

  return result;
}


