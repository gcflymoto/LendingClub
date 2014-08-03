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

DEFAULT_EARLIEST_CREDIT_LINE = 0


class EarliestCreditLine(Filter.Filter):
    """
    """
    multiplier = 60 * 60 * 24 * 365
    datetime = datetime.datetime(2013, 1, 30)
    now = datetime.now()
    sqlite_type = "INT"
    name = "earliest_cr_line"
    query = "(earliest_cr_line >= ?)"
    named_query = "(earliest_cr_line >= :earliest_cr_line)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [1 * self.multiplier, 5 * self.multiplier, 10 * self.multiplier, 20 * self.multiplier, 30 * self.multiplier, 40 * self.multiplier]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        if raw_data:
            raw_value = (self.now - self.datetime.strptime(raw_data, "%Y-%m-%d %H:%M")).total_seconds()
            return int(raw_value)
        else:
            return DEFAULT_EARLIEST_CREDIT_LINE

    def __str__(self):
        return str(float(self.get_value()) / self.multiplier)

    def apply(self, loan, earliest_credit_line=LoanEnum.LOAN_ENUM_earliest_credit_line):
        return loan[earliest_credit_line] >= self.get_value()
