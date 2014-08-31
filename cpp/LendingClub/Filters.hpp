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

typedef std::vector<VariantFilter> FilterVector;

void construct_variant_filter(const Loan::LoanType filter_type,
    FilterVector::iterator& it)
{
    switch (filter_type) {
    case Loan::ACC_OPEN_PAST_24MTHS:
        (*it) = AccountsOpenPast24Months();
        break;
    case Loan::FUNDED_AMNT:
        (*it) = AmountRequested();
        break;
    case Loan::ANNUAL_INCOME:
        (*it) = AnnualIncome();
        break;
    case Loan::GRADE:
        (*it) = CreditGrade();
        break;
    case Loan::DEBT_TO_INCOME_RATIO:
        (*it) = DebtToIncomeRatio();
        break;
    case Loan::DELINQ_2YRS:
        (*it) = Delinquencies();
        break;
    case Loan::EARLIEST_CREDIT_LINE:
        (*it) = EarliestCreditLine();
        break;
    case Loan::EMP_LENGTH:
        (*it) = EmploymentLength();
        break;
    case Loan::HOME_OWNERSHIP:
        (*it) = HomeOwnership();
        break;
    case Loan::INCOME_VALIDATED:
        (*it) = IncomeValidated();
        break;
    case Loan::INQ_LAST_6MTHS:
        (*it) = InqueriesLast6Months();
        break;
    case Loan::PURPOSE:
        (*it) = LoanPurpose();
        break;
    case Loan::MTHS_SINCE_LAST_DELINQ:
        (*it) = MonthsSinceLastDelinquency();
        break;
    case Loan::PUB_REC:
        (*it) = PublicRecordsOnFile();
        break;
    case Loan::REVOL_UTILIZATION:
        (*it) = RevolvingLineUtilization();
        break;
    case Loan::ADDR_STATE:
        (*it) = State();
        break;
    case Loan::TOTAL_ACC:
        (*it) = TotalCreditLines();
        break;
    case Loan::DESC_WORD_COUNT:
        (*it) = WordsInDescription();
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

template<char Remove> bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == Remove); }

// Create a name visitor class so we don't have to attach a name to every 
// instance of a filter
//
class filter_convert_visitor : public boost::static_visitor<FilterValue>
{
private:
    const std::string* raw_data;
public:
    filter_convert_visitor() : raw_data(nullptr) {}
    void set(const std::string& raw_data_a) { raw_data = &raw_data_a; }

