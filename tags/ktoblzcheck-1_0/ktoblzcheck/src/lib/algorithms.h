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

#ifdef HAVE_LONG_LONG
# define long_long long long
#else
# define long_long long
#endif // HAVE_LONG_LONG

// forward declarations
void   multArray(int a[10], int b[10], int dest[10]);
void   crossFoot(int source[10], int dest[10], int start, int stop);
int    add(int source[10], int start, int stop);
std::string array2Number(int a[10]);
void   number2Array(std::string number, int a[10]);
long_long number2LongLong(std::string number);
int    algo02(int modulus, int weight[10], bool crossfoot, int accountId[10]);
int    algo03(int modulus, int weight[10], bool crossfoot, int accountId[10], 
			  int startAdd, int stopAdd);
int    algo03a(int weight[10], bool crossfoot, int accountId[10], 
			  int startAdd, int stopAdd);
AccountNumberCheck::Result 
algo04(std::string bankId, std::string accountId);
AccountNumberCheck::Result 
algo04a(std::string bankId, std::string accountId);
AccountNumberCheck::Result 
algo05(std::string accountId);
AccountNumberCheck::Result
algo06(std::string accountId);
AccountNumberCheck::Result
algo07(int accountId[10], int transform[6][10]);
AccountNumberCheck::Result 
algo01(int modulus, int weight[10], bool crossfoot, 
       int checkIndex, int accountId[10]);

