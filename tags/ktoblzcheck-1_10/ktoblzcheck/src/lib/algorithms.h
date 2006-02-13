/*-*-c++-*-*****************************************************************
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

#include "ktoblzcheck.h"

/** @file 
 * @brief Internal algorithms
 *
 * This file contains the internal algorithms that are used for
 * checking account numbers.
 */
#ifdef HAVE_LONG_LONG
# define long_long long long
#else
# define long_long long
#endif // HAVE_LONG_LONG

// forward declarations
void   multArray(const int *a, const int *b, int dest[10]);
/** @param source Must be array of size 10 
 * @param dest DOCUMENTME 
 * @param start DOCUMENTME 
 * @param stop DOCUMENTME 
*/
void   crossFoot(const int *source, int dest[10], int start, int stop);
/** @param source Must be array of size 10 */
void   crossFoot(int *source);
/** @param source Must be array of size 10
 * @param start DOCUMENTME 
 * @param stop DOCUMENTME 
*/
int    add(const int *source, int start, int stop);
/** @param source Must be array of size 10 */
inline int add_10(const int *source);
std::string array2Number(int a[10]);
void   number2Array(const std::string& number, int a[10]);
long_long number2LongLong(const std::string& number);

int    algo02(int modulus, int weight[10], bool crossfoot, int accountId[10]);
int    algo03(int modulus, int weight[10], bool crossfoot, int accountId[10], 
			  int startAdd, int stopAdd);
int    algo03a(int weight[10], bool crossfoot, int accountId[10], 
			  int startAdd, int stopAdd);
int    algo05(int modulus1, int modulus2, int weight[10], int accountId[10],
			  int startAdd, int stopAdd);
AccountNumberCheck::Result 
algo04(const std::string& bankId, std::string accountId);
AccountNumberCheck::Result 
algo04a(const std::string& bankId, std::string accountId);
AccountNumberCheck::Result
algo06(const std::string& accountId);
AccountNumberCheck::Result
algo07(int accountId[10], int transform[6][10]);
AccountNumberCheck::Result 
algo01(int modulus, int weight[10], bool crossfoot, 
       int checkIndex, int accountId[10]);

std::string accnum_getRegKey(const char *value_name);

inline int add_10(const int *sp) {
    return sp[0] + sp[1] + sp[2] + sp[3] + sp[4] 
	+ sp[5] + sp[6] + sp[7] + sp[8] + sp[9];
}
