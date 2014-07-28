"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""


import Filter
import LoanEnum

DEFAULT_MTHS_SINCE_LAST_DELINQ = 61

class MonthsSinceLastDelinquency(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "mths_since_last_delinq"
    query = "(mths_since_last_delinq >= ?)"
    named_query = "(mths_since_last_delinq >= :mths_since_last_delinq)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [12, 24, 60]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_MTHS_SINCE_LAST_DELINQ

    def apply(self, loan, mths_since_last_delinq=LoanEnum.LOAN_ENUM_mths_since_last_delinq):
        return loan[mths_since_last_delinq] >= self.get_current()
