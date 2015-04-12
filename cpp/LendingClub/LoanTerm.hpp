/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LOAN_TERM_HPP__
#define __LC_LOAN_TERM_HPP__

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

    class LoanTerm : public Filter
    {
    public:
        static const LCString csv_name;
        static const LCString name;
        static const FilterValueVector* options;
        static const Relation relation;

        LoanTerm() : Filter()
        {
            if (options == nullptr) {
                const auto& args = LCArguments::Get();

                LCString test_terms = args["terms"].as<LCString>();

                StringVector terms;
                boost::split(terms, test_terms, boost::is_any_of(","));

                FilterValueVector term_options;
                for (auto& term : terms) {
                    if (term == "36") {
                        term_options.push_back(1 << 0);
                    }
                    else if (term == "60") {
                        term_options.push_back(1 << 1);
                    }
                }
                auto term_bitset = power_bitset(term_options);

                // Skip the bitset value of 0, which is nonsensical
                //
                options = new FilterValueVector(term_bitset.begin() + 1, term_bitset.end());
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
            if (raw_data == " 36 months") {
                return 1 << 0;
            }
            if (raw_data == " 60 months") {
                return 1 << 1;
            }
            assert(0);
            return 0;
        }

        virtual const LCString get_string_value() const
        {
            auto value = get_value();
            LCString result;
            if (((1 << 0) & value) > 0) {
                result += "36,";
            }
            if (((1 << 1) & value) > 0) {
                result += "60,";
            }

            return "=" + result.substr(0, result.size() - 1);
        }

        inline bool apply(const Loan& loan) const
        {
            return ((loan.term & get_value()) > 0);
        }

        virtual const LCString& get_name() const
        {
            return name;
        }
    };

};

#endif // __LC_LOAN_TERM_HPP__
