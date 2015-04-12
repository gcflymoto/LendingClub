/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "TotalCreditLines.hpp"

using namespace lc;

const LCString TotalCreditLines::csv_name = "total_acc";
const LCString TotalCreditLines::name = "TotalCreditLines";
const FilterValueVector* TotalCreditLines::options = nullptr;
const Filter::Relation TotalCreditLines::relation = Filter::Relation::LESS_THAN_EQUAL;
