/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_ARGUMENTS_HPP__
#define __LC_ARGUMENTS_HPP__

#include <boost/program_options.hpp>

namespace lc
{
typedef boost::program_options::variables_map Arguments;

class LCArguments
{
public:
    static Arguments& Get()
    {
        return _args;
    }

private:
    static Arguments _args;
};

};

#endif // __LC_ARGUMENTS_HPP__

