/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_ACCOUNTS_OPEN_PAST_24_MONTHS_HPP__
#define __LC_ACCOUNTS_OPEN_PAST_24_MONTHS_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

static const int DEFAULT_ACCOUNTS_OPEN_PAST_24_MONTHS = 0;

class AccountsOpenLast24Months : public Filter
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    AccountsOpenLast24Months(const Arguments& args, unsigned* current = nullptr) : Filter(name, args)
    {
        static const std::vector<unsigned>* options = create_range(0, 25, 5);
        Filter::initialize(options, current);
    }

    virtual unsigned convert(const std::string& raw_data)
    {
        if (raw_data.empty()) {
            return DEFAULT_ACCOUNTS_OPEN_PAST_24_MONTHS;
        } else {
            return boost::lexical_cast<unsigned>(raw_data);
        }
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (loan.acc_open_past_24mths <= self.get_value());
    }

    inline bool apply(const LCLoan& loan)
    {
        return (loan.acc_open_past_24mths <= get_value());
    }
};

};

#endif // __LC_ACCOUNTS_OPEN_PAST_24_MONTHS_HPP__
