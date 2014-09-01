/*
Created on July 27, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_FILTER_HPP__
#define __LC_FILTER_HPP__

#include <cassert>
#include <cstdlib>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include "Types.hpp"
#include "Arguments.hpp"
#include "Loan.hpp"

namespace lc
{

class Filter
{
public:
    Filter(const LCString& name) :
        _value(0),
        _options(NULL),
        _name(name),
        _current(0)
    {
    }

    virtual ~Filter() = 0;

    Filter& operator=(const Filter& other)
    {
        if (this != &other) // protect against invalid self-assignment
        {
            _value = other._value;
            _options = other._options;
            //_name = other._name;
            _current = other._current;
        }
        // by convention, always return *this
        return *this;
    }

    void initialize(const FilterValueVector* options)
    {
        _options = options;
        set_current(0);
    }

    virtual bool apply(const Loan& loan) const = 0;
    virtual FilterValue convert(const LCString& raw_data) const = 0;

    inline const FilterValue get_value() const
    {
        return _value;
    }

    inline unsigned get_current() const
    {
        return _current;
    }
   
    inline void set_current(const unsigned current)
    {
        assert(current < _options->size());
        _current = current;
        _value = (*_options)[_current];
    }

    virtual const LCString get_string_value() const = 0;

    size_t get_count()
    {
        return _options->size();
    }

    bool next()
    {
        size_t size = _options->size();
        _current = (_current + 1) % (size + 1);
        if (_current != size) {
            _value = (*_options)[_current];
        }
        else {
            _value = 0;
        }
        return (_current != size);
    }

    FilterValueVectorVector power_set(const FilterValueVector& options)
    {
        FilterValueVectorVector result;
        size_t n = options.size();
        assert(n < ((sizeof(FilterValue) * 8) - 1)); // Power-set must fit within the number of bits in FilterValue
    
        for (FilterValue i = 0, pow2n = 1ull << n; i < pow2n; ++i) {
            FilterValueVector set;
            for (size_t j = 0, end = options.size(); j < end; ++j) {
                if (i & (1ull << j)) {
                    set.push_back(options[j]);
                }
            }
            result.push_back(set);
        }
        return result;
    }

    FilterValueVector power_bitset(const FilterValueVector& options)
    {
        auto l = power_set(options);

        // Now we need to sum up each individual set_tupple
        FilterValueVector result(l.size());

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

    Filter* increment(FilterVector& filters)
    {
        for (auto& filter : filters) {
            if (!filter.next()) {
                continue;
            }
            return &filter;
        }
        return nullptr;
    }

    const LCString& get_name() const
    {
        return _name;
    }	
    
protected:
    FilterValue _value;
    const FilterValueVector* _options;
    unsigned _current;
    const LCString& _name;   // TODO: Get rid of this field from the class and instead put it outside the class
};

inline Filter::~Filter() {}  // defined even though it's pure virtual; it's faster this way; trust me

};

#endif // __LC_FILTER_HPP__
