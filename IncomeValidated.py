"""
Created on May 30, 2013

@author: gczajkow
"""
import Filter
import LoanEnum


class IncomeValidated(Filter.Filter):
    """
    classdocs
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [0, 1]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        if raw_data == 'TRUE':
            return 1
        else:
            return 0

    def __str__(self):
        current = self.get_current()
        if current:
            return 'income validated'
        else:
            return 'income unvalidated'

    def apply(self, loan, income_validated=LoanEnum.LOAN_ENUM_income_validated):
        return loan[income_validated] != self.get_current()