    FilterValue operator()(AccountsOpenPast24Months& f) const
    {
        if (raw_data->empty()) {
            return 0;
        }
        else {
            return boost::lexical_cast<FilterValue>(raw_data->c_str());
        }
    }
    FilterValue operator()(AmountRequested& f) const
    {
        return (raw_data->empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data->c_str());
    }
    FilterValue operator()(AnnualIncome& f) const
    {
        return (raw_data->empty()) ? 0 : boost::numeric_cast<FilterValue>(boost::lexical_cast<double>(raw_data->c_str()));
    }
    FilterValue operator()(CreditGrade& f) const
    {
        return f._converation_table[*raw_data];
    }
    FilterValue operator()(DebtToIncomeRatio& f) const
    {
        // Convert DTI 19.48 into normalized 1948
        std::string data;
        if ((*raw_data)[raw_data->length() - 1] == '%') {
            data = raw_data->substr(0, raw_data->length() - 1);
        }
        else {
            data = *raw_data;
        }
        return boost::numeric_cast<FilterValue>(strtod(data.c_str(), nullptr) * 100);
    }
    FilterValue operator()(Delinquencies& f) const
    {
        auto result = (raw_data->empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data->c_str());
        return (result <= 11) ? (1 << result) : (1 << 11);

    }
    FilterValue operator()(EarliestCreditLine& f) const
    {
        if (raw_data->empty()) {
            return 0;
        }
        else {
            boost::posix_time::ptime raw_time(boost::gregorian::from_simple_string(*raw_data));
            return (f.now - raw_time).total_seconds();
        }
    }
    FilterValue operator()(EmploymentLength& f) const
    {
        if (*raw_data == "n/a") {
            return 0;
        }
        else if (*raw_data == "< 1 year") {
            return 1;
        }
        else if (*raw_data == "10 years") {
            return 10;
        }
        else if (*raw_data == "10+ years") {
            return 11;
        }
        else {
            unsigned result = ((*raw_data)[0] - '0') + 1;
            return result;
        }
    }
    FilterValue operator()(HomeOwnership& f) const
    {
        if (*raw_data == "MORTGAGE") {
            return 0;
        }
        else if (*raw_data == "OWN") {
            return 1;
        }
        else if (*raw_data == "RENT") {
            return 2;
        }
        else if (*raw_data == "OTHER") {
            return 3;
        }
        else if (*raw_data == "NONE") {
            return 4;
        }
        else {
            return 5;
        }
    }
    FilterValue operator()(IncomeValidated& f) const
    {
        if (*raw_data == "TRUE") {
            return 1;
        }
        else {
            return 0;
        }
    }
    FilterValue operator()(InqueriesLast6Months& f) const
    {
        return (raw_data->empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data->c_str());
    }
    FilterValue operator()(LoanPurpose& f) const
    {
        auto it = f._conversion_table.find(*raw_data);
        assert(it != f._conversion_table.end());
        return (it->second);

    }
    FilterValue operator()(MonthsSinceLastDelinquency& f) const
    {
        return (raw_data->empty()) ? 61 : boost::lexical_cast<FilterValue>(raw_data->c_str());
    }
    FilterValue operator()(PublicRecordsOnFile& f) const
    {
        return (raw_data->empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data->c_str());
    }
    FilterValue operator()(RevolvingLineUtilization& f) const
    {
        // Convert DTI 19.48 into normalized 1948
        std::string data;
        if (raw_data->empty()) {
            data = "0.0";
        }
        else {
            data = raw_data->substr(0, raw_data->length() - 1);
        }

        return boost::numeric_cast<unsigned>(strtod(data.c_str(), nullptr) * 100);

    }
    FilterValue operator()(State& f) const
    {
        auto it = f._conversion_table.find(*raw_data);
        assert(it != f._conversion_table.end());
        return (it->second);
    }
    FilterValue operator()(TotalCreditLines& f) const
    {
        return (raw_data->empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data->c_str());
    }
    FilterValue operator()(WordsInDescription& f) const
    {
        std::string data = *raw_data;
        std::unique(data.begin(), data.end(), BothAreSpaces<' '>);
        std::unique(data.begin(), data.end(), BothAreSpaces<'\t'>);
        return std::count(data.begin(), data.end(), ' ');

    }
    FilterValue operator()(StubFilter& f) const
    {
        return 0;
    }
};

