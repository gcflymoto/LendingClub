/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_STATE_HPP__
#define __LC_STATE_HPP__

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class State : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;
    static const FilterValueVector* options;
    static const Relation relation;

    State() : Filter()
    {
        if (options == nullptr) {               
            _conversion_table["AK"] = 1ull << 0;
            _conversion_table["AL"] = 1ull << 1;
            _conversion_table["AR"] = 1ull << 2;
            _conversion_table["AZ"] = 1ull << 3;
            _conversion_table["CA"] = 1ull << 4;
            _conversion_table["CO"] = 1ull << 5;
            _conversion_table["CT"] = 1ull << 6;
            _conversion_table["DC"] = 1ull << 7;
            _conversion_table["DE"] = 1ull << 8;
            _conversion_table["FL"] = 1ull << 9;
            _conversion_table["GA"] = 1ull << 10;
            _conversion_table["HI"] = 1ull << 11;
            _conversion_table["IA"] = 1ull << 12;
            _conversion_table["ID"] = 1ull << 13;
            _conversion_table["IL"] = 1ull << 14;
            _conversion_table["IN"] = 1ull << 15;
            _conversion_table["KS"] = 1ull << 16;
            _conversion_table["KY"] = 1ull << 17;
            _conversion_table["LA"] = 1ull << 18;
            _conversion_table["MA"] = 1ull << 19;
            _conversion_table["MD"] = 1ull << 20;
            _conversion_table["ME"] = 1ull << 21;
            _conversion_table["MI"] = 1ull << 22;
            _conversion_table["MN"] = 1ull << 23;
            _conversion_table["MO"] = 1ull << 24;
            _conversion_table["MS"] = 1ull << 25;
            _conversion_table["MT"] = 1ull << 26;
            _conversion_table["NC"] = 1ull << 27;
            _conversion_table["NE"] = 1ull << 28;
            _conversion_table["NH"] = 1ull << 29;
            _conversion_table["NJ"] = 1ull << 30;
            _conversion_table["NM"] = 1ull << 31;
            _conversion_table["NO"] = 1ull << 32;
            _conversion_table["NV"] = 1ull << 33;
            _conversion_table["NY"] = 1ull << 34;
            _conversion_table["OH"] = 1ull << 35;
            _conversion_table["OK"] = 1ull << 36;
            _conversion_table["OR"] = 1ull << 37;
            _conversion_table["PA"] = 1ull << 38;
            _conversion_table["RI"] = 1ull << 39;
            _conversion_table["SC"] = 1ull << 40;
            _conversion_table["SD"] = 1ull << 41;
            _conversion_table["TN"] = 1ull << 42;
            _conversion_table["TX"] = 1ull << 43;
            _conversion_table["UT"] = 1ull << 44;
            _conversion_table["VA"] = 1ull << 45;
            _conversion_table["VT"] = 1ull << 46;
            _conversion_table["WA"] = 1ull << 47;
            _conversion_table["WI"] = 1ull << 48;
            _conversion_table["WV"] = 1ull << 49;
            _conversion_table["WY"] = 1ull << 50;
            _conversion_table["NULL"] = 1ull << 51;

            const auto& args = LCArguments::Get();

            LCString test_states = args["states"].as<LCString>();

            std::vector<std::string> states;
            boost::split(states, test_states, boost::is_any_of(","));

            FilterValueVector state_bitmap;
            for (auto& state :states) {
                if (_conversion_table.find(state) == _conversion_table.end()) {
                    std::cerr << "Error: not a valid state: " << state << std::endl;
                    exit(-1);
                }
                else {
                    state_bitmap.push_back(_conversion_table[state]);
                }
            }
            
            options = new FilterValueVector(power_bitset(state_bitmap));
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
        return ((loan.addr_state & get_value()) > 0);
    }

    virtual const LCString& get_name() const
    {
        return name;
    }

private:
    static std::map<LCString, FilterValue>   _conversion_table;
};

};

#endif // __LC_STATE_HPP__
