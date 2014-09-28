/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "CreditGrade.hpp"

using namespace lc;

const LCString CreditGrade::sqlite_type = "INT";
const LCString CreditGrade::csv_name = "grade";
const LCString CreditGrade::name = "CreditGrade";
std::map<LCString, FilterValue> CreditGrade::_converation_table;
std::map<FilterValue, LCString> CreditGrade::_reverse_table;
const FilterValueVector* CreditGrade::options = nullptr;
const Filter::Relation CreditGrade::relation = Filter::Relation::MASK;