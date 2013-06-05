'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_total_acc

DEFAULT_TOTAL_ACCOUNTS = 0

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

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_TOTAL_ACCOUNTS

    def apply(self, loan, LOAN_ENUM_total_acc=LOAN_ENUM_total_acc):
        current = self.getCurrent()
        return loan[LOAN_ENUM_total_acc] <= current
