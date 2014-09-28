/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "StubFilter.hpp"

using namespace lc;

const LCString StubFilter::name = "State";
const FilterValueVector* StubFilter::options = nullptr;
const Filter::Relation StubFilter::relation = Filter::Relation::LESS_THAN_EQUAL;

