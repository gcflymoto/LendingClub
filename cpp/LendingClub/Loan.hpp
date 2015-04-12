/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LOAN_HPP__
#define __LC_LOAN_HPP__

#include <boost/date_time/gregorian/gregorian.hpp>
#include "Types.hpp"

namespace lc
{

struct Loan
{
    enum LoanType 
    {
        // These ENUMS are used for conversions and back-testing
        ROWID = 0, 
        OPEN_ACCOUNTS = 1,
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
        TERM = 19,
        // These ENUMS are only used for figuring profit
        LOAN_STATUS = 20,
        ISSUE_DATETIME = 21,
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

    // These variables are used for conversions and back-testing
    LoanValue                       rowid;
    LoanValue                       open_acc;
    LoanValue                       funded_amnt;
    LoanValue                       annual_income;
    LoanValue                       grade;
    LoanValue                       debt_to_income_ratio;
    LoanValue                       delinq_2yrs;
    LoanValue                       earliest_credit_line;
    LoanValue                       emp_length;
    LoanValue                       home_ownership;
    LoanValue                       income_validated;
    LoanValue                       inq_last_6mths;
    LoanValue                       purpose;
    LoanValue                       mths_since_last_delinq;
    LoanValue                       pub_rec;
    LoanValue                       revol_utilization;
    LoanValue                       addr_state;
    LoanValue                       total_acc;
    LoanValue                       desc_word_count;
    LoanValue                       term;
};

typedef std::vector<Loan::LoanType> LoanTypeVector;

struct LoanInfo
{
    // These variables are only used for conversions
    LCString                        loan_status;
    boost::gregorian::date          issue_datetime;
    double                          installment;
    double                          int_rate;
    double                          total_pymnt;
    double                          out_prncp;
    double                          out_prncp_inv;
    double                          profit;
    double                          principal;
    double                          lost;
    unsigned                        defaulted;
};

struct LoanReturn
{
    size_t                          num_loans;
    size_t                          loans_per_month;
    double                          expected_apy;
    size_t                          num_defaulted;
    double                          pct_defaulted;
    double                          avg_default_loss;
    double                          net_apy;
};

};

#endif // __LC_LOAN_HPP__
