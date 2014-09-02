/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "State.hpp"

using namespace lc;

const LCString State::sqlite_type = "INT";
const LCString State::csv_name = "addr_state";
const LCString State::name = "State";
std::map<LCString, FilterValue> State::_conversion_table;
const FilterValueVector* State::options = nullptr;
