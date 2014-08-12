/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_REVOLVING_LINE_UTILIZATION_HPP__
#define __LC_REVOLVING_LINE_UTILIZATION_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class RevolvingLineUtilization : public Filter
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    RevolvingLineUtilization(const Arguments& args, unsigned* current = nullptr) : Filter(name, args)
    {
        static const std::vector<FilterValue>* options = create_range(5 * 100, 105 * 100, 5 * 100);
        Filter::initialize(options, current);
    }

    virtual FilterValue convert(const std::string& raw_data) const
    {
        // Convert DTI 19.48 into normalized 1948
        std::string data;
        if (raw_data.empty()) {
            data = "0.0";
        } else {
            data = raw_data.substr(0, raw_data.length() - 1);
        }

        return boost::numeric_cast<unsigned>(strtod(data.c_str(), nullptr) * 100);
    }

    virtual const std::string get_string_value() const
    {
        return "<=" + boost::lexical_cast<std::string>(boost::numeric_cast<double>(get_value()) / 100);
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (loan.revol_utilization <= self.get_value());
    }

    inline bool apply(const LCLoan& loan) const
    {
        return (loan.revol_utilization <= get_value());
    }
};

};

#endif // __LC_REVOLVING_LINE_UTILIZATION_HPP__
