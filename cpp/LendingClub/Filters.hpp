/*
Created on July 27, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_FILTERS_HPP__
#define __LC_FILTERS_HPP__

#include <cassert>
#include <map>
#include <boost/any.hpp>

#include "Loan.hpp"
#include "AccountsOpenPast24Months.hpp"
#include "AmountRequested.hpp"

namespace lc {

	boost::any construct_filter(const LCLoan::LoanType filter_type, 
								const Arguments& args,
								unsigned* current = nullptr) 
	{
		boost::any any;
		switch(filter_type) {
		case LCLoan::ACC_OPEN_PAST_24MTHS:
			any = AccountsOpenLast24Months(args, current);
			break;
		case LCLoan::FUNDED_AMNT:
			any = AmountRequested(args, current);
			break;
		default:
			assert(filter_type < LCLoan::SIZE);
		};
		return any;
	}
};

#endif // __LC_FILTERS_HPP__