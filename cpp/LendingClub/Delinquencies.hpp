/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_DELINQUENCIES_HPP__
#define __LC_DELINQUENCIES_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class Delinquencies : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;
    static const FilterValueVector* options;
	static const Relation relation;

    Delinquencies() : Filter()
    {
        if (options == nullptr) {
            auto new_options = new FilterValueVector;
            new_options->push_back(1 << 0);                                      // 0
            new_options->push_back(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);           // 0 - 3
            new_options->push_back(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4);  // 0 - 4
            new_options->push_back(1 << 4);                                      // 4
            new_options->push_back(1 << 5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 10 | 1 << 11); // 5 - 11
            options = new_options;
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
        auto result = (raw_data.empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data.c_str());
        return (result <= 11) ? (1 << result) : (1 << 11);
    }

    virtual const LCString get_string_value() const
    {
        auto value = get_value();
        LCString delinq_list;
        for (FilterValue i = 0; i < 12; ++i) {
            if (((1ull << i) & value) > 0) {
                delinq_list += boost::lexical_cast<LCString>(i)+',';
            }
        }
        if (delinq_list.empty()) {
            return delinq_list;
        }
        return '"' + delinq_list.substr(0, delinq_list.length() - 1) + '"';
    }

    inline bool apply(const Loan& loan) const
    {
        return ((loan.delinq_2yrs & get_value()) > 0);
    }

    virtual const LCString& get_name() const
    {
        return name;
    }
};

};

#endif // __LC_DELINQUENCIES_HPP__
