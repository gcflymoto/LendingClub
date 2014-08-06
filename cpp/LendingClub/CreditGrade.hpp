/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_CREDIT_GRADE_HPP__
#define __LC_CREDIT_GRADE_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class CreditGrade : public Filter<CreditGrade>
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    CreditGrade(const Arguments& args, unsigned* current = nullptr) : Filter<CreditGrade>(name, args)
    {
        static std::vector<FilterValue> options;

        if (_converation_table.empty()) {
            _converation_table["A"] = 1 << 0;
            _converation_table["B"] = 1 << 1;
            _converation_table["C"] = 1 << 2;
            _converation_table["D"] = 1 << 3;
            _converation_table["E"] = 1 << 4;
            _converation_table["F"] = 1 << 5;
            _converation_table["G"] = 1 << 6;

            _reverse_table[1 << 0] = "A";
            _reverse_table[1 << 1] = "B";
            _reverse_table[1 << 2] = "C";
            _reverse_table[1 << 3] = "D";
            _reverse_table[1 << 4] = "E";
            _reverse_table[1 << 5] = "F";
            _reverse_table[1 << 6] = "G";

            std::string all_grades = args["grades"].as<std::string>();
            size_t num_grades = all_grades.size();
            
            for (size_t i = 0; i < num_grades + 1; ++i) {
                for (size_t j = 0; j < num_grades; ++j) {
                    if ((j + 1) <= num_grades) {
                        std::string grades = all_grades.substr(j, i);
                        FilterValue grades_bit_value = 0;
                        for (auto grade : grades) {
                            grades_bit_value += _converation_table[std::string(1, grade)];
                        }
                        options.push_back(grades_bit_value);
                        _converation_table[grades] = grades_bit_value;
                        _reverse_table[grades_bit_value] = grades;
                    }

                }
            }
        }
        Filter::initialize(&options, current);
    }

    inline FilterValue convert(const std::string& raw_data)
    {
        return _converation_table[raw_data];
    }

    const std::string get_string_value() const
    {
        return _reverse_table[get_value()];
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return ((loan.grade & self.get_value()) > 0);
    }

    inline bool apply(const LCLoan& loan) const
    {
        return ((loan.grade & get_value()) > 0);
    }

private:
    static std::map<std::string, FilterValue>   _converation_table;
    static std::map<FilterValue, std::string>   _reverse_table;
};

};

#endif // __LC_CREDIT_GRADE_HPP__
