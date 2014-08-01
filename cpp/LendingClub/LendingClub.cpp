// LendingClub.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

extern int lcmain(int argc, char* argv[]);

int _tmain(int argc, _TCHAR* argv[])
{
    return lcmain(argc, reinterpret_cast<char**>(argv));
}

