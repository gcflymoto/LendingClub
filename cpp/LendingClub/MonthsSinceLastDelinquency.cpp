/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "MonthsSinceLastDelinquency.hpp"

using namespace lc;

const LCString MonthsSinceLastDelinquency::csv_name = "mths_since_last_delinq";
const LCString MonthsSinceLastDelinquency::name = "MonthsSinceLastDelinquency";
const FilterValueVector* MonthsSinceLastDelinquency::options = nullptr;
const Filter::Relation MonthsSinceLastDelinquency::relation = Filter::Relation::GREATER_THAN_EQUAL;