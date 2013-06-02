'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_total_acc

class TotalCreditLines(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = [10, 20, 30]

    def apply(self, loan, LOAN_ENUM_total_acc=LOAN_ENUM_total_acc):
        current = self.getCurrent()
        return loan[LOAN_ENUM_total_acc] <= current
