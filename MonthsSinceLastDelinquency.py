'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_mths_since_last_delinq

DEFAULT_MTHS_SINCE_LAST_DELINQ = 61

class MonthsSinceLastDelinquency(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, current)
        self.options = [12, 24, 60]

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_MTHS_SINCE_LAST_DELINQ

    def apply(self, loan, LOAN_ENUM_mths_since_last_delinq=LOAN_ENUM_mths_since_last_delinq):
        current = self.getCurrent()
        return loan[LOAN_ENUM_mths_since_last_delinq] >= current

