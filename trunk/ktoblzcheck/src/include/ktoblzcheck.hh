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

#ifndef _ANCHECK_HH
#define _ANCHECK_HH


#include <string>
#include <list>

class AccountNumberCheck {
public:

  /**
   * Gives information about the success of the check
   * <ul>
   * <li><b>OK:</b> everything is ok, account and bank match
   * <li><b>UNKNOWN:</b> could not be validated due to an unknown reason
   * <li><b>ERROR:</b> account and bank probably do not match
   * <li><b>BANK_NOT_KNOWN:</b> the specified bankid is invalid
   * </ul>
   */
  typedef enum Result {
	OK = 0,
	UNKNOWN = 1,
	ERROR = 2,
	BANK_NOT_KNOWN = 3
  } Result;
  
  /**
   * Gives information about the bank<br>
   * <ul>
   * <li><b>bankId</b> The id of the bank (german BLZ)
   * <li><b>method</b> The method used to validate accountId and bankId<br>
   * You won't get in contact with it
   * <li><b>bankName</b> The name of the bank as listed in the file of the 
   * <b>Deutsche Bundesbank</b>
   * <li><b>location</b> The town where the bank is located
   * </ul>
   */
  struct Record {
	unsigned long bankId;
	string method;
	string bankName;
	string location;
  };

  /**
   * Check if <code>bankId</code> and <code>accountId</code> form a valid
   * combination.
   * @param method If set, force the use of specified check-method
   */
  static const Result check(string bankId, string accountId, 
							string method=""); 

  /**
   * Find the info-record for a bank specified by <code>bankId</code>
   * @throws int if <code>bankId</code> could not be found in the database
   */
  static const Record findBank(string bankId);

  // Depends on whether you specified "--with-compile-into-lib"
  // at "./configure"
#ifdef COMPILE_RESOURCE
  /**
   * Initialize the bank-database<br>
   * You must call it prior to any operation you want to perform<br>
   */
  static void init();
#else
  /**
   * Initialize the bank-database specified by <code>filename</code>
   * @param filename The absolute location of the KtoBlzCheck-database
   */
  static void init(string filename);
#endif

  /**
   * Returns the number of bank-records currently loaded
   */
  static int count();

  /**
   * Generates an index over the bankIds.<br>
   * This way you can speed up the checking if you want to check
   * 100s of combination (batch-processing)
   */
  static void createIndex();

  /**
   * Flush all resources<br>
   * Do not perform any operation except "init" afterwards
   */
  static void flush();

private:
  static list<Record*> data;
};

#endif

