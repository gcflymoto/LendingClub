/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "EarliestCreditLine.hpp"

using namespace lc;

const std::string EarliestCreditLine::sqlite_type = "INT";
const std::string EarliestCreditLine::csv_name = "EarliestCreditLine";
const boost::posix_time::ptime EarliestCreditLine::now = boost::posix_time::second_clock::local_time(); //use the clock 