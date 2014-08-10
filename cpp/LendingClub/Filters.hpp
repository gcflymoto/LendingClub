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

#include "Loan.hpp"
#include "AccountsOpenPast24Months.hpp"
#include "AmountRequested.hpp"
#include "AnnualIncome.hpp"
#include "CreditGrade.hpp"
#include "DebtToIncomeRatio.hpp"
#include "Delinquencies.hpp"
#include "EarliestCreditLine.hpp"
#include "EmploymentLength.hpp"
#include "HomeOwnership.hpp"
#include "IncomeValidated.hpp"
#include "InqueriesLast6Months.hpp"
#include "LoanPurpose.hpp"
#include "MonthsSinceLastDelinquency.hpp"
#include "PublicRecordsOnFile.hpp"
#include "RevolvingLineUtilization.hpp"
#include "State.hpp"
#include "TotalCreditLines.hpp"
#include "WordsInDescription.hpp"
#include "StubFilter.hpp"

namespace lc {

class Filters {
public:
    Filters(const Arguments& args, const bool randomize = false) :
        acc_open_past_24mths(args),
        funded_amnt(args),
        annual_inc(args),
        grade(args),
        dti(args),
        delinq_2yrs(args),
        earliest_cr_line(args),
        emp_length(args),
        home_ownership(args),
        is_inc_v(args),
        inq_last_6mths(args),
        purpose(args),
        mths_since_last_delinq(args),
        pub_rec(args),
        revol_util(args),
        addr_state(args),
        total_acc(args),
        desc(args)
    {
        if (randomize) {
            acc_open_past_24mths.set_current(randint(0, acc_open_past_24mths.get_count() - 1));
            funded_amnt.set_current(randint(0, funded_amnt.get_count() - 1));
            annual_inc.set_current(randint(0, annual_inc.get_count() - 1));
            grade.set_current(randint(0, grade.get_count() - 1));
            dti.set_current(randint(0, dti.get_count() - 1));
            delinq_2yrs.set_current(randint(0, delinq_2yrs.get_count() - 1));
            earliest_cr_line.set_current(randint(0, earliest_cr_line.get_count() - 1));
            emp_length.set_current(randint(0, emp_length.get_count() - 1));
            home_ownership.set_current(randint(0, home_ownership.get_count() - 1));
            is_inc_v.set_current(randint(0, is_inc_v.get_count() - 1));
            inq_last_6mths.set_current(randint(0, inq_last_6mths.get_count() - 1));
            purpose.set_current(randint(0, purpose.get_count() - 1));
            mths_since_last_delinq.set_current(randint(0, mths_since_last_delinq.get_count() - 1));
            pub_rec.set_current(randint(0, pub_rec.get_count() - 1));
            revol_util.set_current(randint(0, revol_util.get_count() - 1));
            addr_state.set_current(randint(0, addr_state.get_count() - 1));
            total_acc.set_current(randint(0, total_acc.get_count() - 1));
            desc.set_current(randint(0, desc.get_count() - 1));
        }        
    }
    Filters& operator = (const Filters& a) {
        // Only defined to make the compiler happy. Never should be used.
        assert(0);
        return (*this);
    }

    bool apply(LCLoan& loan)
    {
        if (!acc_open_past_24mths.apply(loan)) {
            acc_open_past_24mths.inc_filtered();
            return false;
        }
            
        if (!funded_amnt.apply(loan)) {
            funded_amnt.inc_filtered();
            return false;
        }
            
        if (!annual_inc.apply(loan)) {
            annual_inc.inc_filtered();
            return false;
        }
            
        if (!grade.apply(loan)) {
            grade.inc_filtered();
            return false;
        }
            
        if (!dti.apply(loan)) {
            dti.inc_filtered();
            return false;
        }
            
        if (!delinq_2yrs.apply(loan)) {
            delinq_2yrs.inc_filtered();
            return false;
        }
            
        if (!earliest_cr_line.apply(loan)) {
            earliest_cr_line.inc_filtered();
            return false;
        }
            
        if (!emp_length.apply(loan)) {
            emp_length.inc_filtered();
            return false;
        }

            
        if (!home_ownership.apply(loan)) {
            home_ownership.inc_filtered();
            return false;
        }
            
        if (!is_inc_v.apply(loan)) {
            is_inc_v.inc_filtered();
            return false;
        }
            
        if (!inq_last_6mths.apply(loan)) {
            inq_last_6mths.inc_filtered();
            return false;
        }
            
        if (!purpose.apply(loan)) {
            purpose.inc_filtered();
            return false;
        }
            
        if (!mths_since_last_delinq.apply(loan)) {
            mths_since_last_delinq.inc_filtered();
            return false;
        }
            
        if (!pub_rec.apply(loan)) {
            pub_rec.inc_filtered();
            return false;
        }
            
        if (!revol_util.apply(loan)) {
            revol_util.inc_filtered();
            return false;
        }
            
        if (!addr_state.apply(loan)) {
            addr_state.inc_filtered();
            return false;
        }
            
        if (!total_acc.apply(loan)) {
            total_acc.inc_filtered();
            return false;
        }
            
        if (!desc.apply(loan)) {
            desc.inc_filtered();
            return false;
        }
            
        return true;
    }

