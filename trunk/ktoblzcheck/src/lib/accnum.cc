/***************************************************************************
                        accnum.cc: AccountNumberCheck's C wrappers
                             -------------------
    cvs         : accnum.cc
    begin       : May 2, 2003
    copyright   : (C) 2003 by Christian Stimming
    email       : stimming@tuhh.de

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

/** @file accnum.cc
 *
 * These are the C wrappers for AccountNumberCheck. I chose to put
 * them into a new file because ktoblzcheck.cc is already very big,
 * and the C wrappers are absolutely independent from all the rest of
 * the code.
 */
#include <config.h>
#include "ktoblzcheck.h"
#include <assert.h>


AccountNumberCheck *AccountNumberCheck_new()
{
    return new AccountNumberCheck();
}

AccountNumberCheck *AccountNumberCheck_new_file(const char *filename)
{
    return new AccountNumberCheck(std::string(filename ? filename : ""));
}

void AccountNumberCheck_delete(AccountNumberCheck *a)
{
    delete(a);
}

AccountNumberCheck_Result
AccountNumberCheck_check(const AccountNumberCheck *a, 
			 const char *bankId, 
			 const char *accountId)
{
    assert(a);
    return a->check(std::string(bankId ? bankId : ""),
		    std::string(accountId ? accountId : ""));
}

const AccountNumberCheck_Record *
AccountNumberCheck_findBank(const AccountNumberCheck *a, 
			    const char *bankId)
{
    assert(a);
    try {
	return &(a->findBank(std::string(bankId ? bankId : "")));
    } 
    catch (...) {
	return NULL;
    }
}

unsigned int AccountNumberCheck_bankCount(const AccountNumberCheck *a)
{
    assert(a);
    return a->bankCount();
}

void AccountNumberCheck_createIndex(AccountNumberCheck *a)
{
    assert(a);
    a->createIndex();
}


void
AccountNumberCheck_Record_delete(AccountNumberCheck_Record *a)
{
    delete (a);
}

unsigned long 
AccountNumberCheck_Record_bankId(const AccountNumberCheck_Record *a)
{
    assert(a);
    return a->bankId;
}

const char *
AccountNumberCheck_Record_bankName(const AccountNumberCheck_Record *a)
{
    assert(a);
    return a->bankName.c_str();
}

const char *
AccountNumberCheck_Record_location(const AccountNumberCheck_Record *a)
{
    assert(a);
    return a->location.c_str();
}
