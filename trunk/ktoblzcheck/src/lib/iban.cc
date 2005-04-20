/***************************************************************************
                             -------------------
    cvs         : $Id$
    begin       : Tue Apr 19 2005
    copyright   : (C) 2005 by Andreas Degert (some parts Gerhard Gappmeier)
    email       : ad@papyrus-gmbh.de

    based on the older version from Gerhard Gappmeier
    (gerhard.gappmeier@ascolab.com)

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

#include "algorithms.h"
#include "iban.h"
#include <assert.h>

// ////////////////////////////////////////////////////////////

Iban::Iban()
  : m_transmission()
  , m_printable()
{
}

Iban::~Iban()
{
}

Iban::Iban(const Iban& iban)
  : m_transmission(iban.m_transmission)
  , m_printable(iban.m_printable)
{
}

Iban::Iban(const std::string& iban, bool normalize)
  : m_transmission(normalize ? createTransmission(iban) : iban)
  , m_printable()
{
}

std::string Iban::createTransmission(const std::string& str)
{
  std::string result;
  std::istringstream is(str);
  while (is) {
    std::string s;
    is >> s;
    if (s.empty())
      break;
    std::string::iterator si = s.begin();
    for (; si != s.end(); si++)
      *si = toupper(*si);
    result.append(s);
  }
  if (result.substr(0,4) == "IBAN")
    result = result.substr(4);
  return result;
}

std::string Iban::createPrintable() const
{
  std::string result = m_transmission.substr(0,4);
  for (int i = 4; ; i += 4) {
    int n = m_transmission.size() - i;
    if (n <= 0)
      break;
    else if (n > 4)
      n = 4;
    std::string s = m_transmission.substr(i,n);
    result += " " + s;
  }
  return result;
}


// ////////////////////////////////////////////////////////////

const char* IbanCheck::m_ResultText[] = {
  "IBAN is ok",
  "IBAN is too short",
  "unknown IBAN country prefix",
  "wrong length of IBAN",
  "country for IBAN not found",
  "IBAN prefix does not match for country",
  "IBAN has incorrect checksum",
  "unknown Result code!",
};

bool IbanCheck::selftest()
{
  bool ret = true;
  specmap::iterator it = m_IbanSpec.begin();
  for (; it != m_IbanSpec.end(); it++) {
    const Iban& ex = it->second->example;
    int r = check(ex.transmissionForm(), ex.transmissionForm().substr(0,2));
    if (r != OK) {
      std::cout << r << " " << it->second->example << std::endl;
      ret = false;
    }
  }
  return ret;
}

std::string IbanCheck::iban2number(const std::string& iban)
{
  std::ostringstream s;

  for (unsigned i = 4; i < iban.size(); i++) {
    if (iban[i] >= '0' && iban[i] <= '9') {
      // add numeric values to substitution string
      s << iban[i];
    }
    else {
      // convert A-Z into its numeric expression,
      // A='10' - Z='35' according the IBAN conversion table (ISO 7064)
      s << to_number(iban[i]);
    }	
  }

  // add country code
  s << to_number(iban[0]);
  s << to_number(iban[1]);

  // add checksum
  s << iban[2];
  s << iban[3];

  return s.str();
}

/** this method is calculating modulo97 for huge numbers. */
int IbanCheck::modulo97(const std::string& number)
{
  unsigned int iPos = 0;
  int iModLen = 0;
  std::string part;
  long lNum = 0;

  while (iPos < number.size()) {
    // build number with maximal 9 digits
    unsigned int n = 9 - iModLen;
    if (iPos + n > number.size())
      n = number.size() - iPos;
    part += number.substr(iPos, n);
    iPos += part.size() - iModLen;

    // convert string to long
    std::istringstream is(part);
    is >> lNum;
    // calc the module 97
    lNum = lNum % 97;
    // convert module97 to string
    std::ostringstream os;
    os << lNum;
    part = os.str();
    // iModLen digits are used to build the next 9 digits number
    iModLen = part.size();
  }

  return lNum;
}

IbanCheck::Result IbanCheck::check(const std::string& iban, const std::string& country) const
{
  if (iban.size() < 2)
    return TOO_SHORT;
  std::string prefix = iban.substr(0,2);
  specmap::const_iterator si = m_IbanSpec.find(prefix);
  if (si == m_IbanSpec.end())
    return PREFIX_NOT_FOUND;
  if (iban.size() != si->second->length)
    return WRONG_LENGTH;
  if (!country.empty()) {
    countrymap::const_iterator ci = m_CountryMap.find(country);
    if (ci == m_CountryMap.end())
      return COUNTRY_NOT_FOUND;
    svector sl = ci->second->prefixes;
    svector::const_iterator p = sl.begin();
    for (; p != sl.end(); p++) {
      if (*p == prefix)
	break;
    }
    if (p == sl.end())
      return WRONG_COUNTRY;
  }
  if (modulo97(iban2number(iban)) != 1)
    return BAD_CHECKSUM;
  return OK;
}

IbanCheck::Result IbanCheck::bic_position(const std::string& iban,
					  int& start, int& end) const
{
  if (iban.size() < 2)
    return TOO_SHORT;
  std::string prefix = iban.substr(0,2);
  specmap::const_iterator si = m_IbanSpec.find(prefix);
  if (si == m_IbanSpec.end())
    return PREFIX_NOT_FOUND;
  start = si->second->bic_start;
  end = si->second->bic_end;
  return OK;
}