    void clear_counters()
    {
        acc_open_past_24mths.reset_filtered();
        funded_amnt.reset_filtered();
        annual_inc.reset_filtered();
        grade.reset_filtered();
        dti.reset_filtered();
        delinq_2yrs.reset_filtered();
        earliest_cr_line.reset_filtered();
        emp_length.reset_filtered();
        home_ownership.reset_filtered();
        is_inc_v.reset_filtered();
        inq_last_6mths.reset_filtered();
        purpose.reset_filtered();
        mths_since_last_delinq.reset_filtered();
        pub_rec.reset_filtered();
        revol_util.reset_filtered();
        addr_state.reset_filtered();
        total_acc.reset_filtered();
        desc.reset_filtered();
    }

    const std::string get_string_value(size_t i) const
    {
        switch (static_cast<LCLoan::LoanType>(i)) {
        case LCLoan::ACC_OPEN_PAST_24MTHS:
            return acc_open_past_24mths.get_string_value();
        case LCLoan::FUNDED_AMNT:
            return funded_amnt.get_string_value();
        case LCLoan::ANNUAL_INCOME:
            return annual_inc.get_string_value();
        case LCLoan::GRADE:
            return grade.get_string_value();
        case LCLoan::DEBT_TO_INCOME_RATIO:
            return dti.get_string_value();
        case LCLoan::DELINQ_2YRS:
            return delinq_2yrs.get_string_value();
        case LCLoan::EARLIEST_CREDIT_LINE:
            return earliest_cr_line.get_string_value();
        case LCLoan::EMP_LENGTH:
            return emp_length.get_string_value();
        case LCLoan::HOME_OWNERSHIP:
            return home_ownership.get_string_value();
        case LCLoan::INCOME_VALIDATED:
            return is_inc_v.get_string_value();
        case LCLoan::INQ_LAST_6MTHS:
            return inq_last_6mths.get_string_value();
        case LCLoan::PURPOSE:
            return purpose.get_string_value();
        case LCLoan::MTHS_SINCE_LAST_DELINQ:
            return mths_since_last_delinq.get_string_value();
        case LCLoan::PUB_REC:
            return pub_rec.get_string_value();
        case LCLoan::REVOL_UTILIZATION:
            return revol_util.get_string_value();
        case LCLoan::ADDR_STATE:
            return addr_state.get_string_value();
        case LCLoan::TOTAL_ACC:
            return total_acc.get_string_value();
        case LCLoan::DESC_WORD_COUNT:
            return desc.get_string_value();
        default:
            assert(0);
        };
        return std::string();
    }

    const std::string get_name(size_t i) const
    {
        switch (static_cast<LCLoan::LoanType>(i)) {
        case LCLoan::ACC_OPEN_PAST_24MTHS:
            return acc_open_past_24mths.get_name();
        case LCLoan::FUNDED_AMNT:
            return funded_amnt.get_name();
        case LCLoan::ANNUAL_INCOME:
            return annual_inc.get_name();
        case LCLoan::GRADE:
            return grade.get_name();
        case LCLoan::DEBT_TO_INCOME_RATIO:
            return dti.get_name();
        case LCLoan::DELINQ_2YRS:
            return delinq_2yrs.get_name();
        case LCLoan::EARLIEST_CREDIT_LINE:
            return earliest_cr_line.get_name();
        case LCLoan::EMP_LENGTH:
            return emp_length.get_name();
        case LCLoan::HOME_OWNERSHIP:
            return home_ownership.get_name();
        case LCLoan::INCOME_VALIDATED:
            return is_inc_v.get_name();
        case LCLoan::INQ_LAST_6MTHS:
            return inq_last_6mths.get_name();
        case LCLoan::PURPOSE:
            return purpose.get_name();
        case LCLoan::MTHS_SINCE_LAST_DELINQ:
            return mths_since_last_delinq.get_name();
        case LCLoan::PUB_REC:
            return pub_rec.get_name();
        case LCLoan::REVOL_UTILIZATION:
            return revol_util.get_name();
        case LCLoan::ADDR_STATE:
            return addr_state.get_name();
        case LCLoan::TOTAL_ACC:
            return total_acc.get_name();
        case LCLoan::DESC_WORD_COUNT:
            return desc.get_name();
        default:
            assert(0);
        };
        return std::string();
    }