class filter_stringify_visitor : public boost::static_visitor<const std::string>
{
public:
    const std::string operator()(AccountsOpenPast24Months& f) const
    {
        return "<=" + boost::lexical_cast<std::string>(f.get_value());
    }
    const std::string operator()(AmountRequested& f) const
    {
        return "<=" + boost::lexical_cast<std::string>(f.get_value());
    }
    const std::string operator()(AnnualIncome& f) const
    {
        return ">=" + boost::lexical_cast<std::string>(f.get_value());
    }
    const std::string operator()(CreditGrade& f) const
    {
        return f._reverse_table[f.get_value()];
    }
    const std::string operator()(DebtToIncomeRatio& f) const
    {
        return "<=" + boost::lexical_cast<std::string>(boost::numeric_cast<double>(f.get_value()) / 100);
    }
    const std::string operator()(Delinquencies& f) const
    {
        auto value = f.get_value();
        std::string delinq_list;
        for (FilterValue i = 0; i < 12; ++i) {
            if (((1ull << i) & value) > 0) {
                delinq_list += boost::lexical_cast<std::string>(i)+',';
            }
        }
        if (delinq_list.empty()) {
            return delinq_list;
        }
        return '"' + delinq_list.substr(0, delinq_list.length() - 1) + '"';
    }
    const std::string operator()(EarliestCreditLine& f) const
    {
        boost::posix_time::time_duration td = boost::posix_time::seconds(static_cast<long>(f.get_value()));
        return ">=" + boost::posix_time::to_simple_string(f.now - td);
    }
    const std::string operator()(EmploymentLength& f) const
    {
        auto value = f.get_value();
        if (value == 0) {
            return "n/a";
        }
        if (value == 1) {
            return "<1 year";
        }
        else if (value == 2) {
            return ">1 year";
        }
        else if (value == 10) {
            return ">10 years";
        }
        else if (value == 11) {
            return ">10 years";
        }
        else {
            return '>' + boost::lexical_cast<std::string>(value)+" years";
        }
    }
    const std::string operator()(HomeOwnership& f) const
    {
        auto value = f.get_value();
        switch (value)
        {
        case 0:
            return "MORTGAGE";
        case 1:
            return "OWN";
        case 2:
            return "RENT";
        case 3:
            return "OTHER";
        case 4:
            return "NONE";
        default:
            return "NULL";
        }
    }
    const std::string operator()(IncomeValidated& f) const
    {
        auto value = f.get_value();
        if (value == 0) {
            return "income validated";
        }
        else {
            return "income unvalidated";
        }
    }
    const std::string operator()(InqueriesLast6Months& f) const
    {
        return "<=" + boost::lexical_cast<std::string>(f.get_value());
    }
    const std::string operator()(LoanPurpose& f) const
    {
        auto value = f.get_value();
        std::string result;
        for (auto& it : f._conversion_table) {
            if ((it.second & value) > 0) {
                result += it.first + ',';
            }
        }
        if (result.empty()) {
            return result;
        }
        else {
            return '"' + result.substr(0, result.length() - 1) + '"';
        }
    }
    const std::string operator()(MonthsSinceLastDelinquency& f) const
    {
        return ">=" + boost::lexical_cast<std::string>(f.get_value());
    }
    const std::string operator()(PublicRecordsOnFile& f) const
    {
        return boost::lexical_cast<std::string>(f.get_value());
    }
    const std::string operator()(RevolvingLineUtilization& f) const
    {
        return "<=" + boost::lexical_cast<std::string>(boost::numeric_cast<double>(f.get_value()) / 100);
    }
    const std::string operator()(State& f) const
    {
        auto value = f.get_value();
        std::string result;
        for (auto& it : f._conversion_table) {
            if ((it.second & value) > 0) {
                result += it.first + ',';
            }
        }
        if (result.empty()) {
            return result;
        }
        else {
            return '"' + result.substr(0, result.length() - 1) + '"';
        }
    }
    const std::string operator()(TotalCreditLines& f) const
    {
        return "<=" + boost::lexical_cast<std::string>(f.get_value());
    }
    const std::string operator()(WordsInDescription& f) const
    {
        return ">=" + boost::lexical_cast<std::string>(f.get_value());
    }
    const std::string operator()(StubFilter& f) const
    {
        return std::string();
    }
};

class filter_test_visitor : public boost::static_visitor <bool>
{
private:
    const Loan* loan;
public:
    filter_test_visitor() : loan(nullptr) {}
    void set(const Loan& loan_a) { loan = &loan_a; }
public:
    inline bool operator()(AccountsOpenPast24Months& f) const
    {
        return (loan->acc_open_past_24mths <= f.get_value());
    }
    inline bool operator()(AmountRequested& f) const
    {
        return (loan->funded_amnt <= f.get_value());
    }
    inline bool operator()(AnnualIncome& f) const
    {
        return (loan->annual_income >= f.get_value());
    }
    inline bool operator()(CreditGrade& f) const
    {
        return ((loan->grade & f.get_value()) > 0);
    }
    inline bool operator()(DebtToIncomeRatio& f) const
    {
        return (loan->debt_to_income_ratio <= f.get_value());
    }
    inline bool operator()(Delinquencies& f) const
    {
        return ((loan->delinq_2yrs & f.get_value()) > 0);
    }
    inline bool operator()(EarliestCreditLine& f) const
    {
        return (loan->earliest_credit_line >= f.get_value());
    }
    inline bool operator()(EmploymentLength& f) const
    {
        return (loan->emp_length <= f.get_value());
    }
    inline bool operator()(HomeOwnership& f) const
    {
        return (loan->home_ownership != f.get_value());
    }
    inline bool operator()(IncomeValidated& f) const
    {
        return (loan->income_validated != f.get_value());
    }
    inline bool operator()(InqueriesLast6Months& f) const
    {
        return (loan->inq_last_6mths <= f.get_value());
    }
    inline bool operator()(LoanPurpose& f) const
    {
        return ((loan->purpose & f.get_value()) > 0);
    }
    inline bool operator()(MonthsSinceLastDelinquency& f) const
    {
        return (loan->mths_since_last_delinq >= f.get_value());
    }
    inline bool operator()(PublicRecordsOnFile& f) const
    {
        return (!loan->pub_rec);
    }
    inline bool operator()(RevolvingLineUtilization& f) const
    {
        return (loan->revol_utilization <= f.get_value());
    }
    inline bool operator()(State& f) const
    {
        return ((loan->addr_state & f.get_value()) > 0);
    }
    inline bool operator()(TotalCreditLines& f) const
    {
        return (loan->total_acc <= f.get_value());
    }
    inline bool operator()(WordsInDescription& f) const
    {
        return (loan->desc_word_count >= f.get_value());
    }
    inline bool operator()(StubFilter& f) const
    {
        return true;
    }
};

