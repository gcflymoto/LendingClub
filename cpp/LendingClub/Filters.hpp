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

    void construct_filter(const LCLoan::LoanType filter_type,
                          std::vector<Filter*>::iterator& it,
                          unsigned* current = nullptr)
    {
        switch (filter_type) {
        case LCLoan::ACC_OPEN_PAST_24MTHS:
            (*it) = new AccountsOpenPast24Months(current);
            break;
        case LCLoan::FUNDED_AMNT:
            (*it) = new AmountRequested(current);
            break;
        case LCLoan::ANNUAL_INCOME:
            (*it) = new AnnualIncome(current);
            break;
        case LCLoan::GRADE:
            (*it) = new CreditGrade(current);
            break;
        case LCLoan::DEBT_TO_INCOME_RATIO:
            (*it) = new DebtToIncomeRatio(current);
            break;
        case LCLoan::DELINQ_2YRS:
            (*it) = new Delinquencies(current);
            break;
        case LCLoan::EARLIEST_CREDIT_LINE:
            (*it) = new EarliestCreditLine(current);
            break;
        case LCLoan::EMP_LENGTH:
            (*it) = new EmploymentLength(current);
            break;
        case LCLoan::HOME_OWNERSHIP:
            (*it) = new HomeOwnership(current);
            break;
        case LCLoan::INCOME_VALIDATED:
            (*it) = new IncomeValidated(current);
            break;
        case LCLoan::INQ_LAST_6MTHS:
            (*it) = new InqueriesLast6Months(current);
            break;
        case LCLoan::PURPOSE:
            (*it) = new LoanPurpose(current);
            break;
        case LCLoan::MTHS_SINCE_LAST_DELINQ:
            (*it) = new MonthsSinceLastDelinquency(current);
            break;
        case LCLoan::PUB_REC:
            (*it) = new PublicRecordsOnFile(current);
            break;
        case LCLoan::REVOL_UTILIZATION:
            (*it) = new RevolvingLineUtilization(current);
            break;
        case LCLoan::ADDR_STATE:
            (*it) = new State(current);
            break;
        case LCLoan::TOTAL_ACC:
            (*it) = new TotalCreditLines(current);
            break;
        case LCLoan::DESC_WORD_COUNT:
            (*it) = new WordsInDescription(current);
            break;
        default:
            assert(filter_type < LCLoan::SIZE);
        };
    }

};

#endif // __LC_FILTERS_HPP__