    const std::string get_details(size_t i) const
    {
        switch (static_cast<LCLoan::LoanType>(i)) {
        case LCLoan::ACC_OPEN_PAST_24MTHS:
            return acc_open_past_24mths.get_details();
        case LCLoan::FUNDED_AMNT:
            return funded_amnt.get_details();
        case LCLoan::ANNUAL_INCOME:
            return annual_inc.get_details();
        case LCLoan::GRADE:
            return grade.get_details();
        case LCLoan::DEBT_TO_INCOME_RATIO:
            return dti.get_details();
        case LCLoan::DELINQ_2YRS:
            return delinq_2yrs.get_details();
        case LCLoan::EARLIEST_CREDIT_LINE:
            return earliest_cr_line.get_details();
        case LCLoan::EMP_LENGTH:
            return emp_length.get_details();
        case LCLoan::HOME_OWNERSHIP:
            return home_ownership.get_details();
        case LCLoan::INCOME_VALIDATED:
            return is_inc_v.get_details();
        case LCLoan::INQ_LAST_6MTHS:
            return inq_last_6mths.get_details();
        case LCLoan::PURPOSE:
            return purpose.get_details();
        case LCLoan::MTHS_SINCE_LAST_DELINQ:
            return mths_since_last_delinq.get_details();
        case LCLoan::PUB_REC:
            return pub_rec.get_details();
        case LCLoan::REVOL_UTILIZATION:
            return revol_util.get_details();
        case LCLoan::ADDR_STATE:
            return addr_state.get_details();
        case LCLoan::TOTAL_ACC:
            return total_acc.get_details();
        case LCLoan::DESC_WORD_COUNT:
            return desc.get_details();
        default:
            assert(0);
        };
        return std::string();
    }

    unsigned get_current(size_t i) const
    {
        switch (static_cast<LCLoan::LoanType>(i)) {
        case LCLoan::ACC_OPEN_PAST_24MTHS:
            return acc_open_past_24mths.get_current();
        case LCLoan::FUNDED_AMNT:
            return funded_amnt.get_current();
        case LCLoan::ANNUAL_INCOME:
            return annual_inc.get_current();
        case LCLoan::GRADE:
            return grade.get_current();
        case LCLoan::DEBT_TO_INCOME_RATIO:
            return dti.get_current();
        case LCLoan::DELINQ_2YRS:
            return delinq_2yrs.get_current();
        case LCLoan::EARLIEST_CREDIT_LINE:
            return earliest_cr_line.get_current();
        case LCLoan::EMP_LENGTH:
            return emp_length.get_current();
        case LCLoan::HOME_OWNERSHIP:
            return home_ownership.get_current();
        case LCLoan::INCOME_VALIDATED:
            return is_inc_v.get_current();
        case LCLoan::INQ_LAST_6MTHS:
            return inq_last_6mths.get_current();
        case LCLoan::PURPOSE:
            return purpose.get_current();
        case LCLoan::MTHS_SINCE_LAST_DELINQ:
            return mths_since_last_delinq.get_current();
        case LCLoan::PUB_REC:
            return pub_rec.get_current();
        case LCLoan::REVOL_UTILIZATION:
            return revol_util.get_current();
        case LCLoan::ADDR_STATE:
            return addr_state.get_current();
        case LCLoan::TOTAL_ACC:
            return total_acc.get_current();
        case LCLoan::DESC_WORD_COUNT:
            return desc.get_current();
        default:
            assert(0);
        };
        return 0;
    }

