/***************************************************************************
                             -------------------
    cvs         : $Id$
    begin       : Sat Aug 10 2002
    copyright   : (C) 2003 by Christian Stimming
    email       : 

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

#include <config.h>
#include <ktoblzcheck.h>
#include <iostream>
#include <stdio.h>

using namespace std;


int main(int argc, char **argv) {
    unsigned long tries = 5, 
	nr_found = 0, 
	nr_notfound = 0,
	rand_blz;
    char buf[50];
    AccountNumberCheck::Record bankData;
    bool found;
    std::string bankId;
    
    AccountNumberCheck checker;

    if (argc > 1)
	tries = atol(argv[1]);

    for (unsigned long i = 0; i < tries; i++) {
	/*
	rand_blz = (unsigned long)(double(rand())*(1e8/double(RAND_MAX)));
	sprintf(buf, "%8.8lu", rand_blz);
	buf[8]='\0';
	bankId = buf;
	*/
	rand_blz = (unsigned long)(double(rand())*(1e2/double(RAND_MAX)));
	sprintf(buf, "%2.2lu", rand_blz);
	buf[2]='\0';
	bankId = string(buf) + "000000";

	//std::cout << "testing blz " << bankId << std::endl;
	found = true;
	try {
	    bankData = checker.findBank(bankId);
	} catch (int i) {
	    found = false;
	}
	if (found)
	    ++nr_found;
	else
	    ++nr_notfound;
    }

    std::cout << "Tried " << tries << " banks. Found " << nr_found 
	      << ", not found " << nr_notfound << ", i.e. found "
	      << int(100*double(nr_found)/double(tries)) << "%." 
	      << std::endl;
    return 0;
}

