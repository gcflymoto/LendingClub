/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_MONTHS_SINCE_LAST_DELINQUENCY_HPP__
#define __LC_MONTHS_SINCE_LAST_DELINQUENCY_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class MonthsSinceLastDelinquency : public Filter<MonthsSinceLastDelinquency>
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    MonthsSinceLastDelinquency(const Arguments& args, unsigned* current = nullptr) : Filter<MonthsSinceLastDelinquency>(name, args)
    {
        static std::vector<FilterValue> options;
        if (options.empty()) {
            options.push_back(12);
            options.push_back(24);
            options.push_back(60);
        }
        Filter::initialize(&options, current);
    }

    inline FilterValue convert(const std::string& raw_data)
    {
        return (raw_data.empty()) ? 61 : boost::lexical_cast<FilterValue>(raw_data.c_str());
    }

    const std::string get_string_value() const
    {
        return ">=" + boost::lexical_cast<std::string>(get_value());
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (loan.mths_since_last_delinq >= self.get_value());
    }

    inline bool apply(const LCLoan& loan) const
    {
        return (loan.mths_since_last_delinq >= get_value());
    }
};

};

#endif // __LC_MONTHS_SINCE_LAST_DELINQUENCY_HPP__
