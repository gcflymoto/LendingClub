/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "InqueriesLast6Months.hpp"

using namespace lc;

const LCString InqueriesLast6Months::sqlite_type = "INT";
const LCString InqueriesLast6Months::csv_name = "inq_last_6mths";
const LCString InqueriesLast6Months::name = "InqueriesLast6Months";
const FilterValueVector* InqueriesLast6Months::options = nullptr;
