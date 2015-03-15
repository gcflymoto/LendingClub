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
#include "Arguments.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class CreditGrade : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;
    static const FilterValueVector* options;
    static const Relation relation;

    CreditGrade() : Filter()
    {
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

            const auto& args = LCArguments::Get();

            LCString test_grades = args["grades"].as<LCString>();
            size_t num_grades = test_grades.size();
            
            for (size_t i = 0; i < num_grades + 1; ++i) {
                for (size_t j = 0; j < num_grades; ++j) {
                    if ((j + i) <= num_grades) {
                        LCString grades = test_grades.substr(j, i+1);
                        FilterValue grades_bit_value = 0;
                        for (auto grade : grades) {
                            grades_bit_value += _converation_table[LCString(1, grade)];
                        }
                        _converation_table[grades] = grades_bit_value;
                        _reverse_table[grades_bit_value] = grades;
                    }
                }
            }
        }

        if (options == nullptr) {
            auto new_options = new FilterValueVector;
            for (auto it : _converation_table) {
                new_options->push_back(it.second);
            }
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
        return _converation_table[raw_data];
    }

    virtual const LCString get_string_value() const
    {
        return _reverse_table[get_value()];
    }

    inline bool apply(const Loan& loan) const
    {
        return ((loan.grade & get_value()) > 0);
    }

    virtual const LCString& get_name() const
    {
        return name;
    }

private:
    static std::map<LCString, FilterValue>   _converation_table;
    static std::map<FilterValue, LCString>   _reverse_table;
};

};

#endif // __LC_CREDIT_GRADE_HPP__
