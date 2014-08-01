"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter

DEFAULT_INQ_LAST_6_MONTHS = 0


class InquiriesLast6Months(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "inq_last_6mths"
    query = "(inq_last_6mths <= ?)"
    named_query = "(inq_last_6mths <= :inq_last_6mths)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = list(range(11))

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_INQ_LAST_6_MONTHS

    def apply(self, loan, inq_last_6mths=LoanEnum.LOAN_ENUM_inq_last_6mths):
        return loan[inq_last_6mths] <= self.get_current()
