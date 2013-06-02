'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_home_ownership

class HomeOwnership(Filter):
    '''
    People with mortgages suck at repaying LC loans.
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = ["MORTGAGE", "OWN", "RENT", "NULL"]

    def apply(self, loan):
        current = self.getCurrent()
        return loan[LOAN_ENUM_home_ownership] != current
