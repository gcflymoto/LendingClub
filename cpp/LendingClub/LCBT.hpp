/*
Created on July 27, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LCBT_HPP__
#define __LC_LCBT_HPP__

#include <vector>
#include <string>
#include <boost/any.hpp>
#include "Loan.hpp"
#include "LoanData.hpp"

namespace lc {

class LCBT {
public:
	LCBT(
		const std::vector<LCLoan::LoanType>& conversion_filters,
		const Arguments& args,
		const int worker_idx) :
			_args(args),
			_filters(conversion_filters.size()),
			_worker_idx(worker_idx),
			_loan_data(conversion_filters, args, worker_idx)
	{
		// Create each of the filters and use its conversion utility for normalizing the data
		unsigned i = 0;
		for (auto& filter_type : conversion_filters) {
			_filters[i++] = construct_filter(filter_type, args);
		}
	}
        
    void initialize() {
        _loan_data.initialize();
	}

private:
	const Arguments&						_args;
	std::vector<boost::any>					_filters;
	const int								_worker_idx;
	LCLoanData								_loan_data;
};

};

#endif // __LC_LCBT_HPP__