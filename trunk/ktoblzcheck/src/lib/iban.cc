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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif
#include "iban.h"
#include <stdio.h>

#ifndef __GNUC__
// A whacky workaround for MSVC
# define snprintf(s,n,f,v) sprintf(s,f,v)
#endif

/** This structure holds the necessary IBAN information for each
 * country.  Some of the data is not yet used, but might be used
 * sometime in the future.
 */
class iban_country_info
{
public:
	//iban_country_info() : countrycode(0), currencycode(0) {};
	iban_country_info(Iban::Country, unsigned, const char*, 
		unsigned, unsigned, const char*, unsigned);

      /** The value from the enum Country */
      Iban::Country enum_value;
      
      /** Numerical country code for this country according to ISO 3166-1 */
      unsigned country;

      /** The two-letter country code (Alpha-2) for this country
       * according to ISO 3166-1. */
      const char* countrycode;
      /** The (maximum?) length of bank codes in this country,
       * i.e. their number of characters. */
      unsigned bankCodeLen;
      /** The (maximum?) length of account codes / account numbers in
       * this country. */
      unsigned accountCodeLen;

      /** Three-letter code for the currency used in this country */
      const char* currencycode;
      /** Number of decimal places of the currency after the decimal
       * point/comma. */
      unsigned currency_decimalpoints;
};
iban_country_info::iban_country_info(Iban::Country e, unsigned c, const char* cc, 
		unsigned bcl, unsigned acl, const char* curc, unsigned cd)
		: enum_value(e)
		, country(c)
		, countrycode(cc)
		, bankCodeLen(bcl)
		, accountCodeLen(acl)
		, currencycode(curc)
		, currency_decimalpoints(cd)
{};

/** Define the values for some known countries here. Part of this data
 * comes from ISO 3166-1 which was replicated e.g. in the HBCI
 * specification chapter VIII.11. */
iban_country_info country_info[] = 
{
   iban_country_info( Iban::DE, 280, "DE", 8, 10, "EUR", 2 ),  
   /* A note about Germany's numerical country code. The HBCI
    * specification says: The new values since 1990 (reunification) is
    * 276, but all banks still use 280, so we stick to this old
    * value. */
   iban_country_info( Iban::AT, 40, "AT", 4, 12, "EUR", 2 ),
   iban_country_info( Iban::CH, 756, "CH", 5, 12, "CHF", 2 )
};
// FIXME: The integer value of the enum Country is directly used as
// subscript into this array! This means everything will break if the
// enum Country is changed and someone forgets this array
// accordingly. We should implement a different mapping from enum
// values to array subscripts, probably with an intermediate std::map
// from some integers (i.e. the infamous "280" for Germany) to this
// array subscript.

Iban::Iban()
{
	memset(m_szIban, 0, 34);
	m_bValid = false;
}

Iban::Iban(const Iban &copy)
{
	memset(m_szIban, 0, 34);
	m_bValid = false;
}

Iban::Iban(const char *pszIban)
{
	memset(m_szIban, 0, 34);
	m_bValid = false;
	
	// convert iban to number according the IBAN conversion table
	std::string sSubstitution = iban2number(pszIban);
		
	// calc modulo 97
	int iModulo97 = modulo97(sSubstitution.data());
	
	// modulo97 must be 1
	m_bValid = (1 == iModulo97);
}



Iban::~Iban()
{
}

bool Iban::isValid()
{
	return m_bValid;
}

std::string Iban::iban2number(const char *pszIban)
{
	std::string sSubstitution;
	char szNum[3];
			
	for (unsigned i=4; i<strlen(pszIban); i++)
	{
		if (pszIban[i] >= '0' && pszIban[i] <= '9')
		{
			// add numeric values to substitution string
			sSubstitution += pszIban[i];
		}
		else
		{
			// convert A-Z into its numeric expression, A='10' - Z='35' according the IBAN conversion table (ISO 7064)
			snprintf(szNum, 2, "%i", pszIban[i] - 'A' + 10);
			sSubstitution += szNum;
		}	
	}
	
	// add country code
	snprintf(szNum, 3, "%i", pszIban[0] - 'A' + 10);
	sSubstitution += szNum;
	snprintf(szNum, 3, "%i", pszIban[1] - 'A' + 10);
	sSubstitution += szNum;
	
	// add checksum
	sSubstitution += pszIban[2];
	sSubstitution += pszIban[3];
	
	return sSubstitution;
}

/** this method is calculating modulo97 for huge numbers. */
int Iban::modulo97(const char *pszNumber)
{
	int iLen = strlen(pszNumber);
	int iPos = 0;
	int iModLen = 0;
	char szPart[10];
	long lNum = 0;
	
	szPart[0] = 0;
	
	while (iPos < iLen)
	{
		// build number with 9 digits
		strncat(szPart, pszNumber+iPos, 9-iModLen);
		szPart[9] = 0;
		iPos += strlen(szPart) - iModLen;
		
		// convert string to long
		lNum = atoi(szPart);
		// calc the module 97
		lNum = lNum % 97;
		// convert module97 to string
		sprintf(szPart, "%li", lNum);
		// iModLen digits are used to build the next 9 digits number
		iModLen = strlen(szPart);
	}	
	
	return lNum;
}

Iban Iban::create(Country country, char *pszBID, char *pszAccountID)
{
	Iban iban;
	
	// set country code	
	iban.m_sCountryCode = country_info[country].countrycode;
	// set BID
	iban.m_sBLZ = pszBID;
	// fill up BID with preceding zeros
	while (iban.m_sBLZ.length() < country_info[country].bankCodeLen)
		iban.m_sBLZ = "0" + iban.m_sBLZ;
	// set AccountID
	iban.m_sAccountID = pszAccountID;
	// fill up AccountID with preceding zeros
	while (iban.m_sAccountID.length() < country_info[country].accountCodeLen)
		iban.m_sAccountID = "0" + iban.m_sAccountID;
		
	// build IBAN string
	strcpy(iban.m_szIban, iban.m_sCountryCode.data());
	// add dummy checksum
	strcat(iban.m_szIban, "00");
	strcat(iban.m_szIban, iban.m_sBLZ.data());
	strcat(iban.m_szIban, iban.m_sAccountID.data());
		
	// convert iban to number according the IBAN conversion table
	std::string sSubstitution = iban2number(iban.m_szIban);
		
	// calc modulo 97
	int iModulo97 = modulo97(sSubstitution.data());
	iModulo97 = 98 - iModulo97;
	
	// set new checksum
	iban.m_szIban[2] = (int)(iModulo97/10)+'0';
	iban.m_szIban[3] = (int)(iModulo97%10)+'0';
	
	return iban;
}

Iban::operator const char*()
{
	return m_szIban;
}
