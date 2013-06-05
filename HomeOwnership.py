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
        self.reverse_table = ["MORTGAGE", "OWN", "RENT", "OTHER", "NONE", "NULL"]
        self.options = [0, 1, 2, 3, 4, 5]

    def convert(self, raw_data, conversion_table={"MORTGAGE":0, "OWN":1, "RENT":2, "OTHER":3, "NONE":4, "":5}):
        return conversion_table[str(raw_data)]

    def __str__(self):
        return self.reverse_table[self.current]

    def apply(self, loan):
        current = self.getCurrent()
        return loan[LOAN_ENUM_home_ownership] != current
