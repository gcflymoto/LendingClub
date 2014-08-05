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
                          const Arguments& args,
                          std::vector<Filter*>::iterator& it,
                          unsigned* current = nullptr)
    {
        switch (filter_type) {
        case LCLoan::ACC_OPEN_PAST_24MTHS:
            (*it) = new AccountsOpenPast24Months(args, current);
            break;
        case LCLoan::FUNDED_AMNT:
            (*it) = new AmountRequested(args, current);
            break;
        case LCLoan::ANNUAL_INCOME:
            (*it) = new AnnualIncome(args, current);
            break;
        case LCLoan::GRADE:
            (*it) = new CreditGrade(args, current);
            break;
        case LCLoan::DEBT_TO_INCOME_RATIO:
            (*it) = new DebtToIncomeRatio(args, current);
            break;
        case LCLoan::DELINQ_2YRS:
            (*it) = new Delinquencies(args, current);
            break;
        case LCLoan::EARLIEST_CREDIT_LINE:
            (*it) = new EarliestCreditLine(args, current);
            break;
        case LCLoan::EMP_LENGTH:
            (*it) = new EmploymentLength(args, current);
            break;
        case LCLoan::HOME_OWNERSHIP:
            (*it) = new HomeOwnership(args, current);
            break;
        case LCLoan::INCOME_VALIDATED:
            (*it) = new IncomeValidated(args, current);
            break;
        case LCLoan::INQ_LAST_6MTHS:
            (*it) = new InqueriesLast6Months(args, current);
            break;
        case LCLoan::PURPOSE:
            (*it) = new LoanPurpose(args, current);
            break;
        case LCLoan::MTHS_SINCE_LAST_DELINQ:
            (*it) = new MonthsSinceLastDelinquency(args, current);
            break;
        case LCLoan::PUB_REC:
            (*it) = new PublicRecordsOnFile(args, current);
            break;
        case LCLoan::REVOL_UTILIZATION:
            (*it) = new RevolvingLineUtilization(args, current);
            break;
        case LCLoan::ADDR_STATE:
            (*it) = new State(args, current);
            break;
        case LCLoan::TOTAL_ACC:
            (*it) = new TotalCreditLines(args, current);
            break;
        case LCLoan::DESC_WORD_COUNT:
            (*it) = new WordsInDescription(args, current);
            break;
        default:
            assert(filter_type < LCLoan::SIZE);
        };
    }

};

#endif // __LC_FILTERS_HPP__
