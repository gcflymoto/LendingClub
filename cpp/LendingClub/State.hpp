/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_STATE_HPP__
#define __LC_STATE_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class State : public Filter
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    State(unsigned* current = nullptr) : Filter(name)
    {
        static std::vector<FilterValue> options;
        if (options.empty()) {               
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
            _conversion_table["NV"] = 1ull << 32;
            _conversion_table["NY"] = 1ull << 33;
            _conversion_table["OH"] = 1ull << 34;
            _conversion_table["OK"] = 1ull << 35;
            _conversion_table["OR"] = 1ull << 36;
            _conversion_table["PA"] = 1ull << 37;
            _conversion_table["RI"] = 1ull << 38;
            _conversion_table["SC"] = 1ull << 39;
            _conversion_table["SD"] = 1ull << 40;
            _conversion_table["TN"] = 1ull << 41;
            _conversion_table["TX"] = 1ull << 42;
            _conversion_table["UT"] = 1ull << 43;
            _conversion_table["VA"] = 1ull << 44;
            _conversion_table["VT"] = 1ull << 45;
            _conversion_table["WA"] = 1ull << 46;
            _conversion_table["WI"] = 1ull << 47;
            _conversion_table["WV"] = 1ull << 48;
            _conversion_table["WY"] = 1ull << 49;
            _conversion_table["NULL"] = 1ull << 50;

            std::vector<FilterValue> state_bitmap;
            state_bitmap.push_back(_conversion_table["CA"]);
            state_bitmap.push_back(_conversion_table["AZ"]);
            state_bitmap.push_back(_conversion_table["FL"]);
            state_bitmap.push_back(_conversion_table["GA"]);
            state_bitmap.push_back(_conversion_table["IL"]);
            state_bitmap.push_back(_conversion_table["MD"]);
            state_bitmap.push_back(_conversion_table["NO"]);
            state_bitmap.push_back(_conversion_table["NV"]);
            state_bitmap.push_back(_conversion_table["TX"]);
            state_bitmap.push_back(_conversion_table["NY"]);                
            options = power_bitset(state_bitmap);
        }
        Filter::initialize(&options, current);
    }

    virtual FilterValue convert(const std::string& raw_data) const
    {
        auto it = _conversion_table.find(raw_data);
        assert(it != _conversion_table.end());
        return (it->second);
    }

    virtual const std::string get_string_value() const
    {
        auto value = get_value();
        std::string result;
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
    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return ((loan.addr_state & self.get_value()) > 0);
    }

    inline bool apply(const LCLoan& loan) const
    {
        return ((loan.addr_state & get_value()) > 0);
    }

private:
    static std::map<std::string, FilterValue>   _conversion_table;
};

};

#endif // __LC_STATE_HPP__
