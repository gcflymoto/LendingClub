/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_EARLIEST_CREDITLINE_HPP__
#define __LC_EARLIEST_CREDITLINE_HPP__

#include <sstream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class EarliestCreditLine : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;
    static const int multiplier = 60 * 60 * 24 * 365;
    static const boost::posix_time::ptime now; // = boost::posix_time::second_clock::local_time(); //use the clock 

    EarliestCreditLine() : Filter(name)
    {
        static FilterValueVector options;
        if (options.empty()) {
            options.push_back(1 * multiplier);
            options.push_back(5 * multiplier);
            options.push_back(10 * multiplier);
            options.push_back(20 * multiplier);
            options.push_back(30 * multiplier);
            options.push_back(40 * multiplier);
        }
        Filter::initialize(&options);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        if (raw_data.empty()) {
            return 0;
        }
        else {
            boost::posix_time::ptime raw_time(boost::gregorian::from_simple_string(raw_data));
            return (now - raw_time).total_seconds();
        }
    }

    virtual const LCString get_string_value() const
    {
        boost::posix_time::time_duration td = boost::posix_time::seconds(static_cast<long>(get_value()));
        return ">=" + boost::posix_time::to_simple_string(now - td);
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.earliest_credit_line >= get_value());
    }
};

};

#endif // __LC_EARLIEST_CREDITLINE_HPP__
