/***************************************************************************
                             -------------------
    cvs         : methods.cc
    begin       : Sun Jan 16 2005
    copyright   : (C) 2002, 2003, 2005 by Fabian Kaiser and Christian Stimming
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


// using namespace std;  // -- not needed


// Each method is one function. The mapping of method identification
// to function is done by the cb_funcs[] array down at the end of this
// file. At runtime, the cb_funcs[] array will be inserted into a hash
// table, so that the lookup of each method's function will be
// *really* fast. 
//
// When adding a new method, do this: 
//
// 1. Write a method_XY() function similar to the ones existing here.
//
// 2. Add one additional line to the cb_funcs[] array down at the end
// of this file.
//
// 3. That's it!


AccountNumberCheck::Result method_00(int *account, int *weight) {
    number2Array("2121212120", weight);
    return algo01(10, weight, true, 10, account);	
}
AccountNumberCheck::Result method_01(int *account, int *weight) {
    number2Array("1731731730", weight);
    return algo01(10, weight, false, 10, account);
}
AccountNumberCheck::Result method_02(int *account, int *weight) {
    number2Array("2987654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_03(int *account, int *weight) {
    number2Array("2121212120", weight);
    return algo01(10, weight, false, 10, account);	
}
AccountNumberCheck::Result method_04(int *account, int *weight) {
    number2Array("4327654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_05(int *account, int *weight) {
    number2Array("1371371370", weight);
    return algo01(10, weight, false, 10, account);
}
AccountNumberCheck::Result method_06(int *account, int *weight) {
    number2Array("4327654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_07(int *account, int *weight) {
    number2Array("987654320", weight);
    weight[0] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_08(int *account, int *weight) {
    if (atol(array2Number(account).c_str()) < 60000)
	return AccountNumberCheck::OK;
    number2Array("2121212120", weight);
    return algo01(10, weight, true, 10, account);
}
AccountNumberCheck::Result method_09(int *account, int *weight) {
    // no calculation -> no error
    return AccountNumberCheck::OK;
}
AccountNumberCheck::Result method_10(int *account, int *weight) {
    number2Array("987654320", weight); weight[0] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_11(int *account, int *weight) {
    number2Array("987654320", weight);
    weight[0] = 10;
    int tmp = algo03(11, weight, false, account, 0, 9);

    // Code by Erik Kerger:
    if (tmp > 0)    //if result == 0 then check is 0 too
	tmp = 11 - tmp;
    if (10 == tmp)  //use 9 instead of 0
	tmp = 9;
    else
	tmp = tmp % 10; 
    //tmp = (11 - tmp) % 10;
    //if (10 == tmp)
    // tmp = 9;

    if  (tmp == account[9])
	return AccountNumberCheck::OK;
    else 
	return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_12(int *account, int *weight) {
    // not used
    return AccountNumberCheck::UNKNOWN;
}
AccountNumberCheck::Result method_13(int *account, int *weight) {
    number2Array("121212000", weight);
    if (algo01(10, weight, true, 8, account) != AccountNumberCheck::OK) {
	// shift left, add 00 as subaccount id and try again
	number2Array(array2Number(account).substr(2) + "00", account);
	if (AccountNumberCheck::OK != algo01(10, weight, true, 8, account))
	    return AccountNumberCheck::ERROR;
    } 
    return AccountNumberCheck::OK;
}
AccountNumberCheck::Result method_14(int *account, int *weight) {
    number2Array("0007654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_15(int *account, int *weight) {
    number2Array("54320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_16(int *account, int *weight) {
    number2Array("4327654320", weight);
    int tmp = algo03(11, weight, false, account, 0, 9);
    tmp = tmp % 11;
    if (1 == tmp && account[8] == account[9])
	return AccountNumberCheck::OK;
    else
	return algo01(11, weight, false, 10, account);
}
// Added by Jens Gecius, validated with one test accountID
AccountNumberCheck::Result method_17(int *account, int *weight) {
    number2Array("0121212000", weight);
    int tmp = algo03a(weight, true, account, 1, 6);
    tmp = (tmp - 1) % 11;
    tmp = 10 - tmp;
    if (10 == tmp)
	tmp = 0;
    if ( account[7] == tmp )
	return AccountNumberCheck::OK;
    else
	return AccountNumberCheck::ERROR;
    // FIXME
}
AccountNumberCheck::Result method_18(int *account, int *weight) {
    number2Array("3179317930", weight);
    return algo01(10, weight, false, 10, account);
}
AccountNumberCheck::Result method_19(int *account, int *weight) {
    number2Array("1987654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_20(int *account, int *weight) {
    number2Array("3987654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_21(int *account, int *weight) {
    int res[10];
    number2Array("2121212120", weight);

    // mult the weight with the account id
    multArray(account, weight, res);

    // for 00,...: calc the crossfoot for each value
    crossFoot(res);

    // add all values
    int result = add_10(res);

    // crossfoot for the result until only one digit is left
    while (result > 9) {
	int tmp = result % 10;
	result /= 10;
	result += tmp;
    }	
    result = 10 - result;

    if (result == account[9])
	return AccountNumberCheck::OK;
    else
	return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_22(int *account, int *weight) {
    number2Array("3131313130", weight);
    return algo01(10, weight, false, 10, account);
}
AccountNumberCheck::Result method_23(int *account, int *weight) {
    number2Array("7654320", weight);
    int tmp = algo03(11, weight, false, account, 0, 9);
    tmp = tmp % 11;
    if (0 == tmp && 0 == account[6])
	return AccountNumberCheck::OK;
    if (1 == tmp && account[5] == account[6])
	return AccountNumberCheck::OK;
    if (1 < tmp && account[6] == (11 - tmp))
	return AccountNumberCheck::OK;
	
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_24(int *account, int *weight, 
				     const std::string& accountId, const std::string& bankId) {
    return algo05(accountId); 
}
// Added by Jens Gecius, validated with one test accountID
AccountNumberCheck::Result method_25(int *account, int *weight) {
    number2Array("987654320", weight);
    int result = algo03(11, weight, false, account, 1, 8);
    result = 11 - result;
    if (result == 11)
	result = 0;
    if (result == 10 && account[9] == 0 && (account[1] == 8 || account[1] == 9))
	return AccountNumberCheck::OK;
    if (result == account[9])
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
// Added by Jens Gecius, validated with three test accountIDs and one own accountID
AccountNumberCheck::Result method_26(int *account, int *weight) {
    if (account[0] == 0 && account[1] == 0)
	number2Array(array2Number(account).substr(2) + "00", account);
    number2Array("2765432000", weight);
    return algo01(11, weight, false, 8, account);
}
// Added by Jens Gecius, validated with nine own accountIDs + testaccountID (six > 1000000000)
AccountNumberCheck::Result method_27(int *account, int *weight) {
    int transform[6][10] = 
	{ {0,0,0,0,0,0,0,0,0,0}, // zero for non-transform [0][x]
	  {0,1,5,9,3,7,4,8,2,6}, // first transform line   [1][x]
	  {0,1,7,6,9,8,3,2,5,4}, // second transform line  [2][x]
	  {0,1,8,4,6,2,9,5,7,3}, // third transform line   [3][x]
	  {0,1,2,3,4,5,6,7,8,9}, // fourth transform line  [4][x]
	  {1,4,3,2,1,4,3,2,1,0} }; // which transform line

    if (atol(array2Number(account).c_str()) < 1000000000) {
	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);
    } else {
        // iterated transformation
	return algo07(account, transform);
    }
    //return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_28(int *account, int *weight) {
    number2Array("8765432000", weight);
    return algo01(11, weight, false, 8, account);
}
// Added by Jens Gecius, validated with test accountID
AccountNumberCheck::Result method_29(int *account, int *weight) {
    int transform[6][10] = 
	{ {0,0,0,0,0,0,0,0,0,0}, // zero for non-transform [0][x]
	  {0,1,5,9,3,7,4,8,2,6}, // first transform line   [1][x]
	  {0,1,7,6,9,8,3,2,5,4}, // second transform line  [2][x]
	  {0,1,8,4,6,2,9,5,7,3}, // third transform line   [3][x]
	  {0,1,2,3,4,5,6,7,8,9}, // fourth transform line  [4][x]
	  {1,4,3,2,1,4,3,2,1,0} }; // which transform line

    return algo07(account, transform);
}
AccountNumberCheck::Result method_30(int *account, int *weight) {
    number2Array("2000012120", weight);
    return algo01(10, weight, false, 10, account);
}
// Changed by Jens Gecius, validated with two test accountIDs
AccountNumberCheck::Result method_31(int *account, int *weight) {
    number2Array("1234567890", weight);
    int result = algo03(11, weight, false, account, 0, 9);

    // no rest after division by modulus (in algo03)?
    // then the the result will be 0! (as it is... removed next two lines
    // as there is no further calculation
    // if (0 == result)
    //  result = 11;

    // next two lines removed, as 10 can never be a digit in accountID
    // if (10 == result)
    //  return AccountNumberCheck::ERROR;

    if (result == account[9])
	return AccountNumberCheck::OK;
    // to stay in line with convention so far, remove next line
    // else
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_32(int *account, int *weight) {
    number2Array("0007654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_33(int *account, int *weight) {
    number2Array("0000654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_34(int *account, int *weight) {
    number2Array("7905842000", weight); weight[2] = 10;
    return algo01(11, weight, false, 8, account);
}
// Added by Jens Gecius, not validated due to missing bank using this algo
AccountNumberCheck::Result method_35(int *account, int *weight) {
    number2Array("0987654320",weight); weight[0] = 10;
    int result = algo03(11, weight, false, account, 0, 9);
    if (result == 10 && account[9] == account[8])
	return AccountNumberCheck::OK;
    if (result == account[9])
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_36(int *account, int *weight) {
    number2Array("0000058420", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_37(int *account, int *weight) {
    number2Array("0000058420", weight); weight[4] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_38(int *account, int *weight) {
    number2Array("0009058420", weight); weight[4] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_39(int *account, int *weight) {
    number2Array("0079058420", weight); weight[4] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_40(int *account, int *weight) {
    number2Array("6379058420", weight); weight[4] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_41(int *account, int *weight) {
    number2Array("2121212120", weight);
    if (9 == account[3]) {
	weight[0] = 0; 
	weight[1] = 0;
	weight[2] = 0;
    }
    return algo01(10, weight, true, 10, account);	
}
AccountNumberCheck::Result method_42(int *account, int *weight) {
    number2Array("0987654320", weight);
    return algo01(11, weight, false, 10, account);
}
// Added by Jens Gecius, validated with two test accountIDs
AccountNumberCheck::Result method_43(int *account, int *weight) {
    number2Array("9876543210", weight);
    return algo01(10, weight, false, 10, account);
}
AccountNumberCheck::Result method_44(int *account, int *weight) {
    number2Array("0000058420", weight); weight[4] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_45(int *account, int *weight) {
    // some numbers do not have a checksum
    if (0 == account[0] || 1 == account[4])
	return AccountNumberCheck::OK;

    number2Array("2121212120", weight);
    return algo01(10, weight, true, 10, account);	
}
AccountNumberCheck::Result method_46(int *account, int *weight) {
    number2Array("0065432000", weight);
    return algo01(11, weight, false, 8, account);
}
AccountNumberCheck::Result method_47(int *account, int *weight) {
    number2Array("0006543200", weight);
    return algo01(11, weight, false, 9, account);
}
AccountNumberCheck::Result method_48(int *account, int *weight) {
    number2Array("0076543200", weight);
    return algo01(11, weight, false, 9, account);
}
AccountNumberCheck::Result method_49(int *account, int *weight) {
    // try with method == 00
    number2Array("2121212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    else {
	// on error try with method == 01
	number2Array("1731731730", weight);
	return algo01(10, weight, false, 10, account);
    }
}
AccountNumberCheck::Result method_50(int *account, int *weight) {
    number2Array("7654320000", weight);
    AccountNumberCheck::Result tmp = algo01(11, weight, false, 7, account);
    if (AccountNumberCheck::OK == tmp)
	return AccountNumberCheck::OK;
    else {
	// on error, shift left by 3 and try again
	number2Array(array2Number(account).substr(3) + "000", account);
	return algo01(11, weight, false, 7, account);
    }
}
// Added by Jens Gecius, validated with six test accountIDs
// Checked with positive numbers for methods A, B, C
// Exception handling see below
AccountNumberCheck::Result method_51(int *account, int *weight) {
    number2Array("0007654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;	// Method A
    number2Array("0000654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;  // Method B
    if (7 == account[9] || 8 == account[9] || 9 == account[9])
	return AccountNumberCheck::ERROR;  // Invalid IDs
    // Modified by Jens Gecius, algo changed July 9th, 2004
    // Exception handling
    // This calculation, as far as I can see, is also influenced
    // by the calculation "error" in algo02 (see below, method "B2"
    // but I have no solution on hand.
    //        if (9 == account[2] && 9 == account[3]) {
    if (9 == account[2]) {
	number2Array("0087654320", weight);
	if (algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK; // Variant 1
	number2Array("0987654320", weight); weight[0] = 10;
	//            	number2Array("987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account); // Variant 2
    }		   // Exception to method C
    return algo01(7, weight, false, 10, account); // Method C
}
AccountNumberCheck::Result method_52(int *account, int *weight, 
				     const std::string& accountId, const std::string& bankId) {
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
AccountNumberCheck::Result method_53(int *account, int *weight,
				     const std::string& accountId, const std::string& bankId) {
    // Changed by Jens Gecius, only 10 digit accountIDs starting with "9",
    // *NOT* validated
    if (10 == accountId.length() && 9 == account[0]) {
	number2Array("3987654320", weight);
	return algo01(11, weight, false, 10, account);
    }
    // Hint by Erik Kerger <erik@kedo.com>, 2004-08-14: return
    // algo04a instead of algo04
    return algo04a(bankId, accountId);
}
AccountNumberCheck::Result method_54(int *account, int *weight) {
    // ids must start with 49
    if (! (4 == account[0] && 9 == account[1]))
	return AccountNumberCheck::ERROR;

    number2Array("0027654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_55(int *account, int *weight) {
    number2Array("8787654320", weight);
    return algo01(11, weight, false, 10, account);
}
// Added by Jens Gecius, validated with two test accountIDs
AccountNumberCheck::Result method_56(int *account, int *weight) {
    number2Array("4327654320", weight);
    int result = 11 - algo03(11, weight, false, account, 0, 9);
    // Ausnahme fuer 9 als erste Stelle; nach Hinweis von Michael
    // Plugge <m.plugge@fh-mannheim.de>
    if (result > 9) {
	if (account[0] == 9) {
	    if (result == 10)
		result = 7;
	    else
		result = 8;
	} else
	    return AccountNumberCheck::ERROR;
    }
    if (result == account[9])
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_57(int *account, int *weight) {
    // validated with 8 test accountIDs (4 with + 4 w/o checksum)
    // many numbers do not have a checksum:
    int firstTwo = atoi(array2Number(account).substr(0, 2).c_str());
    int firstSix = atoi(array2Number(account).substr(0, 6).c_str());
    if ((51 > firstTwo) || (91 == firstTwo) || (95 < firstTwo) ||
	(777777 == firstSix) || (888888 == firstSix))
	return AccountNumberCheck::OK;

    number2Array("1212121210", weight);
    return algo01(10, weight, true, 10, account);
}
// Added by Jens Gecius, validated with four test accountIDs
AccountNumberCheck::Result method_58(int *account, int *weight) {
    if (0 == atoi(array2Number(account).substr(0,5).c_str()))
	return AccountNumberCheck::ERROR;
    number2Array("0000654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_59(int *account, int *weight) {
    // ids less than 9 digits can not be checked
    if (0 == account[0] && 0 == account[1])
	return AccountNumberCheck::OK;

    number2Array("2121212120", weight);
    return algo01(10, weight, true, 10, account);
}
AccountNumberCheck::Result method_60(int *account, int *weight) {
    number2Array("0021212120", weight);
    return algo01(10, weight, true, 10, account);
}
AccountNumberCheck::Result method_61(int *account, int *weight) {
    number2Array("2121212000", weight);
    if (8 == account[8])
	number2Array("2121212012", weight);
    return algo01(10, weight, true, 8, account);
}
// Added by Jens Gecius, validated with one test accountID
AccountNumberCheck::Result method_62(int *account, int *weight) {
    number2Array("0021212000", weight);
    return algo01(10, weight, true, 8, account);
}
AccountNumberCheck::Result method_63(int *account, int *weight) {
    if (0 != account[0])
	return AccountNumberCheck::ERROR; // Added by Jens Gecius, check on invalid accountIDs
    number2Array("0121212000", weight);
    AccountNumberCheck::Result tmp = algo01(10, weight, true, 8, account);
    if (AccountNumberCheck::OK == tmp)
	return AccountNumberCheck::OK;
    else {
	// shift left, add 00 as subaccount id and try again
	number2Array(array2Number(account).substr(2) + "00", account);
	return algo01(10, weight, true, 8, account);
    }
}
AccountNumberCheck::Result method_64(int *account, int *weight) {
    number2Array("9058420000", weight); weight[1] = 10;
    return algo01(11, weight, false, 7, account);
}
AccountNumberCheck::Result method_65(int *account, int *weight) {
    number2Array("2121212000", weight);
    if (9 == account[8]) {
	weight[8] = 1;
	weight[9] = 2;
    }
    return algo01(10, weight, true, 8, account);
}
// Added by Jens Gecius, validated with five test accountIDs
AccountNumberCheck::Result method_66(int *account, int *weight) {
    if (0 != account[0])
	return AccountNumberCheck::ERROR;
    number2Array("700654320", weight);
    int result = algo03(11, weight, false, account, 0, 9);
    if (result == 0)
	result = 1;
    else if (result == 1)
	result = 0;
    else
	result = 11 - result;
    if (account[9] == result)
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_67(int *account, int *weight) {
    number2Array("2121212000", weight);
    return algo01(10, weight, true, 8, account);	
}
AccountNumberCheck::Result method_68(int *account, int *weight) {
   // FIXME: This needs to be checked:
   // Result of 20030000;400000000;68 Ausnahme;Bundesbank: ERROR, they do not match
   // Result of 20030000;499999999;68 Ausnahme;Bundesbank: ERROR, they do not match
    // Modified by Jens Gecius, verified with three test accountIDs
    // size=10
    if (0 != account[0]) {
	// and digit 4 is not "9"? error
	if (9 != account[3])
	    return AccountNumberCheck::ERROR;
	number2Array("0001212120", weight);
	if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	    return AccountNumberCheck::OK;
    } else { // size != 10
	// some accountIDs can not be checked:
	if ("400000000" <= array2Number(account) &&
	    array2Number(account) <= "499999999")
	    return AccountNumberCheck::OK;
	number2Array("0121212120", weight);
	if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	    // variant 1
	    return AccountNumberCheck::OK;
	else {
	    // variant 2
	    number2Array("0100212120", weight);
	    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
		return AccountNumberCheck::OK;
	}
    }

    return AccountNumberCheck::ERROR;
}
// Added by Jens Gecius, validated with three test accountIDs
AccountNumberCheck::Result method_69(int *account, int *weight) {
    int transform[6][10] = 
	{ {0,0,0,0,0,0,0,0,0,0}, // zero for non-transform [0][x]
	  {0,1,5,9,3,7,4,8,2,6}, // first transform line   [1][x]
	  {0,1,7,6,9,8,3,2,5,4}, // second transform line  [2][x]
	  {0,1,8,4,6,2,9,5,7,3}, // third transform line   [3][x]
	  {0,1,2,3,4,5,6,7,8,9}, // fourth transform line  [4][x]
	  {1,4,3,2,1,4,3,2,1,0} }; // which transform line

    long int lastnine = atol(array2Number(account).substr(1).c_str());
    if (account[0] == 9 && lastnine >= 300000000 && lastnine <= 399999999)
	return AccountNumberCheck::OK; // these do not get checked
    if ((account[0] == 9 && (lastnine <= 700000000 || lastnine >= 799999999)) ||
	account[0] != 9) {
	// variant 1
	number2Array("8765432000", weight);
	if (AccountNumberCheck::OK == algo01(11, weight, false, 8, account))
	    return AccountNumberCheck::OK;
    }
    // variant 2
    return algo07(account, transform);
}
AccountNumberCheck::Result method_70(int *account, int *weight) {
    number2Array("4327654320", weight);
    if (5 == account[3] ||
	(6 == account[3] && 9 == account[4]))
	number2Array("0007654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_71(int *account, int *weight) {
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
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_72(int *account, int *weight) {
    number2Array("0001212120", weight);
    return algo01(10, weight, true, 10, account);
}
// Added by Jens Gecius, validated with two test accountIDs
AccountNumberCheck::Result method_73(int *account, int *weight) {
    // Modified by Jens Gecius, algo changed December 6th, 2004
    // Checked with (positive only) Bundesbank-Testnumbers
    if (9 == account[2]) {
	number2Array("0087654320", weight);
	if (algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK; // Variant 1
	number2Array("0987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account); // Variant 2
    }		   // Exception to method C, Method 51
    //        if (account[2] == 9) {
    //            number2Array("0987654320", weight); weight[0] = 10;
    //            return algo01(11, weight, false, 10, account);
    //        }
    number2Array("0001212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    number2Array("0000212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    return algo01(7, weight, true, 10, account);
}
// Added by Jens Gecius, validated with one (double) test accountID
AccountNumberCheck::Result method_74(int *account, int *weight,
				     const std::string& accountId, const std::string& bankId) {
    number2Array("2121212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    if (accountId.length() < 7) {
	if (account[9] == (5 - (algo03a(weight, true, account, 0, 9) % 5)))
	    return AccountNumberCheck::OK;
    }
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_75(int *account, int *weight) {
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
AccountNumberCheck::Result method_76(int *account, int *weight) {
    // Modified by Jens Gecius, validated with four test accountIDs
    number2Array("0765432000", weight);
    int tmp = algo03(11, weight, false, account, 0, 6);

    if (tmp == account[7] && (account[0] == 0 || account[0] == 4 || account[0] > 5) )
	return AccountNumberCheck::OK;
    else if (account[0] == 0 && account[1] == 0) {
	// shift left if two lefts are 0, add 00 as subaccount id and try again
	number2Array(array2Number(account).substr(2) + "00", account);
	tmp = algo03(11, weight, false, account, 0, 6);
	if (tmp == account[7] && (account[0] == 0 || account[0] == 4 || account[0] > 5) )
	    return AccountNumberCheck::OK;
    }
    return AccountNumberCheck::ERROR;
}
// Added by Jens Gecius, validated with four test accountIDs
AccountNumberCheck::Result method_77(int *account, int *weight) {
    number2Array("0000054321", weight);
    if (0 == algo03(11, weight, false, account, 0, 9))
	return AccountNumberCheck::OK;
    weight[8] = 4; weight[9] = 5;
    if (0 == algo03(11, weight, false, account, 0, 9))
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_78(int *account, int *weight) {
    if (0 == account[0] && 0 == account[1])
	return AccountNumberCheck::OK;
	
    number2Array("2121212120", weight);
    return algo01(10, weight, true, 10, account);	
}
AccountNumberCheck::Result method_79(int *account, int *weight) {
    // Modified by Jens Gecius, not verified as no bank seems to use it...
    if (0 == account[0])
	return AccountNumberCheck::ERROR;
    int checkIndex = 10;

    if (2 < account[0] && 9 > account[0])
	number2Array("2121212120", weight);
    else {
	checkIndex--;
	number2Array("1212121200", weight);
    }

    return algo01(10, weight, true, checkIndex, account);
}
AccountNumberCheck::Result method_80(int *account, int *weight) {
    // exception:
    // Modified by Jens Gecius, algo changed June 8th, 2004
    // Checked with (positive only) Bundesbank-Testnumbers
    if (9 == account[2]) {
	number2Array("0087654320", weight);
	if (algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK; // Variant 1
	number2Array("0987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account); // Variant 2
    }		   // Exception to method C, Method 51
    // if (9 == account[2] && 9 == account[3]) {
    //	  number2Array("987654320", weight); weight[0] = 10;
    //	  return algo01(11, weight, false, 10, account);
    //	}
    // var 1
    number2Array("0000212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;

    // var 2:
    return algo01(7, weight, true, 10, account);
}
AccountNumberCheck::Result method_81(int *account, int *weight) {
    // Modified by Jens Gecius, validated with three test accountIDs
    // Modified by Jens Gecius, algo changed September 6th, 2004
    // Checked with (positive only) Bundesbank-Testnumbers
    if (9 == account[2]) {
	number2Array("0087654320", weight);
	if (algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK; // Variant 1
	number2Array("0987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account); // Variant 2
    }		   // Exception to method C, Method 51
    number2Array("0007654320", weight); weight[0] = 10;
    //	if (9 != account[2]) {
    //	  weight[1] = 0; weight[2] = 0;
    //	}
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_82(int *account, int *weight) {
    if (9 == account[2] && 9 == account[3]) {
	number2Array("987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account);
    } else {
	number2Array("0000654320", weight);
	return algo01(11, weight, false, 10, account);
    }
}
AccountNumberCheck::Result method_83(int *account, int *weight) {
    // var a:
    number2Array("0007654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // var b:
    number2Array("0000654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // var c:
    if (account[9] > 6)
	return AccountNumberCheck::ERROR;
    if (AccountNumberCheck::OK == algo01(7, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // non-customer accounts
    if (9 != account[2] || 9 != account[3])
	return AccountNumberCheck::ERROR;

    number2Array("0087654320", weight);
    return algo01(11, weight, false, 10, account);	
}
AccountNumberCheck::Result method_84(int *account, int *weight) {
    // exception:
    // Modified by Jens Gecius, algo changed September 6th, 2004
    // Checked with (positive only) Bundesbank-Testnumbers
    if (9 == account[2]) {
	number2Array("0087654320", weight);
	if (algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK; // Variant 1
	number2Array("0987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account); // Variant 2
    }		   // Exception to method C, Method 51
    //	if (9 == account[2] && 9 == account[3]) {
    //	  number2Array("987654320", weight); weight[0] = 10;
    //	  return algo01(11, weight, false, 10, account);	  	  
    //	}

    // var 1
    number2Array("0000654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // var2
    return algo01(7, weight, false, 10, account);
}
AccountNumberCheck::Result method_85(int *account, int *weight) {
    if (9 == account[2] && 9 == account[3]) {
	number2Array("0087654320", weight);
	return algo01(11, weight, false, 10, account);
    }

    // var a:
    number2Array("0007654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // var b:
    number2Array("0000654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // var c:
    if (account[9] > 6)
	return AccountNumberCheck::ERROR;

    number2Array("0000654320", weight);
    return algo01(7, weight, false, 10, account);
}
AccountNumberCheck::Result method_86(int *account, int *weight) {
    // exception:
    // Modified by Jens Gecius, algo changed September 6th, 2004
    // Checked with (positive only) Bundesbank-Testnumbers
    if (9 == account[2]) {
	number2Array("0087654320", weight);
	if (algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK; // Variant 1
	number2Array("0987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account); // Variant 2
    }		   // Exception to method C, Method 51
    //	if (9 == account[2]) {
    //	  number2Array("987654320", weight); weight[0] = 10;
    //	  return algo01(11, weight, false, 10, account);	  
    //	}

    // var 1
    number2Array("0001212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;

    // var2: on error try with 32
    number2Array("0007654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_87(int *account, int *weight,
				     const std::string& accountId, const std::string& bankId) {
    // Modified by Jens Gecius, algo changed September 6th, 2004
    // Checked with (positive only) Bundesbank-Testnumbers
    if (9 == account[2]) {
	number2Array("0087654320", weight);
	if (algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK; // Variant 1
	number2Array("0987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account); // Variant 2
    }		   // Exception to method C, Method 51
    // method a, b, c not verified in regard of changes (Sep 6th, 2004)!
    //	if (9 == account[2]) {
    // calc with method 10
    //	  number2Array("987654320", weight); weight[0] = 10;
    //	  return algo01(11, weight, false, 10, account);
    //	}

    // method a
    if (AccountNumberCheck::OK == algo06(accountId))
	return AccountNumberCheck::OK;

    // method b
    number2Array("0000654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // method c
    return algo01(7, weight, false, 10, account);
}
AccountNumberCheck::Result method_88(int *account, int *weight) {
    number2Array("0007654320", weight);
    if (9 == account[2])
	number2Array("0087654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_89(int *account, int *weight) {
    // some numbers can not be checked
    std::string acc10 = array2Number(account);
    if (acc10 < "0000999999" || 0 != account[0])
	return AccountNumberCheck::OK;

    // those with length 7
    if (0 == account[0] && 0 == account[1] && 0 == account[2]) {
	number2Array("0007654320", weight);
	return algo01(11, weight, true, 10, account);
    }

    // length 8 or 9? check with method 10
    number2Array("987654320", weight); weight[0] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_90(int *account, int *weight) {
    // variant a
    number2Array("0007654320", weight);	
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // variant b
    number2Array("0000654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;
	
    // variant c
    if (AccountNumberCheck::OK == algo01(07, weight, false, 10, account))
	return AccountNumberCheck::OK;
	
    // variant d
    if (AccountNumberCheck::OK == algo01(9, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // variant e
    number2Array("0000212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // non-customer-accounts
    number2Array("0087654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_91(int *account, int *weight) {
    number2Array("7654320000", weight);
    AccountNumberCheck::Result tmp = algo01(11, weight, false, 7, account);
    if (AccountNumberCheck::OK == tmp)
	return AccountNumberCheck::OK;
    else {	
	number2Array("2345670000", weight);
	tmp = algo01(11, weight, false, 7, account);
	if (AccountNumberCheck::OK == tmp)
	    return AccountNumberCheck::OK;
	else {	
	    number2Array("0987650432", weight); weight[0] = 10;	
	    tmp = algo01(11, weight, false, 7, account);
	    if (AccountNumberCheck::OK == tmp)
		return AccountNumberCheck::OK;
	    else {
		number2Array("9058420000", weight); weight[1] = 10;
		return algo01(11, weight, false, 7, account);
	    }
	}
    }
}
AccountNumberCheck::Result method_92(int *account, int *weight) {
    number2Array("0001731730", weight);
    return algo01(10, weight, false, 10, account);
}
AccountNumberCheck::Result method_93(int *account, int *weight) {
    // Modified by Jens Gecius, indirectly validated via method A4,
    // as no bank seems to use it...
    int checkIndex = 6;
    number2Array("6543200000", weight);
    if ("0000" == array2Number(account).substr(0, 4)) {
	checkIndex = 10;
	number2Array("0000654320", weight);	  
    }

    if (AccountNumberCheck::OK == algo01(11, weight, false, checkIndex, account))
	return AccountNumberCheck::OK;
    int result = algo03(7, weight, false, account, 0, 9);
    if (result == 0)
	result = 7;
    if (account[checkIndex] == (7 - result))
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_94(int *account, int *weight) {
    number2Array("1212121210", weight);
    return algo01(10, weight, true, 10, account);
}
AccountNumberCheck::Result method_95(int *account, int *weight) {
    // some numbers do not get checked
    std::string accNumber = array2Number(account);
    if (("0000000001" <= accNumber && accNumber <= "0001999999") ||
	("0009000000" <= accNumber && accNumber <= "0025999999") ||
	("0396000000" <= accNumber && accNumber <= "0499999999") ||
	("0700000000" <= accNumber && accNumber <= "0799999999"))
	return AccountNumberCheck::OK;

    number2Array("4327654320", weight);

    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_96(int *account, int *weight) {
    // Modified by Jens Gecius
    number2Array("1987654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;
    // check with 00
    number2Array("2121212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    std::string acc10 = array2Number(account);
    if (acc10 > "0001300000" || acc10 < "0099399999")
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_97(int *account, int *weight) {
    std::string tmp = array2Number(account).substr(0, 9);
    long_long valueX = number2LongLong(tmp);
    valueX -= (valueX / 11) * 11;
    valueX = valueX % 10;
	
    if (valueX == account[9])
	return AccountNumberCheck::OK;
    else 
	return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_98(int *account, int *weight) {
    number2Array("0037137130", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, false, 10, account))
	return AccountNumberCheck::OK;

    // on error run method 32
    number2Array("0007654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_99(int *account, int *weight) {
    // some numbers do not get checked
    if ("0396000000" <= array2Number(account) && 
	array2Number(account) <= "0499999999")
	return AccountNumberCheck::OK;
    number2Array("4327654320", weight);
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_A0(int *account, int *weight) {
    // some numbers are not checked
    if ("0000000" == array2Number(account).substr(0, 7))
	return AccountNumberCheck::OK;
    // Modified by Jens Gecius, NOT validated as no bank seems to use it
    number2Array("0000058420", weight); weight[4] = 10;
    int result = algo03a(weight, false, account, 0, 9);
    result = result % 11;
    if (result == 0 || result == 1)
	result = 11;
    if (account[9] == result)
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
AccountNumberCheck::Result method_A1(int *account, int *weight) {
    // Modified by Jens Gecius as of recent changes, validated with six test accountIDs
    // three positives and three negatives (first algo to check negatives)
    std::string acc10 = array2Number(account);
    if ((acc10 < "1000000000" && acc10 > "0099999999") ||
	acc10 < "0010000000")
	return AccountNumberCheck::ERROR;
    number2Array("0021212120", weight);
    return algo01(10, weight, true, 10, account);
}
// Added by Jens Gecius, validated with six test accountIDs (4 positives, 2 negatives)
AccountNumberCheck::Result method_A2(int *account, int *weight) {
    number2Array("2121212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    number2Array("4327654320", weight);
    return algo01(11, weight, false, 10, account);
}
// Added by Jens Gecius, NOT validated as no bank seems to use it...
AccountNumberCheck::Result method_A3(int *account, int *weight) {
    number2Array("2121212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    number2Array("0987654320", weight); weight[0] = 10;
    return algo01(11, weight, false, 10, account);
}
// Added by Jens Gecius, validated with 16 test accountIDs (11 pos
// and 5 negs). The test accountIDs chosen in the Bundesbank paper
// are stupid, as some of the false ones are actually correct as of
// other variants also relevant for them!!!  costed me another hour
// calculating all of them by hand! grr.
AccountNumberCheck::Result method_A4(int *account, int *weight) {
    number2Array("0000654320", weight);
    if (account[2] == 9 && account[3] == 9) { // variant 3
	if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK;
    } else {
	number2Array("0007654320", weight);
	if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account)) // variant 1
	    return AccountNumberCheck::OK;
	int result = algo03(7, weight, false, account, 0, 9); // variant 2
	if (result == 0)
	    result = 7;
	if (account[9] == (7 - result))
	    return AccountNumberCheck::OK;
    }
    int checkIndex = 6;		// variant 4
    number2Array("6543200000", weight);
    if ("0000" == array2Number(account).substr(0, 4)) {
	checkIndex = 10;
	number2Array("0000654320", weight);	  
    }

    if (AccountNumberCheck::OK == algo01(11, weight, false, checkIndex, account))
	return AccountNumberCheck::OK;
    int result = algo03(7, weight, false, account, 0, 9);
    if (result == 0)
	result = 7;
    if (account[checkIndex] == (7 - result))
	return AccountNumberCheck::OK;
    return AccountNumberCheck::ERROR;
}
// Added by Jens Gecius, NOT validated as no bank seems to use it...
AccountNumberCheck::Result method_A5(int *account, int *weight) {
    number2Array("2121212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    if (account[0] == 9)
	return AccountNumberCheck::ERROR;
    number2Array("0987654320", weight); weight[0] = 10;
    return algo01(11, weight, false, 10, account);
}
AccountNumberCheck::Result method_A6(int *account, int *weight) {
    if (account[1] == 8) {
	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);
    } else {
	number2Array("1731731730", weight);
	return algo01(10, weight, false, 10, account);
    }
}
AccountNumberCheck::Result method_A7(int *account, int *weight) {
    number2Array("2121212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    return algo01(10, weight, false, 10, account);
}
AccountNumberCheck::Result method_A8(int *account, int *weight) {
    // Modified by Jens Gecius, indirect change due to method 81 (see there)
    // as of September 6th, 2004
    // Checked with Bundesbank-Testnumbers; could someone investigate,
    // why test-number 3199500502 is shown correct, although it should be wrong?
    //	number2Array("0987654320", weight); weight[0] = 10;
    //	if (9 != account[2])
    //	  weight[1] = weight[2] = 0;
    if (9 == account[2]) {
	number2Array("0087654320", weight);
	if (algo01(11, weight, false, 10, account))
	    return AccountNumberCheck::OK; // Variant 1
	number2Array("0987654320", weight); weight[0] = 10;
	return algo01(11, weight, false, 10, account); // Variant 2
    }		   // Exception to method C, Method 51
    number2Array("0007654320", weight);
    if (AccountNumberCheck::OK == algo01(11, weight, false, 10, account))
	return AccountNumberCheck::OK;
    if (account[2] == 9)
	return AccountNumberCheck::ERROR;
    // Further test according to method 73 (see there), without account[2]==9
    // (not tested in variant 2)
    number2Array("0001212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    number2Array("0000212120", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, true, 10, account))
	return AccountNumberCheck::OK;
    return algo01(7, weight, true, 10, account);
    //        number2Array("0001212120", weight);
    //        return algo01(10,weight, true, 10, account);
}
AccountNumberCheck::Result method_A9(int *account, int *weight) {
    number2Array("1731731730", weight);
    if(algo01(10, weight, false, 10, account) != AccountNumberCheck::OK) {
	number2Array("4327654320", weight);
	return algo01(11, weight, false, 10, account);
    }
    return AccountNumberCheck::OK;
}
// Added by Jens Gecius (new Method as of December 6th, 2004)
// Not checked, no bank seems to use it.
AccountNumberCheck::Result method_B0(int *account, int *weight) {
    if (array2Number(account) < "1000000000" || account[0] == 8)
	return AccountNumberCheck::ERROR;
    if (account[7] == 1 || account[7] == 2 || account[7] == 3 || account[7] == 6)
	return AccountNumberCheck::OK; // variant 1 -> no check if 1,2,3 or 6 at 8th place of account (method "09")
    // Other accounts: method "06"
    number2Array("4327654320", weight);
    return algo01(11, weight, false, 10, account);
}
// Added by Jens Gecius (new Method as of September 6th, 2004)
// Checked with Bundesbank-Testnumbers
AccountNumberCheck::Result method_B1(int *account, int *weight) {
    number2Array("1371371370", weight);
    if (AccountNumberCheck::OK == algo01(10, weight, false, 10, account))
	return AccountNumberCheck::OK; // variant 1 (method "05")
    number2Array("1731731730", weight);
    return algo01(10, weight, false, 10, account);
    // variant 2 (method "01")
}
// Added by Jens Gecius (new Method as of September 6th, 2004)
// Checked with Bundesbank-Testnumbers (one of them failed the test due to deeper
// problem with algo02, where the result for method 02 is wrongly calculated!!
// result shows account is valid although it is not. I don't know about deeper
// implications of a change of that line:
//   result = (modulus - result) % 10;
// to
//   result = (modulus - result)
// as per method "02" with modulus 11 and test-result "10", it shows an invalid account,
// which is here shown as valid account.
AccountNumberCheck::Result method_B2(int *account, int *weight) {
    if (account[0] < 8) {
	number2Array("2987654320", weight);
	return algo01(11, weight, false, 10, account);
    } // variant 1 (method "02")
    if (account[0] == 8 || account[0] == 9) {
	number2Array("2121212120", weight);
	return algo01(10, weight, true, 10, account);	
    } // variant 2 (method "00")
    return AccountNumberCheck::ERROR; // should never happen, to be on the safe side
}
// Added by Jens Gecius (new method as of December 6th, 2004)
// Checked with Bundesbank-Testnumbers
AccountNumberCheck::Result method_B3(int *account, int *weight) {
    if (account[0] < 9) {
	number2Array("0007654320", weight);
	return algo01(11, weight, false, 10, account);
    } // variant 1 (method "32")
    if (account[0] == 9) {
	number2Array("4327654320", weight);
	return algo01(11, weight, false, 10, account);
    } // variant 2 (method "06")
    return AccountNumberCheck::ERROR; // should never happen, to be on the safe side
}

struct method_func_s {
    const char *str;
    AccountNumberCheck::MethodFunc func;
};
const struct method_func_s cb_funcs[] = {
  { "00", method_00},
  { "01", method_01},
  { "02", method_02},
  { "03", method_03},
  { "04", method_04},
  { "05", method_05},
  { "06", method_06},
  { "07", method_07},
  { "08", method_08},
  { "09", method_09},
  { "10", method_10},
  { "11", method_11},
  { "12", method_12},
  { "13", method_13},
  { "14", method_14},
  { "15", method_15},
  { "16", method_16},
  { "17", method_17},
  { "18", method_18},
  { "19", method_19},
  { "20", method_20},
  { "21", method_21},
  { "22", method_22},
  { "23", method_23},
  { "25", method_25},
  { "26", method_26},
  { "27", method_27},
  { "28", method_28},
  { "29", method_29},
  { "30", method_30},
  { "31", method_31},
  { "32", method_32},
  { "33", method_33},
  { "34", method_34},
  { "35", method_35},
  { "36", method_36},
  { "37", method_37},
  { "38", method_38},
  { "39", method_39},
  { "40", method_40},
  { "41", method_41},
  { "42", method_42},
  { "43", method_43},
  { "44", method_44},
  { "45", method_45},
  { "46", method_46},
  { "47", method_47},
  { "48", method_48},
  { "49", method_49},
  { "50", method_50},
  { "51", method_51},
  { "54", method_54},
  { "55", method_55},
  { "56", method_56},
  { "57", method_57},
  { "58", method_58},
  { "59", method_59},
  { "60", method_60},
  { "61", method_61},
  { "62", method_62},
  { "63", method_63},
  { "64", method_64},
  { "65", method_65},
  { "66", method_66},
  { "67", method_67},
  { "68", method_68},
  { "69", method_69},
  { "70", method_70},
  { "71", method_71},
  { "72", method_72},
  { "73", method_73},
  { "75", method_75},
  { "76", method_76},
  { "77", method_77},
  { "78", method_78},
  { "79", method_79},
  { "80", method_80},
  { "81", method_81},
  { "82", method_82},
  { "83", method_83},
  { "84", method_84},
  { "85", method_85},
  { "86", method_86},
  { "88", method_88},
  { "89", method_89},
  { "90", method_90},
  { "91", method_91},
  { "92", method_92},
  { "93", method_93},
  { "94", method_94},
  { "95", method_95},
  { "96", method_96},
  { "97", method_97},
  { "98", method_98},
  { "99", method_99},
  { "A0", method_A0},
  { "A1", method_A1},
  { "A2", method_A2},
  { "A3", method_A3},
  { "A4", method_A4},
  { "A5", method_A5},
  { "A6", method_A6},
  { "A7", method_A7},
  { "A8", method_A8},
  { "A9", method_A9},
  { "B0", method_B0},
  { "B1", method_B1},
  { "B2", method_B2},
  { "B3", method_B3},
  { 0, 0} // Important: The array has to end with the {0,0} entry,
	  // otherwise initMethodMap() will runaway and crash!
};


// This second structure and array is only a quick workaround for
// those method functions which use the account and bankId
// string. Since 95% of all methods don't use them, I decided against
// using the MethodFuncLong prototype for all methods. Maybe in the
// future, these five methods will also switch to the MethodFunc
// prototype i.e. do not use the accountId and bankId
// string. Whatever, for now this works quite well.
struct method_funcLong_s {
    const char *str;
    AccountNumberCheck::MethodFuncLong func;
};
const struct method_funcLong_s cb_funcs_long[] = {
   { "24", method_24},
   { "52", method_52},
   { "53", method_53},
   { "74", method_74},
   { "87", method_87},
   { 0, 0} // Important: The array has to end with the {0,0} entry,
           // otherwise initMethodMap() will runaway and crash!
};

void AccountNumberCheck::initMethodMap()
{
  const struct method_func_s* ptr = cb_funcs;
  for ( ; ptr->str != 0; ptr++)
  {
     method_map.insert(method_map_t::value_type(ptr->str, ptr->func));
  }

  const struct method_funcLong_s* ptr2 = cb_funcs_long;
  for ( ; ptr2->str != 0; ptr2++)
  {
     method_map2.insert(method_map2_t::value_type(ptr2->str, ptr2->func));
  }
}
