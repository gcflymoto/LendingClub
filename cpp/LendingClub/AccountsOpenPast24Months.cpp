/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "AccountsOpenPast24Months.hpp"

using namespace lc;

const LCString AccountsOpenPast24Months::sqlite_type = "INT";
const LCString AccountsOpenPast24Months::csv_name = "acc_open_past_24mths";
const LCString AccountsOpenPast24Months::name = "AccountsOpenPast24Months";
const FilterValueVector* AccountsOpenPast24Months::options = nullptr;
