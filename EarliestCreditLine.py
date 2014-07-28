"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""

import datetime
import Filter
import LoanEnum

DEFAULT_EARLIEST_CREDIT_LINE = 0.0


class EarliestCreditLine(Filter.Filter):
    """
    """
    datetime = datetime.datetime(2013, 1, 30)
    now = datetime.now()
    sqlite_type = "REAL"
    name = "earliest_cr_line"
    query = "(earliest_cr_line >= ?)"
    named_query = "(earliest_cr_line >= :earliest_cr_line)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [1.0, 5.0, 10.0]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        if raw_data:
            return float((self.now - self.datetime.strptime(raw_data, "%Y-%m-%d %H:%M")).total_seconds() / 60 / 60 / 24 / 365)
        else:
            return DEFAULT_EARLIEST_CREDIT_LINE

    def apply(self, loan, earliest_credit_line=LoanEnum.LOAN_ENUM_earliest_credit_line):
        return loan[earliest_credit_line] >= self.get_current()
