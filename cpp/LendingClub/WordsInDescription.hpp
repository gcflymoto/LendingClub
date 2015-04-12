/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_WORDS_IN_DESCRIPTION_HPP__
#define __LC_WORDS_IN_DESCRIPTION_HPP__

#include <algorithm>
#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{
template<char Remove> bool BothAreSpaces(char lhs, char rhs) { return (lhs == rhs) && (lhs == Remove); }

class WordsInDescription : public Filter
{
public:
    static const LCString csv_name;
    static const LCString name;
    static const FilterValueVector* options;
    static const Relation relation;

    WordsInDescription() : Filter()
    {
        if (options == nullptr) {
            options = create_range(25, 200, 25);
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
        LCString data = raw_data;
        std::unique(data.begin(), data.end(), BothAreSpaces<' '>);
        std::unique(data.begin(), data.end(), BothAreSpaces<'\t'>);
        return std::count(data.begin(), data.end(), ' ');
    }

    virtual const LCString get_string_value() const
    {
        const FilterValue& val = get_value();
        return ">=" + boost::lexical_cast<LCString>(val);
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.desc_word_count >= get_value());
    }

    virtual const LCString& get_name() const
    {
        return name;
    }
};

};

#endif // __LC_WORDS_IN_DESCRIPTION_HPP__
