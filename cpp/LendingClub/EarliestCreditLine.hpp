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

class EarliestCreditLine : public Filter<EarliestCreditLine>
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;
    static const int multiplier = 60 * 60 * 24 * 365;
    static const boost::posix_time::ptime now; // = boost::posix_time::second_clock::local_time(); //use the clock 

    EarliestCreditLine(const Arguments& args, unsigned* current = nullptr) : Filter<EarliestCreditLine>(name, args)
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

    inline FilterValue convert(const std::string& raw_data)
    {
        if (raw_data.empty()) {
            return 0;
        }
        else {
            std::stringstream ss(raw_data);
            boost::posix_time::ptime raw_time;
            ss >> raw_time;
            return (now - raw_time).total_seconds();
        }
    }

    const std::string get_string_value() const
    {
        return boost::lexical_cast<std::string>(boost::numeric_cast<double>(get_value()) / multiplier);
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (loan.earliest_credit_line >= self.get_value());
    }

    inline bool apply(const LCLoan& loan) const
    {
        return (loan.earliest_credit_line >= get_value());
    }
};

};

#endif // __LC_EARLIEST_CREDITLINE_HPP__
