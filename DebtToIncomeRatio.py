'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_debt_to_income_ratio

class DebtToIncomeRatio(Filter):
    '''
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = list(range(10, 35, 5))

    def apply(self, loan, LOAN_ENUM_debt_to_income_ratio=LOAN_ENUM_debt_to_income_ratio):
        current = self.getCurrent()
        return loan[LOAN_ENUM_debt_to_income_ratio] <= current
