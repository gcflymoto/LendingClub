/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "Delinquencies.hpp"

using namespace lc;

const LCString Delinquencies::sqlite_type = "INT";
const LCString Delinquencies::csv_name = "delinq_2yrs";
const LCString Delinquencies::name = "Delinquencies";
const FilterValueVector* Delinquencies::options = nullptr;
