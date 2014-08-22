/*
Created on July 27, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_FILTER_HPP__
#define __LC_FILTER_HPP__

#include <map>
#include <string>
#include <vector>
#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "Arguments.hpp"
#include "Loan.hpp"

namespace lc
{

typedef unsigned long long FilterValue;

class Filter
{
public:
    Filter(const std::string& name) :
        _options(NULL),
        _name(name),
        _current(0)
    {
    }

    virtual ~Filter() = 0;

    void initialize(const std::vector<FilterValue>* options, unsigned* current)
    {
        _options = options;
        if (nullptr == current) {
            _current = options->size();
        } else {
            _current = *current;
            assert(_current < options->size());
        }
    }

    virtual bool apply(const LCLoan& loan) const = 0;
    virtual FilterValue convert(const std::string& raw_data) const = 0;

    inline const FilterValue& get_value() const
    {
        return (*_options)[_current];
    }

    inline unsigned get_current() const
    {
        return _current;
    }
   
    inline void set_current(const unsigned current)
    {
        assert(current < _options->size());
        _current = current;
    }

    virtual const std::string get_string_value() const = 0;

    size_t get_count()
    {
        return _options->size();
    }

    bool next()
    {
        size_t size = _options->size();
        _current = (_current + 1) % (size+1);
        return (_current != size);
    }

    std::vector<std::vector<FilterValue>> power_set(const std::vector<FilterValue>& options)
    {
        std::vector<std::vector<FilterValue>> result;
        size_t n = options.size();
        assert(n < 31);
    
        for(unsigned i = 0, pow2n = 1 << n; i < pow2n; ++i) {
            std::vector<FilterValue> set;
            for(int j = 0, end = options.size(); j < end; ++j) {
                if (i & (1 << j)) {
                    set.push_back(options[j]);
                }
            }
            result.push_back(set);
        }
        return result;
    }

    std::vector<FilterValue> power_bitset(const std::vector<FilterValue>& options)
    {
        auto l = power_set(options);

        // Now we need to sum up each individual set_tupple
        std::vector<FilterValue> result(l.size());

        unsigned idx = 0;
        for (auto set_tupple : l) {
            FilterValue sum = 0;
            for (auto v : set_tupple) {
                sum += v;
            }
            result[idx] = sum;
            ++idx;
        }
        return result;
    }

    Filter* increment(std::vector<Filter>& filters)
    {
        for (auto& filter : filters) {
            if (!filter.next()) {
                continue;
            }
            return &filter;
        }
        return nullptr;
    }

    const std::string get_name() const
    {
        return _name;
    }	
    
    const std::string get_details() const
    {
        return get_name() + "=" + get_string_value();
    }

protected:
    const std::vector<FilterValue>* _options;
    const std::string& _name;
    unsigned _current;
};

inline Filter::~Filter() {}  // defined even though it's pure virtual; it's faster this way; trust me

};

#endif // __LC_FILTER_HPP__
