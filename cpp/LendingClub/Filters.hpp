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
#include <boost/variant.hpp>

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

namespace lc
{
typedef boost::variant<
    AccountsOpenPast24Months,
    AmountRequested,
    AnnualIncome,
    CreditGrade,
    DebtToIncomeRatio,
    Delinquencies,
    EarliestCreditLine,
    EmploymentLength,
    HomeOwnership,
    IncomeValidated,
    InqueriesLast6Months,
    LoanPurpose,
    MonthsSinceLastDelinquency,
    PublicRecordsOnFile,
    RevolvingLineUtilization,
    State,
    TotalCreditLines,
    WordsInDescription,
    StubFilter,
    StubFilter
> VariantFilter;

void construct_variant_filter(const Loan::LoanType filter_type,
    std::vector<Filter*>::iterator& it)
{
    switch (filter_type) {
    case Loan::ACC_OPEN_PAST_24MTHS:
        (*it) = new AccountsOpenPast24Months();
        break;
    case Loan::FUNDED_AMNT:
        (*it) = new AmountRequested();
        break;
    case Loan::ANNUAL_INCOME:
        (*it) = new AnnualIncome();
        break;
    case Loan::GRADE:
        (*it) = new CreditGrade();
        break;
    case Loan::DEBT_TO_INCOME_RATIO:
        (*it) = new DebtToIncomeRatio();
        break;
    case Loan::DELINQ_2YRS:
        (*it) = new Delinquencies();
        break;
    case Loan::EARLIEST_CREDIT_LINE:
        (*it) = new EarliestCreditLine();
        break;
    case Loan::EMP_LENGTH:
        (*it) = new EmploymentLength();
        break;
    case Loan::HOME_OWNERSHIP:
        (*it) = new HomeOwnership();
        break;
    case Loan::INCOME_VALIDATED:
        (*it) = new IncomeValidated();
        break;
    case Loan::INQ_LAST_6MTHS:
        (*it) = new InqueriesLast6Months();
        break;
    case Loan::PURPOSE:
        (*it) = new LoanPurpose();
        break;
    case Loan::MTHS_SINCE_LAST_DELINQ:
        (*it) = new MonthsSinceLastDelinquency();
        break;
    case Loan::PUB_REC:
        (*it) = new PublicRecordsOnFile();
        break;
    case Loan::REVOL_UTILIZATION:
        (*it) = new RevolvingLineUtilization();
        break;
    case Loan::ADDR_STATE:
        (*it) = new State();
        break;
    case Loan::TOTAL_ACC:
        (*it) = new TotalCreditLines();
        break;
    case Loan::DESC_WORD_COUNT:
        (*it) = new WordsInDescription();
        break;
    default:
        assert(filter_type < Loan::SIZE);
    };
}

void construct_filter(const Loan::LoanType filter_type,
    std::vector<Filter*>::iterator& it)
{
    switch (filter_type) {
    case Loan::ACC_OPEN_PAST_24MTHS:
        (*it) = new AccountsOpenPast24Months();
        break;
    case Loan::FUNDED_AMNT:
        (*it) = new AmountRequested();
        break;
    case Loan::ANNUAL_INCOME:
        (*it) = new AnnualIncome();
        break;
    case Loan::GRADE:
        (*it) = new CreditGrade();
        break;
    case Loan::DEBT_TO_INCOME_RATIO:
        (*it) = new DebtToIncomeRatio();
        break;
    case Loan::DELINQ_2YRS:
        (*it) = new Delinquencies();
        break;
    case Loan::EARLIEST_CREDIT_LINE:
        (*it) = new EarliestCreditLine();
        break;
    case Loan::EMP_LENGTH:
        (*it) = new EmploymentLength();
        break;
    case Loan::HOME_OWNERSHIP:
        (*it) = new HomeOwnership();
        break;
    case Loan::INCOME_VALIDATED:
        (*it) = new IncomeValidated();
        break;
    case Loan::INQ_LAST_6MTHS:
        (*it) = new InqueriesLast6Months();
        break;
    case Loan::PURPOSE:
        (*it) = new LoanPurpose();
        break;
    case Loan::MTHS_SINCE_LAST_DELINQ:
        (*it) = new MonthsSinceLastDelinquency();
        break;
    case Loan::PUB_REC:
        (*it) = new PublicRecordsOnFile();
        break;
    case Loan::REVOL_UTILIZATION:
        (*it) = new RevolvingLineUtilization();
        break;
    case Loan::ADDR_STATE:
        (*it) = new State();
        break;
    case Loan::TOTAL_ACC:
        (*it) = new TotalCreditLines();
        break;
    case Loan::DESC_WORD_COUNT:
        (*it) = new WordsInDescription();
        break;
    default:
        assert(filter_type < Loan::SIZE);
    };
}

// Create a name visitor class so we don't have to attach a name to every 
// instance of a filter
//
class filter_name_visitor : public boost::static_visitor<const std::string&>
{
public:
    const std::string& operator()(AccountsOpenPast24Months& f) const
    {
        static std::string name = "AccountsOpenPast24Months";
        return name;
    }
    const std::string& operator()(AmountRequested& f) const
    {
        static std::string name = "AmountRequested";
        return name;
    }
    const std::string& operator()(AnnualIncome& f) const
    {
        static std::string name = "AnnualIncome";
        return name;
    }
    const std::string& operator()(CreditGrade& f) const
    {
        static std::string name = "CreditGrade";
        return name;
    }
    const std::string& operator()(DebtToIncomeRatio& f) const
    {
        static std::string name = "DebtToIncomeRatio";
        return name;
    }
    const std::string& operator()(Delinquencies& f) const
    {
        static std::string name = "Delinquencies";
        return name;
    }
    const std::string& operator()(EarliestCreditLine& f) const
    {
        static std::string name = "EarliestCreditLine";
        return name;
    }
    const std::string& operator()(EmploymentLength& f) const
    {
        static std::string name = "EmploymentLength";
        return name;
    }
    const std::string& operator()(HomeOwnership& f) const
    {
        static std::string name = "HomeOwnership";
        return name;
    }
    const std::string& operator()(IncomeValidated& f) const
    {
        static std::string name = "IncomeValidated";
        return name;
    }
    const std::string& operator()(InqueriesLast6Months& f) const
    {
        static std::string name = "InqueriesLast6Months";
        return name;
    }
    const std::string& operator()(LoanPurpose& f) const
    {
        static std::string name = "LoanPurpose";
        return name;
    }
    const std::string& operator()(MonthsSinceLastDelinquency& f) const
    {
        static std::string name = "MonthsSinceLastDelinquency";
        return name;
    }
    const std::string& operator()(PublicRecordsOnFile& f) const
    {
        static std::string name = "PublicRecordsOnFile";
        return name;
    }
    const std::string& operator()(RevolvingLineUtilization& f) const
    {
        static std::string name = "RevolvingLineUtilization";
        return name;
    }
    const std::string& operator()(State& f) const
    {
        static std::string name = "State";
        return name;
    }
    const std::string& operator()(TotalCreditLines& f) const
    {
        static std::string name = "TotalCreditLines";
        return name;
    }
    const std::string& operator()(WordsInDescription& f) const
    {
        static std::string name = "WordsInDescription";
        return name;
    }
    const std::string& operator()(StubFilter& f) const
    {
        static std::string name = "StubFilter";
        return name;
    }
};


};

#endif // __LC_FILTERS_HPP__
