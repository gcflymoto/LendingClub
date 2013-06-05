'''
Created on May 30, 2013

@author: gczajkow
'''
from Filter import Filter
from LoanEnum import LOAN_ENUM_funded_amnt

class AmountRequested(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        options = [float(n) for n in range(5000, 30000, 5000)]

        Filter.__init__(self, args, options, current)

    def apply(self, loan, LOAN_ENUM_funded_amnt=LOAN_ENUM_funded_amnt):
        current = self.getCurrent()
        return loan[LOAN_ENUM_funded_amnt] <= current
