'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_earliest_credit_line

class EarliestCreditLine(Filter):
    '''
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = [1.0, 5.0, 10.0]

    def apply(self, loan, LOAN_ENUM_earliest_credit_line=LOAN_ENUM_earliest_credit_line):
        current = self.getCurrent()
        return loan[LOAN_ENUM_earliest_credit_line] >= current
