/**-*-c++-*-****************************************************************
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

#ifndef KTOBLZCHECK_H
#define KTOBLZCHECK_H


#ifdef __cplusplus

#include <string>
#include <map>

/** Class that stores a list of known banks, returns banks with given
 * bank codes and validates account numbers accordings to the bank's
 * known validation/checking algorithms.
 *
 * The list of known banks is read from the configuration file when
 * this object is constructed.
 */
class AccountNumberCheck {
public:

  /**
   * Gives information about the success of the check
   * <ul>
   * <li><b>OK:</b> everything is ok, account and bank match
   *
   * <li><b>UNKNOWN:</b> Could not be validated because the validation
   * algorithm is unknown/unimplemented in ktoblzcheck
   *
   * <li><b>ERROR:</b> The result of the validation algorithm is that
   * the account and bank probably do <b>not</b> match
   *
   * <li><b>BANK_NOT_KNOWN:</b> No bank with the specified bankid
   * could be found
   *
   * </ul>
   */
  typedef enum Result {
      /** Everything is ok: account and bank match */
      OK = 0,
      /** Could not be validated because the validation algorithm is
       * unknown/unimplemented in ktoblzcheck */
      UNKNOWN = 1,
      /** The result of the validation algorithm is that the account
       * and bank probably do <b>not</b> match */
      ERROR = 2,
      /** No bank with the specified bankid could be found */
      BANK_NOT_KNOWN = 3
  } Result;
  
  /**
   * This class holds information about one bank.
   *
   * <ul>
   * <li><b>bankId</b> The id of the bank (german BLZ)</li>
   *
   * <li><b>method</b> The method used to validate accountId and
   * bankId (This information is only needed in AccountNumberCheck
   * internally; an application probably does not need this)</li>
   *
   * <li><b>bankName</b> The name of the bank as listed in the file of the 
   * <b>Deutsche Bundesbank</b></li>
   *
   * <li><b>location</b> The town where the bank is located</li>
   * </ul>
   */
  class Record {
    public:
    /** The id of the bank (german BLZ) */
    unsigned long bankId;
    /** The method used to validate accountId and bankId.
     * 
     * This information is only needed in AccountNumberCheck
     * internally; an application probably does not need this. */
    std::string method;
    /**  The name of the bank as listed in the file of the 
     * <b>Deutsche Bundesbank</b>*/
    std::string bankName;
    /** The town where the bank is located */
    std::string location;
    /** Default Constructor for an empty record. */
    Record();
    /** Constructor with all values */
    Record(unsigned long id, const std::string& method, 
	   const std::string& name, 
	   const std::string& loc);
  };

  /**
   * Default Constructor. 
   *
   * This constructor also initializes the bank-database.  The bank
   * data is obtained from the location specified at compiler
   * time. Default is $prefix/share/ktoblzcheck/bankdata.txt (set in
   * configure.in) where default for $prefix is /usr/local. 
   *
   * On Windows, this constructor also looks up the location of the
   * bank data in the registry in the key
   * HKEY_LOCAL_MACHINE/Software/Ktoblzcheck/Paths, key "datadir".
   */
  AccountNumberCheck();

  /**
   * Constructor that initialize the bank-database from a data file at
   * <code>filename</code>. 
   *
   * If the file could not be found, then the resulting
   * AccountNumberCheck object simply has an empty bank database and a
   * message is printed to stderr.
   *
   * @param filename The absolute location of the KTOBLZCheck-database
   */
  AccountNumberCheck(const std::string& filename);

  /**
   * Destructor. All entries of the bank database are deleted as well.
   *
   * @note This destructor also destroys any references that have been
   * returned by findBank()! You have to make sure not to use these
   * references any longer after this destructor has been called.
   */
  ~AccountNumberCheck();


  /**
   * Check if <code>bankId</code> and <code>accountId</code> form a valid
   * combination.
   * @param bankId The bank code (BLZ) of the bank to test
   * @param accountId The account id to check
   * @param method If set, force the use of specified check-method
   */
  Result check(const std::string& bankId, const std::string& accountId, 
	       const std::string& method="") const; 

  /**
   * Find the info-record for a bank specified by <code>bankId</code>
   * or otherwise throw an exception.
   *
   * @note The returned objects are still owned by this object! You
   * have to make sure not to use the returned references after
   * the destructor ~AccountNumberCheck() has been called.
   *
   * @throws int if a bank with bank code <code>bankId</code> (german
   * BLZ) could not be found in the database
   *
   * @return A reference to the resulting bank object. The returned
   * reference is still owned by this AccountNumberObject and it
   * becomes invalid if the AcccountNumberObject is being deleted.
   */
  const Record& findBank(const std::string& bankId) const;

