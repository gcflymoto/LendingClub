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

class MonthsSinceLastDelinquency : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;

    MonthsSinceLastDelinquency() : Filter(name)
    {
        static FilterValueVector options;
        if (options.empty()) {
            options.push_back(12);
            options.push_back(24);
            options.push_back(60);
            options.push_back(84);
        }
        Filter::initialize(&options);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        return (raw_data.empty()) ? 61 : boost::lexical_cast<FilterValue>(raw_data.c_str());
    }

    virtual const LCString get_string_value() const
    {
        return ">=" + boost::lexical_cast<LCString>(get_value());
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.mths_since_last_delinq >= get_value());
    }
};

};

#endif // __LC_MONTHS_SINCE_LAST_DELINQUENCY_HPP__
