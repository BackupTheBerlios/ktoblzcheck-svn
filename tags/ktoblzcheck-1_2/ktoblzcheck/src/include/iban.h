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

      /** Returns whether this IBAN represents a valid IBAN. */
      bool isValid();

      /** Factory function to create a new IBAN from the parameters.
       * @param country The country enum value.
       * @param bankCode The bank code
       * @param accountCode The account ID/account code/account number */
      static Iban create(Country country, char *bankCode, char *accountCode);

      /** Cast operator to return a string literal. */
      operator const char*();
      
   private:
      static std::string iban2number(const char *szIban);
      static int modulo97(const char *szNumber);
      
      std::string m_sCountryCode;
      std::string m_sCheckSum;
      std::string m_sBLZ;
      std::string m_sAccountID;
      char m_szIban[34];
      bool m_bValid;
};

#endif // __cplusplus

#endif // IBAN_H