class filter_initialize_visitor : public boost::static_visitor < void >
{
private:
    const std::vector<FilterValue>& values;
public:
    filter_initialize_visitor(const std::vector<FilterValue>& values) : values(values) {}

    void operator()(AccountsOpenPast24Months& f) const
    {
        f.initialize(&values);
    }
    void operator()(AmountRequested& f) const
    {
        f.initialize(&values);
    }
    void operator()(AnnualIncome& f) const
    {
        f.initialize(&values);
    }
    void operator()(CreditGrade& f) const
    {
        f.initialize(&values);
    }
    void operator()(DebtToIncomeRatio& f) const
    {
        f.initialize(&values);
    }
    void operator()(Delinquencies& f) const
    {
        f.initialize(&values);
    }
    void operator()(EarliestCreditLine& f) const
    {
        f.initialize(&values);
    }
    void operator()(EmploymentLength& f) const
    {
        f.initialize(&values);
    }
    void operator()(HomeOwnership& f) const
    {
        f.initialize(&values);
    }
    void operator()(IncomeValidated& f) const
    {
        f.initialize(&values);
    }
    void operator()(InqueriesLast6Months& f) const
    {
        f.initialize(&values);
    }
    void operator()(LoanPurpose& f) const
    {
        f.initialize(&values);
    }
    void operator()(MonthsSinceLastDelinquency& f) const
    {
        f.initialize(&values);
    }
    void operator()(PublicRecordsOnFile& f) const
    {
        f.initialize(&values);
    }
    void operator()(RevolvingLineUtilization& f) const
    {
        f.initialize(&values);
    }
    void operator()(State& f) const
    {
        f.initialize(&values);
    }
    void operator()(TotalCreditLines& f) const
    {
        f.initialize(&values);
    }
    void operator()(WordsInDescription& f) const
    {
        f.initialize(&values);
    }
    void operator()(StubFilter& f) const
    {
        f.initialize(&values);
    }
};

class filter_count_visitor : public boost::static_visitor<size_t>
{
public:
    size_t operator()(AccountsOpenPast24Months& f) const
    {
        return f.get_count();
    }
    size_t operator()(AmountRequested& f) const
    {
        return f.get_count();
    }
    size_t operator()(AnnualIncome& f) const
    {
        return f.get_count();
    }
    size_t operator()(CreditGrade& f) const
    {
        return f.get_count();
    }
    size_t operator()(DebtToIncomeRatio& f) const
    {
        return f.get_count();
    }
    size_t operator()(Delinquencies& f) const
    {
        return f.get_count();
    }
    size_t operator()(EarliestCreditLine& f) const
    {
        return f.get_count();
    }
    size_t operator()(EmploymentLength& f) const
    {
        return f.get_count();
    }
    size_t operator()(HomeOwnership& f) const
    {
        return f.get_count();
    }
    size_t operator()(IncomeValidated& f) const
    {
        return f.get_count();
    }
    size_t operator()(InqueriesLast6Months& f) const
    {
        return f.get_count();
    }
    size_t operator()(LoanPurpose& f) const
    {
        return f.get_count();
    }
    size_t operator()(MonthsSinceLastDelinquency& f) const
    {
        return f.get_count();
    }
    size_t operator()(PublicRecordsOnFile& f) const
    {
        return f.get_count();
    }
    size_t operator()(RevolvingLineUtilization& f) const
    {
        return f.get_count();
    }
    size_t operator()(State& f) const
    {
        return f.get_count();
    }
    size_t operator()(TotalCreditLines& f) const
    {
        return f.get_count();
    }
    size_t operator()(WordsInDescription& f) const
    {
        return f.get_count();
    }
    size_t operator()(StubFilter& f) const
    {
        return f.get_count();
    }
};

