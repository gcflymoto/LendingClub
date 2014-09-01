/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_HOME_OWNERSHIP_HPP__
#define __LC_HOME_OWNERSHIP_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class HomeOwnership : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;

    HomeOwnership() : Filter(name)
    {
        static const FilterValueVector* options = create_range(0, 6, 1);
        Filter::initialize(options);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        if (raw_data == "MORTGAGE") {
            return 0;
        }
        else if (raw_data == "OWN") {
            return 1;
        }
        else if (raw_data == "RENT") {
            return 2;
        }
        else if (raw_data == "OTHER") {
            return 3;
        }
        else if (raw_data == "NONE") {
            return 4;
        }
        else {
            return 5;
        }
    }

    virtual const LCString get_string_value() const
    {
        auto value = get_value();
        switch (value)
        {
        case 0:
            return "MORTGAGE";
        case 1:
            return "OWN";
        case 2:
            return "RENT";
        case 3:
            return "OTHER";
        case 4:
            return "NONE";
        default:
            return "NULL";
        }
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.home_ownership != get_value());
    }
};

};

#endif // __LC_HOME_OWNERSHIP_HPP__
