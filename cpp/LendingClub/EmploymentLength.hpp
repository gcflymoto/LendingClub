/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_EMPLOYMENT_LENGTH_HPP__
#define __LC_EMPLOYMENT_LENGTH_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class EmploymentLength : public Filter
{
public:
    static const LCString csv_name;
    static const LCString name;
    static const FilterValueVector* options;
    static const Relation relation;

    EmploymentLength() : Filter()
    {
        if (options == nullptr) {
            options = create_range(0, 13, 1);
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
        if (raw_data == "n/a") {
            return 0;
        }
        else if (raw_data == "< 1 year") {
            return 1;
        }
        else if (raw_data == "10 years") {
            return 11;
        }
        else if (raw_data == "10+ years") {
            return 12;
        }
        else {
            unsigned result = (raw_data[0] - '0') + 1;
            return result;
        }
    }
    virtual const LCString get_string_value() const
    {
        auto value = static_cast<unsigned>(get_value());
        if (value == 0) {
            return "n/a";
        }
        if (value == 1) {
            return "<1 year";
        }
        else if (value == 2) {
            return "1 year";
        }
        else if (value == 11) {
            return "10 years";
        }
        else if (value == 12) {
            return "10+ years";
        }
        else {
            return boost::lexical_cast<LCString>(value - 1) + " years";
        }
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.emp_length <= get_value());
    }

    virtual const LCString& get_name() const
    {
        return name;
    }
};

};

#endif // __LC_EMPLOYMENT_LENGTH_HPP__
