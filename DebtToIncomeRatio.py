"""
Created on May 30, 2013

@author: gczajkow
"""

import Filter
import LoanEnum


class DebtToIncomeRatio(Filter.Filter):
    """
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = list(range(10, 35, 5))

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return float(raw_data[:-1]) if raw_data[-1] == '%' else float(raw_data)

    def apply(self, loan, debt_to_income_ratio=LoanEnum.LOAN_ENUM_debt_to_income_ratio):
        return loan[debt_to_income_ratio] <= self.get_current()
