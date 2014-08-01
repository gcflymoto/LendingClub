"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter

DEFAULT_REVOLING_UTILIZATION = 0.0


class RevolvingLineUtilization(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "revol_util"
    query = "(revol_util <= ?)"
    named_query = "(revol_util <= :revol_util)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [float(n) for n in range(5*100, 105*100, 5*100)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        raw_data = float(raw_data[:-1]) if raw_data else DEFAULT_REVOLING_UTILIZATION
        return int(raw_data * 100)

    def __str__(self):
        return str(float(self.get_current()) / 100)

    def apply(self, loan, revol_utilization=LoanEnum.LOAN_ENUM_revol_utilization):
        return loan[revol_utilization] <= self.get_current()
