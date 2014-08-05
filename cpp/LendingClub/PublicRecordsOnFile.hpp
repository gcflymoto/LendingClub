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

    class PublicRecordsOnFile : public Filter
    {
    public:
        static const std::string sqlite_type;
        static const std::string csv_name;
        static const std::string name;

        PublicRecordsOnFile(const Arguments& args, unsigned* current = nullptr) : Filter(name, args)
        {
            static std::vector<FilterValue> options(1);
            Filter::initialize(&options, current);
        }

        static bool static_apply(const Filter& self, const LCLoan& loan)
        {
            return (!loan.pub_rec);
        }

        inline bool apply(const LCLoan& loan)
        {
            return (!loan.pub_rec);
        }
    };

};

#endif // __LC_PUBLIC_RECORDS_ON_FILE_HPP__
