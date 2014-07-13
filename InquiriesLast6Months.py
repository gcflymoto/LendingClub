"""
Created on May 30, 2013

@author: gczajkow
"""

import Filter
import LoanEnum

DEFAULT_INQ_LAST_6_MONTHS = 0


class InquiriesLast6Months(Filter.Filter):
    """
    classdocs
    """
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
