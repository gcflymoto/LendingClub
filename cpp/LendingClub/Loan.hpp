/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LOAN_HPP__
#define __LC_LOAN_HPP__

#include <string>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace lc {

struct LCLoan {
	enum LoanType 
	{
		// These enums are used for conversions and backtesting
		ROWID = 0, 
		ACC_OPEN_PAST_24MTHS = 1,
		FUNDED_AMNT = 2,
		ANNUAL_INCOME = 3,
		GRADE = 4,
		DEBT_TO_INCOME_RATIO = 5,
		DELINQ_2YRS = 6,
		EARLIEST_CREDIT_LINE = 7,
		EMP_LENGTH = 8,
		HOME_OWNERSHIP = 9,
		INCOME_VALIDATED = 10,
		INQ_LAST_6MTHS = 11,
		PURPOSE = 12,
		MTHS_SINCE_LAST_DELINQ = 13,
		PUB_REC = 14,
		REVOL_UTILIZATION = 15,
		ADDR_STATE = 16,
		TOTAL_ACC = 17,
		DESC_WORD_COUNT = 18,
		LOAN_STATUS = 19,
		// These enums are only used for conversions
		ISSUE_DATETIME = 20,
		NUMBER_OF_PAYMENTS = 21,
		INSTALLMENT = 22,
		INT_RATE = 23,
		TOTAL_PYMNT = 24,
		OUT_PRNCP = 25,
		OUT_PRNCP_INV = 26,
		PROFIT = 27,
		PRINCIPAL = 28,
		LOST = 29,
		DEFAULTED = 30,
		SIZE = 31
	};

	// These variables are used for conversions and backtesting
	unsigned rowid;
	unsigned acc_open_past_24mths;
	unsigned funded_amnt;
	unsigned annual_income;
	unsigned grade;
	unsigned debt_to_income_ratio;
	unsigned delinq_2yrs;
	unsigned earliest_credit_line; 
	unsigned emp_length;
	unsigned home_ownership;
	unsigned incomre_validated;
	unsigned inq_last_6mths;
	unsigned purpose;	
	unsigned mths_since_last_delinq;
	unsigned pub_rec;
	unsigned revol_utilization;
	unsigned addr_state;
	unsigned total_acc;
	unsigned desc_word_count;
	// These variables are only used for conversions
	std::string loan_status;
	boost::gregorian::date issue_datetime;
	unsigned number_of_payments;
	double installment;
	double int_rate;
	double total_pymnt;
	double out_prncp;
	double out_prncp_inv;
	double profit;
	double principal;
	double lost;
	unsigned defaulted;	
};

struct LoanReturn {
	size_t							num_loans;
	unsigned						loans_per_month;
	double							expected_apy;
	unsigned						num_defaulted;
	double							pct_defaulted;
	double							avg_default_loss;
	double							net_apy;
};

};

#endif // __LC_LOAN_HPP__
