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

#ifndef KTOBLZCHECK_HH
#define KTOBLZCHECK_HH


#ifdef __cplusplus
using namespace std;

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
  class Record {
    public:
	unsigned long bankId;
	string method;
	string bankName;
        string location;
        Record();
        Record(unsigned long id, const string& method, const string& name, 
	       const string& loc);
  };

  /**
   * Initialize the bank-database<br>
   * You must call it prior to any operation you want to perform.
   *
   * The bank data is obtained from the location specified at compiler
   * time. Default is $prefix/share/ktoblzcheck/bankdata.txt where
   * default for $prefix is /usr/local.
   */
  AccountNumberCheck();

  /**
   * Initialize the bank-database specified by <code>filename</code>
   * @param filename The absolute location of the KtoBlzCheck-database
   */
  AccountNumberCheck(const string& filename);

  /**
   * Flush all resources<br>
   * Do not perform any operation except "init" afterwards
   */
  ~AccountNumberCheck();


  /**
   * Check if <code>bankId</code> and <code>accountId</code> form a valid
   * combination.
   * @param bankId The bank code (BLZ) of the bank to test
   * @param accountId The account id to check
   * @param method If set, force the use of specified check-method
   */
  Result check(const string& bankId, const string& accountId, 
	       const string& method="") const; 

  /**
   * Find the info-record for a bank specified by <code>bankId</code>
   * or otherwise throw an exception.
   *
   * @throws int if <code>bankId</code> could not be found in the database
   */
  const Record& findBank(const string& bankId) const;

  /**
   * Returns the number of bank-records currently loaded
   */
  unsigned int bankCount() const;

  /**
   * Generates an index over the bankIds.<br>
   * This way you can speed up the checking if you want to check
   * 100s of combination (batch-processing)
   *
   * Currently not implemented.
   */
  void createIndex();


private:
  /* The list of the bank data */
  list<Record*> data;

  /** Deletes all records inside the bank data list */
  void deleteList();
  /** The function to actually read data from file into the list;
      clears all existing data. */
  void readFile(const string &filename);
};

typedef AccountNumberCheck::Result AccountNumberCheck_Result;
typedef AccountNumberCheck::Record AccountNumberCheck_Record;

extern "C" {
#else /* __cplusplus */
typedef int AccountNumberCheck_Result;
typedef struct AccountNumberCheck AccountNumberCheck;
typedef struct AccountNumberCheck_Record AccountNumberCheck_Record;
#endif /* __cplusplus */

  /*@{*/
  /** Constructor for banklist compiled into library. Returns NULL if
      not available. */
  extern AccountNumberCheck *AccountNumberCheck_new();

  /** Constructor for banklist in separate file. Returns NULL if not
      available. */
  extern AccountNumberCheck *AccountNumberCheck_new_file(const char *filename);

  /** Destructor */
  extern void AccountNumberCheck_delete(AccountNumberCheck *a);

  /**
   * Check if <code>bankId</code> and <code>accountId</code> form a valid
   * combination. 
   *
   * The returned value is simply an integer. Please look up the
   * meanings of each integer values as defined in the
   * AccountNumberCheck::Result enum.
   */
  extern AccountNumberCheck_Result
  AccountNumberCheck_check(const AccountNumberCheck *a, 
			   const char *bankId, 
			   const char *accountId); 

  /**
   * Find the info-record for a bank specified by <code>bankId</code>
   * @throws int if <code>bankId</code> could not be found in the database
   */
  extern const AccountNumberCheck_Record *
  AccountNumberCheck_findBank(const AccountNumberCheck *a, 
			      const char *bankId);

  /**
   * Returns the number of bank-records currently loaded
   */
  extern unsigned int 
  AccountNumberCheck_bankCount(const AccountNumberCheck *a);

  /**
   * Generates an index over the bankIds.<br>
   * Currently not implemented.
   */
  extern void AccountNumberCheck_createIndex(AccountNumberCheck *a);
  /*@}*/

  /*@{*/
  /** Destructor */
  extern void
  AccountNumberCheck_Record_delete(AccountNumberCheck_Record *a);

  /** Returns the id of the bank (german BLZ) */
  extern unsigned long 
  AccountNumberCheck_Record_bankId(const AccountNumberCheck_Record *a);

  /** Returns the  name of the bank as listed in the file of the 
   * <b>Deutsche Bundesbank</b> */
  extern const char *
  AccountNumberCheck_Record_bankName(const AccountNumberCheck_Record *a);

  /**  Returns the city where the bank is located */
  extern const char *
  AccountNumberCheck_Record_location(const AccountNumberCheck_Record *a);
  /*@}*/

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* KTOBLZCHECK_HH */

