/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "EarliestCreditLine.hpp"

using namespace lc;

const LCString EarliestCreditLine::csv_name = "EarliestCreditLine";
const LCString EarliestCreditLine::name = "EarliestCreditLine";
const boost::posix_time::ptime EarliestCreditLine::now = boost::posix_time::second_clock::local_time(); //use the clock 
const FilterValueVector* EarliestCreditLine::options = nullptr;
const Filter::Relation EarliestCreditLine::relation = Filter::Relation::GREATER_THAN_EQUAL;