'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_inq_last_6mths

class Inquiries(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = list(range(11))

    def apply(self, loan, LOAN_ENUM_inq_last_6mths=LOAN_ENUM_inq_last_6mths):
        current = self.getCurrent()
        return loan[LOAN_ENUM_inq_last_6mths] <= current
