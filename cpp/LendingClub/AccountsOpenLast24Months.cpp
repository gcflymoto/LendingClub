/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "AccountsOpenPast24Months.hpp"

using namespace lc;

const std::string AccountsOpenLast24Months::sqlite_type = "INT";
const std::string AccountsOpenLast24Months::name = "acc_open_past_24mths";
const std::string AccountsOpenLast24Months::query = "(acc_open_past_24mths <= ?)";
const std::string AccountsOpenLast24Months::named_query = "(acc_open_past_24mths <= :acc_open_past_24mths)";