class filter_value_visitor : public boost::static_visitor<FilterValue>
{
public:
    FilterValue operator()(AccountsOpenPast24Months& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(AmountRequested& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(AnnualIncome& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(CreditGrade& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(DebtToIncomeRatio& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(Delinquencies& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(EarliestCreditLine& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(EmploymentLength& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(HomeOwnership& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(IncomeValidated& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(InqueriesLast6Months& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(LoanPurpose& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(MonthsSinceLastDelinquency& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(PublicRecordsOnFile& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(RevolvingLineUtilization& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(State& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(TotalCreditLines& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(WordsInDescription& f) const
    {
        return f.get_value();
    }
    FilterValue operator()(StubFilter& f) const
    {
        return f.get_value();
    }
};

class filter_set_current_visitor : public boost::static_visitor<void>
{
private:
    unsigned current;
public:
    filter_set_current_visitor() : current(0) {}
    void set(unsigned current_a) { current = current_a; }

    void operator()(AccountsOpenPast24Months& f) const
    {
        f.set_current(current);
    }
    void operator()(AmountRequested& f) const
    {
        f.set_current(current);
    }
    void operator()(AnnualIncome& f) const
    {
        f.set_current(current);
    }
    void operator()(CreditGrade& f) const
    {
        f.set_current(current);
    }
    void operator()(DebtToIncomeRatio& f) const
    {
        f.set_current(current);
    }
    void operator()(Delinquencies& f) const
    {
        f.set_current(current);
    }
    void operator()(EarliestCreditLine& f) const
    {
        f.set_current(current);
    }
    void operator()(EmploymentLength& f) const
    {
        f.set_current(current);
    }
    void operator()(HomeOwnership& f) const
    {
        f.set_current(current);
    }
    void operator()(IncomeValidated& f) const
    {
        f.set_current(current);
    }
    void operator()(InqueriesLast6Months& f) const
    {
        f.set_current(current);
    }
    void operator()(LoanPurpose& f) const
    {
        f.set_current(current);
    }
    void operator()(MonthsSinceLastDelinquency& f) const
    {
        f.set_current(current);
    }
    void operator()(PublicRecordsOnFile& f) const
    {
        f.set_current(current);
    }
    void operator()(RevolvingLineUtilization& f) const
    {
        f.set_current(current);
    }
    void operator()(State& f) const
    {
        f.set_current(current);
    }
    void operator()(TotalCreditLines& f) const
    {
        f.set_current(current);
    }
    void operator()(WordsInDescription& f) const
    {
        f.set_current(current);
    }
    void operator()(StubFilter& f) const
    {
        f.set_current(current);
    }
};

class filter_get_current_visitor : public boost::static_visitor<unsigned>
{
public:
    unsigned operator()(const AccountsOpenPast24Months& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const AmountRequested& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const AnnualIncome& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const CreditGrade& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const DebtToIncomeRatio& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const Delinquencies& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const EarliestCreditLine& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const EmploymentLength& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const HomeOwnership& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const IncomeValidated& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const InqueriesLast6Months& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const LoanPurpose& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const MonthsSinceLastDelinquency& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const PublicRecordsOnFile& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const RevolvingLineUtilization& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const State& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const TotalCreditLines& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const WordsInDescription& f) const
    {
        return f.get_current();
    }
    unsigned operator()(const StubFilter& f) const
    {
        return f.get_current();
    }
};

};

#endif // __LC_FILTERS_HPP__
