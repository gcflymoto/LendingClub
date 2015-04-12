/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_OPEN_ACCOUNTS_HPP__
#define __LC_OPEN_ACCOUNTS_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class OpenAccounts : public Filter
{
public:
    static const LCString csv_name;
    static const LCString name;
    static const FilterValueVector* options;
    static const Relation relation;

    OpenAccounts() : Filter()
    {
        if (options == nullptr) {
            options = create_range(0, 25, 5);
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
        if (raw_data.empty()) {
            return 0;
        }
        else {
            return boost::lexical_cast<FilterValue>(raw_data.c_str());
        }
    }

    virtual const LCString get_string_value() const
    {
        return "<=" + boost::lexical_cast<LCString>(get_value());
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.open_acc <= get_value());
    }

    virtual const LCString& get_name() const
    {
        return name;
    }
};

};

#endif // __LC_OPEN_ACCOUNTS_HPP__
