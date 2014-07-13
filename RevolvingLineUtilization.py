"""
Created on May 30, 2013

@author: gczajkow
"""

import Filter
import LoanEnum

DEFAULT_REVOLING_UTILIZATION = 0.0


class RevolvingLineUtilization(Filter.Filter):
    """
    classdocs
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [float(n) for n in range(5, 105, 5)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return float(raw_data[:-1]) if raw_data else DEFAULT_REVOLING_UTILIZATION

    def apply(self, loan, revol_utilization=LoanEnum.LOAN_ENUM_revol_utilization):
        return loan[revol_utilization] <= self.get_current()