    unsigned set_current(size_t i, unsigned current)
    {
        switch (static_cast<LCLoan::LoanType>(i)) {

        case LCLoan::ACC_OPEN_PAST_24MTHS:
            return acc_open_past_24mths.set_current(current);
        case LCLoan::FUNDED_AMNT:
            return funded_amnt.set_current(current);
        case LCLoan::ANNUAL_INCOME:
            return annual_inc.set_current(current);
        case LCLoan::GRADE:
            return grade.set_current(current);
        case LCLoan::DEBT_TO_INCOME_RATIO:
            return dti.set_current(current);
        case LCLoan::DELINQ_2YRS:
            return delinq_2yrs.set_current(current);
        case LCLoan::EARLIEST_CREDIT_LINE:
            return earliest_cr_line.set_current(current);
        case LCLoan::EMP_LENGTH:
            return emp_length.set_current(current);
        case LCLoan::HOME_OWNERSHIP:
            return home_ownership.set_current(current);
        case LCLoan::INCOME_VALIDATED:
            return is_inc_v.set_current(current);
        case LCLoan::INQ_LAST_6MTHS:
            return inq_last_6mths.set_current(current);
        case LCLoan::PURPOSE:
            return purpose.set_current(current);
        case LCLoan::MTHS_SINCE_LAST_DELINQ:
            return mths_since_last_delinq.set_current(current);
        case LCLoan::PUB_REC:
            return pub_rec.set_current(current);
        case LCLoan::REVOL_UTILIZATION:
            return revol_util.set_current(current);
        case LCLoan::ADDR_STATE:
            return addr_state.set_current(current);
        case LCLoan::TOTAL_ACC:
            return total_acc.set_current(current);
        case LCLoan::DESC_WORD_COUNT:
            return desc.set_current(current);
        default:
            assert(0);
        };
        return 0;
    }

    unsigned get_count(size_t i)
    {
        switch (static_cast<LCLoan::LoanType>(i)) {

        case LCLoan::ACC_OPEN_PAST_24MTHS:
            return acc_open_past_24mths.get_count();
        case LCLoan::FUNDED_AMNT:
            return funded_amnt.get_count();
        case LCLoan::ANNUAL_INCOME:
            return annual_inc.get_count();
        case LCLoan::GRADE:
            return grade.get_count();
        case LCLoan::DEBT_TO_INCOME_RATIO:
            return dti.get_count();
        case LCLoan::DELINQ_2YRS:
            return delinq_2yrs.get_count();
        case LCLoan::EARLIEST_CREDIT_LINE:
            return earliest_cr_line.get_count();
        case LCLoan::EMP_LENGTH:
            return emp_length.get_count();
        case LCLoan::HOME_OWNERSHIP:
            return home_ownership.get_count();
        case LCLoan::INCOME_VALIDATED:
            return is_inc_v.get_count();
        case LCLoan::INQ_LAST_6MTHS:
            return inq_last_6mths.get_count();
        case LCLoan::PURPOSE:
            return purpose.get_count();
        case LCLoan::MTHS_SINCE_LAST_DELINQ:
            return mths_since_last_delinq.get_count();
        case LCLoan::PUB_REC:
            return pub_rec.get_count();
        case LCLoan::REVOL_UTILIZATION:
            return revol_util.get_count();
        case LCLoan::ADDR_STATE:
            return addr_state.get_count();
        case LCLoan::TOTAL_ACC:
            return total_acc.get_count();
        case LCLoan::DESC_WORD_COUNT:
            return desc.get_count();
        default:
            assert(0);
        };
        return 0;
    }

    size_t begin() const{
        return static_cast<size_t>(LCLoan::ACC_OPEN_PAST_24MTHS);
    }

    size_t end() const 
    { 
        return static_cast<size_t>(LCLoan::LOAN_STATUS);
    }

    AccountsOpenPast24Months acc_open_past_24mths;
    AmountRequested funded_amnt;
    AnnualIncome annual_inc;
    CreditGrade grade;
    DebtToIncomeRatio dti;
    Delinquencies delinq_2yrs;
    EarliestCreditLine earliest_cr_line;
    EmploymentLength emp_length;
    HomeOwnership home_ownership;
    IncomeValidated is_inc_v;
    InqueriesLast6Months inq_last_6mths;
    LoanPurpose purpose;
    MonthsSinceLastDelinquency mths_since_last_delinq;
    PublicRecordsOnFile pub_rec;
    RevolvingLineUtilization revol_util;
    State addr_state;
    TotalCreditLines total_acc;
    WordsInDescription desc;
};

};

#endif // __LC_FILTERS_HPP__
