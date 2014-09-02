/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#include "PublicRecordsOnFile.hpp"

using namespace lc;

const LCString PublicRecordsOnFile::sqlite_type = "INT";
const LCString PublicRecordsOnFile::csv_name = "pub_rec";
const LCString PublicRecordsOnFile::name = "PublicRecordsOnFile";
const FilterValueVector* PublicRecordsOnFile::options = nullptr;
