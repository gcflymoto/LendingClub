'''
Created on May 30, 2013

@author: gczajkow
'''

import datetime
from Filter import Filter
from LoanEnum import LOAN_ENUM_earliest_credit_line

DEFAULT_EARLIEST_CREDIT_LINE = 0

class EarliestCreditLine(Filter):
    '''
    '''

    datetime = datetime.datetime(2013, 1, 30)
    now = datetime.now()

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = [1.0, 5.0, 10.0]

    def convert(self, raw_data):
        if raw_data:
            return (self.now - self.datetime.strptime(raw_data, "%Y-%m-%d %H:%M")).total_seconds() / 60 / 60 / 24 / 365
        else:
            return DEFAULT_EARLIEST_CREDIT_LINE

    def apply(self, loan, LOAN_ENUM_earliest_credit_line=LOAN_ENUM_earliest_credit_line):
        current = self.getCurrent()
        return loan[LOAN_ENUM_earliest_credit_line] >= current
