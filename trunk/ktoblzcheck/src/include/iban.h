/*-*-c++-*-*****************************************************************
 *   Copyright (C) 2004 by Gerhard Gappmeier                               *
 *   gerhard.gappmeier@ascolab.com                                         *
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

#ifndef IBAN_H
#define IBAN_H

#ifdef __cplusplus
#include <string>

/** This class verifies International Bank Account Numbers (IBAN), and
 * it can create them from the parameters country, Bank code, account
 * id.
 *
 * The implementation is in an intermediate stage. It works, but any
 * improvements would be welcomed. Please feel free to even change the
 * structure of this implementation, if you think other structures
 * would be better suited to this task.
 *
 * FIXME (2005-04-11, cstim): The code in this file is VERY UNTESTED
 * and probably quite non-functional! Please fix it and also get rid
 * of the char[] array before you intend to do anything serious with
 * it!  (The rest of ktoblzcheck, on the other hand, is quite
 * functional and very well-tested.)
 *
 * @author Gerhard Gappmeier
 */
class Iban
{
   public:
      /** Enum for the selection of a particular country. Only the
       * defined variables are supported. */
      enum Country { DE = 0, AT, CH };

      /** Constructor for an empty IBAN. */
      Iban();
      /** Copy constructor. */
      Iban(const Iban&);
      /** Constructor from a string literal */
      Iban(const char*);
      /** Default destructor. */
      ~Iban();
      /** Create a new IBAN from the parameters.
       * @param country The country enum value.
       * @param bankCode The bank code
       * @param accountCode The account ID/account code/account number */
      Iban(Country country, const std::string& pszBID, const std::string& pszAccountID);

      /** Returns whether this IBAN represents a valid IBAN. */
      bool isValid() const;

      /** Factory function to create a new IBAN from the parameters.
       * @param country The country enum value.
       * @param bankCode The bank code
       * @param accountCode The account ID/account code/account number */
      static Iban create(Country country, const std::string& bankCode, const std::string& accountCode);

      /** Cast operator to return a string literal.  FIXME: It is
       * absolutely baaad practice to use a char[] array here! By all
       * means, please us a std::string instead! */
      operator const char*() const;

      /** Return a string literal. FIXME: It is absolutely baaad
       * practice to use a char[] array here! By all means, please us
       * a std::string instead! */
      const char* toChar() const;
      /** Return a string literal. FIXME: currently unimplemented. */
      const std::string& toString() const;
      
   private:
      // FIXME: It is absolutely baaad practice to use a char[] array
      // here! By all means, please us a std::string instead!
      static std::string iban2number(const char *szIban);

      static int modulo97(const char *szNumber);
      
      std::string m_sCountryCode;
      std::string m_sCheckSum;
      std::string m_sBLZ;
      std::string m_sAccountID;

      // FIXME: It is absolutely baaad practice to use a char[] array
      // here! By all means, please us a std::string instead!
      char m_szIban[34];

      bool m_bValid;
};
typedef Iban::Country Iban_Country;

extern "C" {
#else /* __cplusplus */
   typedef int Iban_Country;
   typedef struct Iban Iban;
#endif /* __cplusplus */

   /** Constructor for an empty IBAN. */
   extern Iban* Iban_new_empty();
   /** Copy constructor. */
   extern Iban* Iban_copy(const Iban* i);
   /** Constructor from a string literal */
   extern Iban* Iban_fromString(const char* c);
   /** Create a new IBAN from the parameters.
    * @param country The country enum value.
    * @param bankCode The bank code
    * @param accountCode The account ID/account code/account number */
   extern Iban* Iban_new(Iban_Country country, const char * pszBID, 
			 const char * pszAccountID);

   /** Default destructor. */
   extern void Iban_delete(Iban *i);

   /** Returns whether this IBAN represents a valid IBAN. */
   extern int Iban_isValid(const Iban *i);

   /** Return a string literal. */
   extern const char* Iban_toChar(const Iban *i);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IBAN_H */
