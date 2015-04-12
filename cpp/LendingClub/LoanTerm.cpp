/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "LoanTerm.hpp"

using namespace lc;

const LCString LoanTerm::csv_name = "term";
const LCString LoanTerm::name = "LoanTerm";
const FilterValueVector* LoanTerm::options = nullptr;
const Filter::Relation LoanTerm::relation = Filter::Relation::MASK;
