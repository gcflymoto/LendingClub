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
        static const std::string sqlite_type;
        static const std::string csv_name;
        static const std::string name;

        Delinquencies(unsigned* current = nullptr) : Filter(name)
        {
            static std::vector<FilterValue> options;
            if (options.empty()) {
                options.push_back(1 << 0);                                      // 0
                options.push_back(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);           // 0 - 3
                options.push_back(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4);  // 0 - 4
                options.push_back(1 << 4);                                      // 4
                options.push_back(1 << 5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 10 | 1 << 11); // 5 - 11
            }

            Filter::initialize(&options, current);
        }

        virtual FilterValue convert(const std::string& raw_data) const
        {
            auto result = (raw_data.empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data.c_str());
            return (result <= 11) ? (1 << result) : (1 << 11);
        }

        virtual const std::string get_string_value() const
        {
            auto value = get_value();
            std::string delinq_list;
            for (FilterValue i = 0; i < 12; ++i) {
                if (((1ull << i) & value) > 0) {
                    delinq_list += boost::lexical_cast<std::string>(i)+',';
                }
            }
            if (delinq_list.empty()) {
                return delinq_list;
            }
            return '"' + delinq_list.substr(0, delinq_list.length() - 1) + '"';
        }

        static bool static_apply(const Filter& self, const LCLoan& loan)
        {
            return ((loan.delinq_2yrs & self.get_value()) > 0);
        }

        inline bool apply(const LCLoan& loan) const
        {
            return ((loan.delinq_2yrs & get_value()) > 0);
        }
    };

    };

    #endif // __LC_DELINQUENCIES_HPP__
