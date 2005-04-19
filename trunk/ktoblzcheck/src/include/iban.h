/**-*-c++-*-****************************************************************
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

#ifndef IBAN_H
#define IBAN_H

#ifdef __cplusplus

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <ctype.h>

/** Stores an IBAN (International Bank Account Number) and produces
 * the electronic format (transmission format) and the printable
 * format (paper format) according to the ECBS document TR 201.
 */
class Iban
{
public:
  /** Constructor for an empty Iban. */
  Iban();
  /** Copy constructor. */
  Iban(const Iban& iban);
  /** Constructor from a string */
  Iban(const std::string& iban, bool normalize = true);
  /** Default destructor. */
  ~Iban();

  /** Returns the transmission format for the IBAN */
  const std::string& transmissionForm() {
    if (m_transmission.empty()) createTransmission(m_printable);
    return m_transmission;
  }

  /** Returns the printable format for the IBAN */
  const std::string& printableForm() {
    if (m_printable.empty()) createPrintable();
    return m_printable;
  }

private:
  std::string m_transmission; ///< stored electronic format
  std::string m_printable;    ///< stored paper format

  /** create the electronic format */
  void createTransmission(const std::string& iban_str);
  /** create the paper format */
  void createPrintable();
};

/** Stores a mapping of IBAN-prefixes to required length of the IBAN
 * and BIC (Bank Identification Code) position inside the IBAN, and a
 * second mapping of the ISO 3166 2-character country code to the list
 * of prefixes for that country (usually one, identically to the
 * country code).
 *
 * The mappings are read from a file when an instance of the class is
 * created.
 */
class IbanCheck {
public:

  /** code returned by check() (and also by bic_position()). */
  enum Result {
    // do not change anything here without changing
    // the initialisation of m_ResultText!
    OK = 0,            ///< IBAN is formally correct (length and checksum)
    TOO_SHORT,         ///< IBAN is too short to even check
    PREFIX_NOT_FOUND,  ///< the 2-character IBAN prefix is unknown
    WRONG_LENGTH,      ///< IBAN has the wrong length
    COUNTRY_NOT_FOUND, ///< the country code to check against is unknown
    WRONG_COUNTRY,     ///< the IBAN doesn't belong to the country
    BAD_CHECKSUM,      ///< bad IBAN checksum
  };


  /** Constructor that initalizes the mappings from a data file at
   * @a filename.
   *
   * If the file could not be found or is not successfully read, the
   * mappings will be empty. Use error() to check for such an error
   * condition.
   *
   * @param filename The (preferably) absolute location of the data
   * file
   */
  IbanCheck(const std::string& filename = "");
  ~IbanCheck();

  /** check the formal correctness of a given iban.  This function
   * checks if the prefix is known, if the length is correct for the
   * prefix, if the checksum is ok and if the prefix is valid for a
   * given country (if set).
   * @param iban     Iban instance
   * @param country  2-character country code (ISO 3166)
   */
  Result check(Iban& iban, const std::string& country = "") {
    return check(iban.transmissionForm(), country); }

  /** @overload
   * @param iban     IBAN in transmission format
   * @param country  2-character country code (ISO 3166)
   */
  Result check(const std::string& iban, const std::string& country = "");

  /** convert Result code into an english message string.
   *
   * @note if the value of @a res is an integer not inside
   * the enum range, a special message will be returned.
   *
   * @param res   Result code from check() or bic_position()
   */
  static const char *resultText(Result res);

  /** Returns the position of the BIC inside the IBAN. The iban should
   * be formally correct, if not an error Result might be returned.
   *
   * @param[in] iban    IBAN in transmission format
   * @param[out] start  start of BIC (0-based index)
   * @param[out] end    first position after BIC (0-based index)
   */
  Result bic_position(const std::string& iban, int& start, int& end);

  /** @return false if the data file could not be opened and
   * successfully read.
   */
  bool error() { return m_IbanSpec.size() == 0; }

  /** uses the example data to test the check routines.
   * @return false if not successfull
   */
  bool selftest();

private:

  static const char *m_ResultText[];

  typedef std::vector<std::string> svector;

  struct Spec {
    std::string prefix;
    unsigned int length;
    unsigned int bic_start, bic_end;
    std::string example;
  };

  typedef std::map<std::string,Spec*> specmap;

  struct Country {
    std::string country;
    svector prefixes;
  };

  typedef std::map<std::string, Country*> countrymap;

  friend std::istream& operator>>(std::istream &is, Spec &spec);
  friend std::istream& operator>>(std::istream &is, Country &c);

  bool readSpecTable(std::istream &fin, const std::string& stopcomment);
  bool readCountryTable(std::istream &fin);
  int to_number(char c) { return c - 'A' + 10; }
  std::string IbanCheck::iban2number(const std::string& iban);
  int IbanCheck::modulo97(const std::string& number);

  specmap m_IbanSpec;
  countrymap m_CountryMap;
};

extern "C" {
#else /* __cplusplus */
  typedef struct IbanCheck IbanCheck;
  typedef struct Iban Iban;
#endif /* __cplusplus */
  extern IbanCheck *IbanCheck_new(const char *filename);
  extern void IbanCheck_free(IbanCheck *p);
  extern int IbanCheck_error(IbanCheck *p);
  extern int IbanCheck_check_str(IbanCheck *p, const char *iban,
				 const char *country);
  extern int IbanCheck_check_iban(IbanCheck *p, Iban *iban,
				  const char *country);
  extern int IbanCheck_bic_position(IbanCheck *p, const char *iban,
				    int *start, int *end);
  extern Iban *Iban_new(const char* iban, int normalize);
  extern void Iban_free(Iban *p);
  extern const char *Iban_transmissionForm(Iban *iban);
  extern const char *Iban_printableForm(Iban *iban);
  extern const char *IbanCheck_resultText(int res);
  extern int IbanCheck_selftest(IbanCheck *p);
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* IBAN_H */
