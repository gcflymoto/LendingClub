/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_PUBLIC_RECORDS_ON_FILE_HPP__
#define __LC_PUBLIC_RECORDS_ON_FILE_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class PublicRecordsOnFile : public Filter<PublicRecordsOnFile>
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    PublicRecordsOnFile(const Arguments& args, unsigned* current = nullptr) : Filter<PublicRecordsOnFile>(name, args)
    {
        static std::vector<FilterValue> options(1);
        Filter::initialize(&options, current);
    }

    inline FilterValue convert(const std::string& raw_data)
    {
        return (raw_data.empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data.c_str());
    }

    const std::string get_string_value() const
    {
        return boost::lexical_cast<std::string>(get_value());
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (!loan.pub_rec);
    }

    inline bool apply(const LCLoan& loan) const
    {
        return (!loan.pub_rec);
    }
};

};

#endif // __LC_PUBLIC_RECORDS_ON_FILE_HPP__
