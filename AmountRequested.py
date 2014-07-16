"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""

import Filter
import LoanEnum


class AmountRequested(Filter.Filter):
    """
    classdocs
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [float(n) for n in range(5000, 30000, 5000)]

        Filter.Filter.__init__(self, args, options, current)

    def apply(self, loan, funded_amnt=LoanEnum.LOAN_ENUM_funded_amnt):
        return loan[funded_amnt] <= self.get_current()
