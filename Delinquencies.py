'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_delinq_2yrs

class Delinquencies(Filter):
    '''
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = [
                        (0,),  # 0
                        (0, 1, 2, 3),  # 0, 1-3
                        (0, 1, 2, 3, 4),  # 0, 1-3, 4
                        (1, 2, 3),  # 1-3
                        (1, 2, 3, 4),  # 1-3, 4
                        (4,),  # 4
                        ]

    def apply(self, loan, LOAN_ENUM_delinq_2yrs=LOAN_ENUM_delinq_2yrs):
        current = self.getCurrent()
        return loan[LOAN_ENUM_delinq_2yrs] in current
