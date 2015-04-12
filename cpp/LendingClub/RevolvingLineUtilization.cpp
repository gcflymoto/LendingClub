/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "RevolvingLineUtilization.hpp"

using namespace lc;

const LCString RevolvingLineUtilization::csv_name = "revol_util";
const LCString RevolvingLineUtilization::name = "RevolvingLineUtilization";
const FilterValueVector* RevolvingLineUtilization::options = nullptr;
const Filter::Relation RevolvingLineUtilization::relation = Filter::Relation::LESS_THAN_EQUAL;
