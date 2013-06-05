'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_revol_utilization

DEFAULT_REVOLING_UTILIZATION = 0.0

class RevolvingLineUtilization(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        options = [float(n) for n in range(5, 105, 5)]

        Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return float(raw_data[:-1]) if raw_data else DEFAULT_REVOLING_UTILIZATION

    def apply(self, loan, LOAN_ENUM_revol_utilization=LOAN_ENUM_revol_utilization):
        current = self.getCurrent()
        return loan[LOAN_ENUM_revol_utilization] <= current



