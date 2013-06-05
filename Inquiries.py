'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_inq_last_6mths

DEFAULT_INQ_LAST_6MTHS = 0

class Inquiries(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        options = list(range(11))

        Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_INQ_LAST_6MTHS

    def apply(self, loan, LOAN_ENUM_inq_last_6mths=LOAN_ENUM_inq_last_6mths):
        current = self.getCurrent()
        return loan[LOAN_ENUM_inq_last_6mths] <= current
