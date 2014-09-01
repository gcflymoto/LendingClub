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
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;

    WordsInDescription() : Filter(name)
    {
        static const FilterValueVector* options = create_range(25, 110, 10);
        Filter::initialize(options);
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
};

};

#endif // __LC_WORDS_IN_DESCRIPTION_HPP__
