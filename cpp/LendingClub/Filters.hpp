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

namespace lc
{

void construct_filter(const Loan::LoanType filter_type,
                      std::vector<Filter*>::iterator& it,
                      unsigned* current = nullptr)
{
    switch (filter_type) {
    case Loan::ACC_OPEN_PAST_24MTHS:
        (*it) = new AccountsOpenPast24Months(current);
        break;
    case Loan::FUNDED_AMNT:
        (*it) = new AmountRequested(current);
        break;
    case Loan::ANNUAL_INCOME:
        (*it) = new AnnualIncome(current);
        break;
    case Loan::GRADE:
        (*it) = new CreditGrade(current);
        break;
    case Loan::DEBT_TO_INCOME_RATIO:
        (*it) = new DebtToIncomeRatio(current);
        break;
    case Loan::DELINQ_2YRS:
        (*it) = new Delinquencies(current);
        break;
    case Loan::EARLIEST_CREDIT_LINE:
        (*it) = new EarliestCreditLine(current);
        break;
    case Loan::EMP_LENGTH:
        (*it) = new EmploymentLength(current);
        break;
    case Loan::HOME_OWNERSHIP:
        (*it) = new HomeOwnership(current);
        break;
    case Loan::INCOME_VALIDATED:
        (*it) = new IncomeValidated(current);
        break;
    case Loan::INQ_LAST_6MTHS:
        (*it) = new InqueriesLast6Months(current);
        break;
    case Loan::PURPOSE:
        (*it) = new LoanPurpose(current);
        break;
    case Loan::MTHS_SINCE_LAST_DELINQ:
        (*it) = new MonthsSinceLastDelinquency(current);
        break;
    case Loan::PUB_REC:
        (*it) = new PublicRecordsOnFile(current);
        break;
    case Loan::REVOL_UTILIZATION:
        (*it) = new RevolvingLineUtilization(current);
        break;
    case Loan::ADDR_STATE:
        (*it) = new State(current);
        break;
    case Loan::TOTAL_ACC:
        (*it) = new TotalCreditLines(current);
        break;
    case Loan::DESC_WORD_COUNT:
        (*it) = new WordsInDescription(current);
        break;
    default:
        assert(filter_type < Loan::SIZE);
    };
}

};

#endif // __LC_FILTERS_HPP__
