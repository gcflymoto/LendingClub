/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "OpenAccounts.hpp"

using namespace lc;

const LCString OpenAccounts::sqlite_type = "INT";
const LCString OpenAccounts::csv_name = "OPEN_ACCOUNTS";
const LCString OpenAccounts::name = "OpenAccounts";
const FilterValueVector* OpenAccounts::options = nullptr;
const Filter::Relation OpenAccounts::relation = Filter::Relation::LESS_THAN_EQUAL;