  /**
   * Returns the number of bank-records currently loaded
   */
  unsigned int bankCount() const;

  /**
   * Generates an index over the bankIds. This is supposed to speed up
   * the checking if you want to check 100s of combination
   * (batch-processing)
   *
   * Currently this does nothing. The bank list structure (std::map)
   * does not need this anyway.
   */
  void createIndex();


private:
  /** The type of the list of the bank data */
  typedef std::map<unsigned long, Record*> banklist_type;
  /** The list of the bank data */
  banklist_type data;

public:
  typedef Result (*MethodFunc)(int *account, int *weight);
  typedef Result (*MethodFuncLong)(int *account, int *weight, 
				   const std::string& accountId, const std::string& bankId);
private:
  typedef std::map<std::string, MethodFunc> method_map_t;
  method_map_t method_map;
  typedef std::map<std::string, MethodFuncLong> method_map2_t;
  method_map2_t method_map2;

  /** Deletes all records inside the bank data list */
  void deleteList();
  /** The function to actually read data from file into the list;
      clears all existing data. */
  void readFile(const std::string &filename);

  /** Initialized the map of method string to the respective callback function */
  void initMethodMap();
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

  /**
   * Default Constructor. 
   *
   * This constructor also initializes the bank-database.  The bank
   * data is obtained from the location specified at compiler
   * time. Default is $prefix/share/ktoblzcheck/bankdata.txt (set in
   * configure.in) where default for $prefix is /usr/local. 
   *
   * On Windows, this constructor also looks up the location of the
   * bank data in the registry in the key
   * HKEY_LOCAL_MACHINE/Software/Ktoblzcheck/Paths, key "datadir".
   */
  extern AccountNumberCheck *AccountNumberCheck_new();

  /**
   * Constructor that initialize the bank-database from a data file at
   * <code>filename</code>. 
   *
   * If the file could not be found, then the resulting
   * AccountNumberCheck object simply has an empty bank database and a
   * message is printed to stderr.
   *
   * @param filename The absolute location of the KTOBLZCheck-database
   */
  extern AccountNumberCheck *AccountNumberCheck_new_file(const char *filename);

  /**
   * Destructor. All entries of the bank database are deleted as well.
   *
   * @note This destructor also destroys any references that have been
   * returned by AccountNumberCheck_findBank()! You have to make sure
   * not to use these references any longer after this destructor has
   * been called.
   */
  extern void AccountNumberCheck_delete(AccountNumberCheck *a);

  /**
   * Check if <code>bankId</code> and <code>accountId</code> form a valid
   * combination. 
   *
   * The returned value is simply an integer. The meanings of each
   * integer values are defined in the AccountNumberCheck::Result
   * enum, which is the following:
   *
   * <ul>
   * <li><b>0</b> everything is ok, account and bank match
   * <li><b>1</b> Could not be validated because the validation algorithm is unknown/unimplemented in ktoblzcheck
   * <li><b>2</b> account and bank probably do not match
   * <li><b>3</b> No bank with the specified bankid could be found
   * </ul>
   *
   * @param bankId The bank code (BLZ) of the bank to test
   * @param accountId The account id to check
   * @param method If set, force the use of specified check-method
   */
  extern AccountNumberCheck_Result
  AccountNumberCheck_check(const AccountNumberCheck *a, 
			   const char *bankId, 
			   const char *accountId); 

  /**
   * Find the info-record for a bank specified by <code>bankId</code>
   * or otherwise return NULL.
   *
   * @note The returned objects are still owned by this object! You
   * have to make sure not to use the returned references after
   * the destructor AccountNumberCheck_delete() has been called.
   *
   * @return A pointer to the resulting bank object, or NULL if no
   * matching bank could be found. The returned object is still owned
   * by this AccountNumberObject and it becomes invalid if the
   * AcccountNumberObject is being deleted.
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

  /** Copy constructor. The returned object will be owned by the
   * caller and has to be deleted by the caller when no longer
   * used. FIXME: Needs to be tested whether it works correctly --
   * internally it uses the automatically synthetized copy constructor
   * of the C++ compiler. */
  extern AccountNumberCheck_Record *
  AccountNumberCheck_Record_copy(const AccountNumberCheck_Record *a);

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

#endif /* KTOBLZCHECK_H */

