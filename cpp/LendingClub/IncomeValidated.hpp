/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_INCOME_VALIDATED_HPP__
#define __LC_INCOME_VALIDATED_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

    class IncomeValidated : public Filter
    {
    public:
        static const std::string sqlite_type;
        static const std::string csv_name;
        static const std::string name;

        IncomeValidated(const Arguments& args, unsigned* current = nullptr) : Filter(name, args)
        {
            static const std::vector<FilterValue>* options = create_range(0, 2, 1);
            Filter::initialize(options, current);
        }

        virtual FilterValue convert(const std::string& raw_data)
        {
            if (raw_data == "TRUE") {
                return 1;
            }
            else {
                return 0;
            }
        }

        static bool static_apply(const Filter& self, const LCLoan& loan)
        {
            return (loan.income_validated != self.get_value());
        }

        inline bool apply(const LCLoan& loan)
        {
            return (loan.income_validated != get_value());
        }

        std::string get_string_value() const
        {
            auto value = get_value();
            if (value == 0) {
                return "income validated";
            }
            else {
                return "income unvalidated";
            }
        }
    };

};

#endif // __LC_INCOME_VALIDATED_HPP__
