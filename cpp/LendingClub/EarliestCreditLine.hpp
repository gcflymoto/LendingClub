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
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;
    static const int multiplier = 60 * 60 * 24 * 365;
    static const boost::posix_time::ptime now; // = boost::posix_time::second_clock::local_time(); //use the clock 

    EarliestCreditLine(unsigned* current = nullptr) : Filter(name)
    {
        static std::vector<FilterValue> options;
        if (options.empty()) {
            options.push_back(1 * multiplier);
            options.push_back(5 * multiplier);
            options.push_back(10 * multiplier);
            options.push_back(20 * multiplier);
            options.push_back(30 * multiplier);
            options.push_back(40 * multiplier);
        }
        Filter::initialize(&options, current);
    }

    virtual FilterValue convert(const std::string& raw_data) const
    {
        if (raw_data.empty()) {
            return 0;
        }
        else {
            boost::posix_time::ptime raw_time(boost::gregorian::from_simple_string(raw_data));
            return (now - raw_time).total_seconds();
        }
    }

    const std::string get_string_value() const
    {
        boost::posix_time::time_duration td = boost::posix_time::seconds(static_cast<long>(get_value()));
        return ">=" + boost::posix_time::to_simple_string(now - td);
    }

    static bool static_apply(const Filter& self, const Loan& loan)
    {
        return (loan.earliest_credit_line >= self.get_value());
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.earliest_credit_line >= get_value());
    }
};

};

#endif // __LC_EARLIEST_CREDITLINE_HPP__
