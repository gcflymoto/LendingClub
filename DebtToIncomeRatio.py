"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""


import Filter
import LoanEnum


class DebtToIncomeRatio(Filter.Filter):
    """
    """
    sqlite_type = "REAL"
    name = "dti"
    query = "(dti <= ?)"
    named_query = "(dti <= :dti)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [float(p) for p in range(10, 35, 5)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return float(raw_data[:-1]) if raw_data[-1] == '%' else float(raw_data)

    def apply(self, loan, debt_to_income_ratio=LoanEnum.LOAN_ENUM_debt_to_income_ratio):
        return loan[debt_to_income_ratio] <= self.get_current()
