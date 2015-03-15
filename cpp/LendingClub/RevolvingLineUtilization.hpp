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
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;
    static const FilterValueVector* options;
    static const Relation relation;

    RevolvingLineUtilization() : Filter()
    {
        if (options == nullptr) {
            options = create_range(5 * 100, 100 * 100, 5 * 100);
        }
    }

    virtual Relation get_relation()
    {
        return relation;
    }

    virtual const FilterValueVector& get_options()
    {
        return *options;
    }

    virtual void set_options(const FilterValueVector* new_options)
    {
        assert(new_options != nullptr);
        assert(new_options->empty() == false);
        options = new_options;
        set_current(0);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        // Convert DTI 19.48 into normalized 1948
        LCString data;
        if (raw_data.empty()) {
            data = "0.0";
        } else {
            data = raw_data.substr(0, raw_data.length() - 1);
        }

        return boost::numeric_cast<unsigned>(strtod(data.c_str(), nullptr) * 100);
    }

    virtual const LCString get_string_value() const
    {
        return "<=" + boost::lexical_cast<LCString>(boost::numeric_cast<double>(get_value()) / 100);
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.revol_utilization <= get_value());
    }

    virtual const LCString& get_name() const
    {
        return name;
    }
};

};

#endif // __LC_REVOLVING_LINE_UTILIZATION_HPP__
