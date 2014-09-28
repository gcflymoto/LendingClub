/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_STUBFILTER_HPP__
#define __LC_STUBFILTER_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class StubFilter : public Filter
{
public:
    //static const LCString sqlite_type;
    static const LCString name;
    static const FilterValueVector* options;
	static const Relation relation;

    StubFilter() : Filter() 
    {
        if (options == nullptr) {
            auto new_options = new FilterValueVector;
            new_options->push_back(1);
            options = new_options;
        }
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
        return 0;
    }
    virtual const LCString get_string_value() const
    {
        return "";
    }

    inline bool apply(const Loan&)  const
    {
        return true;
    }

    virtual const LCString& get_name() const
    {
        return name;
    }
};

};

#endif // __LC_STUBFILTER_HPP__
