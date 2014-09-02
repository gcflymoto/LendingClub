/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "LoanPurpose.hpp"

using namespace lc;

const LCString LoanPurpose::sqlite_type = "INT";
const LCString LoanPurpose::csv_name = "purpose";
const LCString LoanPurpose::name = "LoanPurpose";
std::map<LCString, FilterValue> LoanPurpose::_conversion_table;
const FilterValueVector* LoanPurpose::options = nullptr;
