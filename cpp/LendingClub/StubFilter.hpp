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
    //static const std::string sqlite_type;
    //static const std::string name;

    StubFilter() : Filter("Stub") 
    {
        static std::vector<FilterValue> options(1);
        Filter::initialize(&options);
    }

    virtual FilterValue convert(const std::string& raw_data) const
    {
        return 0;
    }
    virtual const std::string get_string_value() const
    {
        return "";
    }

    static bool static_apply(const Filter&, const Loan&) 
    {
        return true;
    }
    inline bool apply(const Loan&)  const
    {
        return true;
    }
};

};

#endif // __LC_STUBFILTER_HPP__