std::istream& operator>>(std::istream &is, IbanCheck::Spec &spec)
{
  is >> spec.prefix >> spec.length
     >> spec.bic_start >> spec.bic_end >> std::ws;
  is.ignore();
  getline(is, spec.example);
  return is;
}

std::istream& operator>>(std::istream &is, IbanCheck::Country &c)
{
  std::string words;
  is >> c.country >> words;
  int pos = 0;
  while (true) {
    int n = words.find("|", pos);
    if (n < 0)
      break;
    c.prefixes.push_back(words.substr(pos, n - pos));
    pos = n + 1;
  }
  c.prefixes.push_back(words.substr(pos));
  is.ignore('\n');
  return is;
}

bool IbanCheck::readSpecTable(std::istream &fin,
			      const std::string& stopcomment)
{
  std::string line;
  while (getline(fin, line)) {
    if (line.empty())
      continue;
    if (line[0] == '#') {
      if (line == stopcomment)
	break;
      continue;
    }
    Spec &spec = *new Spec;
    std::istringstream istr(line);
    istr >> spec;
    if (!istr)
      return false;
    m_IbanSpec.insert(specmap::value_type(spec.prefix,&spec));
  }
  return true;
}

bool IbanCheck::readCountryTable(std::istream& fin)
{
  std::string line;
  while (getline(fin, line)) {
    if (line.empty() || line[0] == '#')
      continue;
    std::istringstream istr(line);
    Country &country = *new Country;
    istr >> country;
    if (!istr)
      return false;
    m_CountryMap.insert(countrymap::value_type(country.country, &country));
  }
  return true;
}

IbanCheck::IbanCheck(const std::string& filename)
{
  std::string fname = filename;
  if (fname.empty()) {
    std::string registry_path = accnum_getRegKey("datadir");
    std::string data_path = BANKDATA_PATH;
    fname = (registry_path.empty() ? data_path : registry_path)
      + 
#if OS_WIN32
      "\\"
#else
      "/"
#endif
      + std::string("ibandata.txt");
  }
  std::ifstream fin(fname.c_str());
  if (!readSpecTable(fin, "#IBAN_prefix") || !readCountryTable(fin)) {
    std::cerr << "Error reading Tables!" << std::endl;
    m_IbanSpec.clear();
    m_CountryMap.clear();
  }
}

IbanCheck::~IbanCheck()
{
  for (specmap::iterator p = m_IbanSpec.begin(); p != m_IbanSpec.end(); p++)
    delete p->second;
  for (countrymap::iterator p = m_CountryMap.begin(); p != m_CountryMap.end(); p++)
    delete p->second;
}

const char* IbanCheck::resultText(Result res)
{
  // check if res is inside of array range
  unsigned int r = res;
  unsigned int n = sizeof m_ResultText / sizeof m_ResultText[0] - 1;
  return m_ResultText[r < n ? r : n];
}

extern "C" {
  IbanCheck *IbanCheck_new(const char *filename)
  {
    return new IbanCheck(filename ? filename : "");
  }

  void IbanCheck_free(IbanCheck *p)
  {
    delete p;
  }

  int IbanCheck_error(const IbanCheck *p)
  {
    return p->error();
  }

  IbanCheck_Result 
  IbanCheck_check_str(const IbanCheck *p,
		      const char *iban, const char *country)
  {
    assert(p);
    return p->check(iban ? iban : "", country ? country : "");
  }

  IbanCheck_Result
  IbanCheck_check_iban(const IbanCheck *p,
		       const Iban *iban, const char *country)
  {
    assert(p);
    assert(iban);
    return p->check(*iban, country ? country : "");
  }

  IbanCheck_Result
  IbanCheck_bic_position(const IbanCheck *p, const char *iban,
			 int *start, int *end)
  {
    assert(p);
    assert(start);
    assert(end);
    return p->bic_position(iban ? iban : "", *start, *end);
  }

  Iban *Iban_new(const char* iban, int normalize)
  {
    return new Iban(iban ? iban : "", normalize);
  }

  void Iban_free(Iban *p)
  {
    delete p;
  }

  const char *Iban_transmissionForm(const Iban *iban)
  {
    assert(iban);
    return iban->transmissionForm().c_str();
  }

  const char *Iban_printableForm(Iban *iban)
  {
    assert(iban);
    return iban->printableForm().c_str();
  }

  const char *IbanCheck_resultText(IbanCheck_Result res)
  {
    return IbanCheck::resultText(res);
  }

  int IbanCheck_selftest(IbanCheck *p)
  {
    assert(p);
    return p->selftest();
  }
}


#if TEST
int main(int argc, char **argv)
{
  IbanCheck ck;
  if (ck.selftest())
    std::cout << "selftest ok" << std::endl;
  else
    std::cout << "selftest failed" << std::endl;
  std::string s = " iban fr14 2004 1010 0505 0001 3m02 606";
  std::cout << "test for iban    : " << s << std::endl;
  Iban iban = s;
  std::cout << "transmission form: " << iban.transmissionForm() << std::endl;
  IbanCheck::Result res = ck.check(iban);
  std::cout << "check result     : " << res
	    << " (" << ck.resultText(res) << ")" << std::endl;
  std::cout << "printable form   : " << iban.printableForm()
	    << std::endl;
  s = "FR1420041010050500013X02606";
  std::cout << "expect incorrect checksum:" << std::endl;
  res = ck.check(s);
  std::cout << s << ": " << res
	    << " (" << ck.resultText(res) << ")" << std::endl;

  return 0;
}
#endif
