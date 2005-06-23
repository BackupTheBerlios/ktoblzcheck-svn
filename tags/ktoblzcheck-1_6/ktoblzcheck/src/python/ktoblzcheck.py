# -*- encoding: latin1 -*-
#
# cvs         : $Id$
# begin       : Tue Apr 19 2005
# copyright   : (C) 2005 by Andreas Degert
# email       : ad@papyrus-gmbh.de
#
###########################################################################
#                                                                         #
#   This library is free software; you can redistribute it and/or         #
#   modify it under the terms of the GNU Lesser General Public            #
#   License as published by the Free Software Foundation; either          #
#   version 2.1 of the License, or (at your option) any later version.    #
#                                                                         #
#   This library is distributed in the hope that it will be useful,       #
#   but WITHOUT ANY WARRANTY; without even the implied warranty of        #
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     #
#   Lesser General Public License for more details.                       #
#                                                                         #
#   You should have received a copy of the GNU Lesser General Public      #
#   License along with this library; if not, write to the Free Software   #
#   Foundation, Inc., 59 Temple Place, Suite 330, Boston,                 #
#   MA  02111-1307  USA                                                   #
#                                                                         #
###########################################################################

"""KtoBlzCheck classes Record, AccountNumberCheck, Iban and IbanCheck.

Please refer to the C++ API documentation of libktoblzcheck.
Differences from C++ implementation:

- IbanCheck()              raises a ValueError if the data file cannot be read.
- IbanCheck.bic_position() raises a ValueError when the IBAN is unuseable.
"""

import os
from ctypes import c_void_p, c_char_p, c_int, c_ulong, cdll, POINTER, byref
#from enum import Enum

__all__ = ["Record", "AccountNumberCheck", "Iban", "IbanCheck"]

if os.name == 'nt':
    kto = cdll.ktoblzcheck32
else:
    kto = cdll['libktoblzcheck.so.1']


class Record(c_void_p):

    def _check_retval_(p):
        if not p:
            return None
        v = Record.__new__(Record)
        c_void_p.__init__(v, p)
        return v
    _check_retval_ = staticmethod(_check_retval_)

    def __init__(self):
        raise RuntimeError("can't intanciate class Record directly")

    bankId = property(kto.AccountNumberCheck_Record_bankId)
    bankName = property(kto.AccountNumberCheck_Record_bankName)
    location = property(kto.AccountNumberCheck_Record_location)

    def __str__(self):
        return '<%s: %s/%s/%s>' % (
            self.__class__.__name__, self.bankId, self.bankName, self.location)

kto.AccountNumberCheck_Record_delete.argtypes = c_void_p,
kto.AccountNumberCheck_Record_bankId.restype = c_ulong
kto.AccountNumberCheck_Record_bankId.argtypes = c_void_p,
kto.AccountNumberCheck_Record_bankName.restype = c_char_p
kto.AccountNumberCheck_Record_bankName.argtypes = c_void_p,
kto.AccountNumberCheck_Record_location.restype = c_char_p
kto.AccountNumberCheck_Record_location.argtypes = c_void_p,

#
# custom module Enum not provided, use simple symbol definition in
# AccountNumberCheck instead
#
#class Result(Enum):
#    (ok,              # Everything is ok: account and bank match
#     unknown,         # Could not be validated because the validation
#                      # algorithm is unknown/unimplemented in ktoblzcheck
#     error,           # The result of the validation algorithm is that
#                      # the account and bank probably do <b>not</b> match
#     bank_not_known,  # No bank with the specified bankid could be found
#     ) = range(4)


class AccountNumberCheck(c_void_p):

    (ok,              # Everything is ok: account and bank match
     unknown,         # Could not be validated because the validation
                      # algorithm is unknown/unimplemented in ktoblzcheck
     error,           # The result of the validation algorithm is that
                      # the account and bank probably do <b>not</b> match
     bank_not_known,  # No bank with the specified bankid could be found
    ) = range(4)

    def __init__(self, filename=None):
        if filename is None:
            p = kto.AccountNumberCheck_new()
        else:
            p = kto.AccountNumberCheck_new_file(filename)
        c_void_p.__init__(self, p)

    def __del__(self, f=kto.AccountNumberCheck_delete):
	f(self)

    def check(self, bankId, accountId):
        return kto.AccountNumberCheck_check(self, bankId, accountId)

    def findBank(self, bankId):
        rec = kto.AccountNumberCheck_findBank(self, bankId)
        if rec is not None:
            # prevent deleting self if there's still a Record alive
            rec._owner = self
        return rec

    bankCount = property(kto.AccountNumberCheck_bankCount)

