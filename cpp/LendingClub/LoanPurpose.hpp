/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LOAN_PURPOSE_HPP__
#define __LC_LOAN_PURPOSE_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class LoanPurpose : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;

    LoanPurpose() : Filter(name)
    {
        static FilterValueVector options;
        if (options.empty()) {
            _conversion_table["other"] = 1 << 0;
            _conversion_table["debt_consolidation"] = 1 << 1;
            _conversion_table["educational"] = 1 << 2;
            _conversion_table["credit_card"] = 1 << 3;
            _conversion_table["car"] = 1 << 4;
            _conversion_table["home_improvement"] = 1 << 5;
            _conversion_table["small_business"] = 1 << 6;
            _conversion_table["vacation"] = 1 << 7;
            _conversion_table["moving"] = 1 << 8;
            _conversion_table["wedding"] = 1 << 9;
            _conversion_table["house"] = 1 << 10;
            _conversion_table["medical"] = 1 << 11;
            _conversion_table["major_purchase"] = 1 << 12;
            _conversion_table["renewable_energy"] = 1 << 13;

            FilterValueVector purpose_bitmap;

            for (unsigned i = 0; i < 14; ++i) {
                purpose_bitmap.push_back(1 << i);
            }            
            options = power_bitset(purpose_bitmap);
        }
        Filter::initialize(&options);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        auto it = _conversion_table.find(raw_data);
        assert(it != _conversion_table.end());
        return (it->second);
    }

    virtual const LCString get_string_value() const
    {
        auto value = get_value();
        LCString result;
        for (auto& it : _conversion_table) {
            if ((it.second & value) > 0) {
                result += it.first + ',';
            }
        }
        if (result.empty()) {
            return result;
        }
        else {
            return '"' + result.substr(0, result.length() - 1) + '"';
        }
    }

    inline bool apply(const Loan& loan) const
    {
        return ((loan.purpose & get_value()) > 0);
    }

private:
    static std::map<LCString, FilterValue>   _conversion_table;
};

};

#endif // __LC_LOAN_PURPOSE_HPP__
