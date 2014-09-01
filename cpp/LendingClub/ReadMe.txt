========================================================================
    CONSOLE APPLICATION : LendingClub Project Overview
========================================================================

AppWizard has created this LendingClub application for you.

This file contains a summary of what you will find in each of the files that
make up your LendingClub application.


LendingClub.vcxproj
    This is the main project file for VC++ projects generated using an Application Wizard.
    It contains information about the version of Visual C++ that generated the file, and
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

LendingClub.vcxproj.filters
    This is the filters file for VC++ projects generated using an Application Wizard. 
    It contains information about the association between the files in your project 
    and the filters. This association is used in the IDE to show grouping of files with
    similar extensions under a specific node (for e.g. ".cpp" files are associated with the
    "Source Files" filter).

LendingClub.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named LendingClub.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////

With current, and get_value() returning FilterValue&
[iteration 573/4096 10.93 sec/iter] Matched 1000/313600 loans (0/mo.) test at 13.24% APY. 32 loans defaulted (3.2%, $18.14 avg loss) 10.81% net APY
No current, and get_value() returning FilterValue
[iteration 573/4096 8.636 sec/iter] Matched 1000/313600 loans (0/mo.) test at 13.24% APY. 32 loans defaulted (3.2%, $18.14 avg loss) 10.81% net APY
No current, and direct _value lookup
[iteration 489/4096 5.795 sec/iter] Matched 1000/313600 loans (0/mo.) test at 13.24% APY. 32 loans defaulted (3.2%, $18.14 avg loss) 10.81% net APY

With PGO
[iteration 50/50 5.948 sec/iter] Matched 1677/318062 loans (0/mo.) test at 12.9% APY. 78 loans defaulted (4.651%, $19.63 avg loss) 9.391% net APY
