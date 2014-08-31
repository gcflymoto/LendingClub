/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "LoanPurpose.hpp"

using namespace lc;

const std::string LoanPurpose::sqlite_type = "INT";
const std::string LoanPurpose::csv_name = "purpose";
std::map<std::string, FilterValue> LoanPurpose::_conversion_table;