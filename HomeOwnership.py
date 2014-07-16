"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""

import Filter
import LoanEnum


class HomeOwnership(Filter.Filter):
    """
    People with mortgages suck at repaying LC loans.
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [0, 1, 2, 3, 4, 5]

        Filter.Filter.__init__(self, args, options, current)

        self.reverse_table = ["MORTGAGE", "OWN", "RENT", "OTHER", "NONE", "NULL"]

    def convert(self, raw_data, conversion_table={"MORTGAGE":0, "OWN":1, "RENT":2, "OTHER":3, "NONE":4, "":5}):
        return conversion_table[str(raw_data)]

    def __str__(self):
        return self.reverse_table[self.current]

    def apply(self, loan, home_ownership=LoanEnum.LOAN_ENUM_home_ownership):
        return loan[home_ownership] != self.get_current()
