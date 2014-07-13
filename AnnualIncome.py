"""
Created on May 30, 2013

@author: gczajkow
"""
import Filter
import LoanEnum


class AnnualIncome(Filter.Filter):
    """
    classdocs
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [i for i in range(0, 300000, 25000)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data[0]) if raw_data else 0

    def apply(self, loan, annual_income=LoanEnum.LOAN_ENUM_annual_income):
        return loan[annual_income] <= self.get_current()
