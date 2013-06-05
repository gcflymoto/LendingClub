'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_delinq_2yrs

DEFAULT_DELINQUENCIES_LAST_2YEARS = 0

class Delinquencies(Filter):
    '''
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = [
                        1 << 0,  # 0
                        1 << 0 | 1 << 1 | 1 << 2 | 1 << 3,  # 0, 1-3
                        1 << 0 | 1 << 1 | 1 << 2 | 1 << 3,  # 0, 1-3, 4
                        1 << 1 | 1 << 2 | 1 << 3,  # 1-3
                        1 << 1 | 1 << 2 | 1 << 3 | 1 << 4,  # 1-3, 4
                        1 << 4,  # 4
                        1 << 5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 10 | 1 << 11,  # 5 - 11
                        ]

    def convert(self, raw_data):
        delinq = int(raw_data) if raw_data else DEFAULT_DELINQUENCIES_LAST_2YEARS

        if delinq <= 11:
            return 1 << delinq
        else:
            return 1 << 11

    def __str__(self):
        current = self.getCurrent()

        delinq_list = []
        for i in range(12):
            if (1 << i) & current > 0:
                delinq_list.append(i)
        return str(tuple(delinq_list))

    def apply(self, loan, LOAN_ENUM_delinq_2yrs=LOAN_ENUM_delinq_2yrs):
        current = self.getCurrent()
        return loan[LOAN_ENUM_delinq_2yrs] & current > 0
