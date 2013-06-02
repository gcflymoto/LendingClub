'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_revol_utilization

class RevolvingLineUtilization(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = [float(n) for n in range(5, 105, 5)]

    def apply(self, loan, LOAN_ENUM_revol_utilization=LOAN_ENUM_revol_utilization):
        current = self.getCurrent()
        return loan[LOAN_ENUM_revol_utilization] <= current



