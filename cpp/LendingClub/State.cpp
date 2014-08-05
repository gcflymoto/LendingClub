/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "State.hpp"

using namespace lc;

const std::string State::sqlite_type = "INT";
const std::string State::csv_name = "addr_state";
const std::string State::name = "State";
std::map<std::string, FilterValue> State::_conversion_table;