/*
Created on July 27, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/
#ifndef __LC_TYPES_HPP__
#define __LC_TYPES_HPP__

#include <map>

#ifdef FB_FOLLY_STRING
//////////////////////////////////////////////////////////////////////////
// Building glog - DOES NOT WORK
// aclocal -I . -I `pwd`/m4 && autoheader && autoconf && automake
// ./configure 
//
// Building folly on cygwin
// libtoolize --force
// aclocal
// autoheader
// automake --force-missing --add-missing
// autoconf
//////////////////////////////////////////////////////////////////////////

#include <folly/FBString.h>
namespace lc
{
    typedef folly::fbstring LCString;
};
#else

#include <string>
namespace lc
{
    typedef std::string LCString;
};
#endif

#ifdef FB_FOLLY_VECTOR
#include <folly/FBVector.h>
namespace lc
{
    typedef unsigned long long FilterValue;
    typedef folly::fbvector<FilterValue> FilterValueVector;
    typedef folly::fbvector<FilterValueVector> FilterValueVectorVector;

    class Filter;
    typedef folly::fbvector<Filter> FilterVector;
    typedef folly::fbvector<Filter*> FilterPtrVector;

    struct Loan;
    struct LoanInfo;
    typedef folly::fbvector<Loan> LoanVector;
    typedef folly::fbvector<LoanInfo> LoanInfoVector;

    typedef unsigned long long LoanValue;
    typedef folly::fbvector<LoanValue> LoanValueVector;

    typedef folly::fbvector<LCString> StringVector;

    struct LoanReturn;
    typedef folly::fbvector<std::pair<LoanReturn, FilterPtrVector>> PopulationType;
};

#else

#include <vector>
namespace lc
{
    typedef unsigned long long UllValue;
    typedef std::vector<UllValue> UllValueVector;

    typedef unsigned long long FilterValue;
    typedef std::vector<FilterValue> FilterValueVector;
    typedef std::vector<FilterValueVector> FilterValueVectorVector;

    class Filter;
    typedef std::vector<Filter> FilterVector;
    typedef std::vector<Filter*> FilterPtrVector;

    struct Loan;
    struct LoanInfo;
    typedef std::vector<Loan> LoanVector;
    typedef std::vector<LoanInfo> LoanInfoVector;

    typedef unsigned long long LoanValue;
    typedef std::vector<LoanValue> LoanValueVector;

    typedef std::vector<LCString> StringVector;

    struct LoanReturn;
    typedef std::vector<std::pair<LoanReturn, FilterPtrVector>> PopulationType;

    struct LCBT_ThreadData;
    typedef std::vector<LCBT_ThreadData*> ThreadDataVector;
};

#endif

#endif