kto.AccountNumberCheck_new.restype = c_void_p
kto.AccountNumberCheck_new_file.restype = c_void_p
kto.AccountNumberCheck_new_file.argtypes = c_char_p,
kto.AccountNumberCheck_delete.argtypes = c_void_p,
kto.AccountNumberCheck_check.restype = c_int #Result
kto.AccountNumberCheck_check.argtypes = c_void_p, c_char_p, c_char_p
kto.AccountNumberCheck_findBank.restype = Record
kto.AccountNumberCheck_findBank.argtypes = c_void_p, c_char_p
kto.AccountNumberCheck_bankCount.argtypes = c_void_p,

class Iban(c_void_p):
    def __init__(self, iban, normalize = True):
        c_void_p.__init__(self, kto.Iban_new(iban, normalize))

    def __del__(self, f=kto.Iban_free):
        f(self)

    def transmissionForm(self):
        return kto.Iban_transmissionForm(self)

    def printableForm(self):
        return kto.Iban_printableForm(self)

kto.Iban_new.restype = c_void_p
kto.Iban_new.argtypes = c_char_p, c_int
kto.Iban_free.argtypes = c_void_p,
kto.Iban_transmissionForm.restype = c_char_p
kto.Iban_transmissionForm.argtypes = c_void_p,
kto.Iban_printableForm.restype = c_char_p
kto.Iban_printableForm.argtypes = c_void_p,


class IbanCheck(c_void_p):

    (ok, too_short, prefix_not_found, wrong_length, country_not_found,
     wrong_country, bad_checksum) = range(7)

    def __init__(self, filename=None):
        c_void_p.__init__(self, kto.IbanCheck_new(filename))
        if kto.IbanCheck_error(self):
            raise ValueError("IbanCheck: file not found or wrong format")

    def __del__(self, f=kto.IbanCheck_free):
        f(self)

    def resultText(code):
        return kto.IbanCheck_resultText(code)
    resultText = staticmethod(resultText)

    def check(self, iban, country = None):
        if isinstance(iban, Iban):
            return kto.IbanCheck_check_iban(self, iban, country)
        else:
            return kto.IbanCheck_check_str(self, iban, country)

    def bic_position(self, iban):
        if isinstance(iban, Iban):
            iban = iban.transmissionForm()
        start, end = c_int(), c_int()
        res = kto.IbanCheck_bic_position(self, iban, byref(start), byref(end))
        if res != self.ok:
            raise ValueError("IBAN '%s': %s"
                             % (iban, self.resultText(res)))
        return start.value, end.value


kto.IbanCheck_new.restype = c_void_p
kto.IbanCheck_new.argtypes = c_char_p,
kto.IbanCheck_error.restype = c_int
kto.IbanCheck_error.argtypes = c_void_p,
kto.IbanCheck_free.argtypes = c_void_p,
kto.IbanCheck_check_str.restype = c_int
kto.IbanCheck_check_str.argtypes = c_void_p, c_char_p, c_char_p
kto.IbanCheck_check_iban.restype = c_int
kto.IbanCheck_check_iban.argtypes = c_void_p, c_void_p, c_char_p
kto.IbanCheck_resultText.restype = c_char_p
kto.IbanCheck_resultText.argtypes = c_int,
kto.IbanCheck_bic_position.restype = c_int
kto.IbanCheck_bic_position.argtypes = (
    c_void_p, c_char_p, POINTER(c_int), POINTER(c_int))

def test():
    a = AccountNumberCheck()
    print 'Number of Banks:', a.bankCount
    print 'find 20010020:', a.findBank('20010020')
    print 'find 20010033:', a.findBank('20010033')
    print 'check 20070024/9291394:', a.check('20070024','9291394')
    print 'check 20070024/9291394:', a.check('20070024','9291394023')
    print
    ck = IbanCheck()
    if not kto.IbanCheck_selftest(ck):  # not publicly defined
	print "Self-Test failed!"
	raise SystemExit
    s = " iban fr14 2004 1010 0505 0001 3m02 606"
    print "test for iban    :", s
    iban = Iban(s)
    print "transmission form:", iban.transmissionForm()
    res = ck.check(iban)
    print "check result     :", res, "("+IbanCheck.resultText(res)+")"
    print "printable form   :", iban.printableForm()
    print
    print "expect bad checksum:"
    s = "FR1420041010050500013X02606"
    res = ck.check(s)
    print s + ":", res, "("+IbanCheck.resultText(res)+")"
    print
    s = "IBAN DE66 2007 0024 0929 1394 00"
    print s
    s = Iban(s).transmissionForm()
    start, end = ck.bic_position(s)
    print "prefix, checksum, BIC, account:"
    print ', '.join((s[:2], s[2:4], s[start:end], s[end:].lstrip('0')))


if __name__ == '__main__':
    test()
