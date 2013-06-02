'''
Created on May 30, 2013

@author: gczajkow
'''
from Filter import Filter
from LoanEnum import LOAN_ENUM_purpose

class LoanPurpose(Filter):
    '''
    classdocs
    '''
    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, current)
        self.options = self.powerSet(["other", "debt_consolidation", "educational", "credit_card", "car",
                "home_improvement", "small_business", "vacation", "moving", "wedding",
                "house", "medical", "major_purchase", "renewable_energy"])

    def apply(self, loan, LOAN_ENUM_purpose=LOAN_ENUM_purpose):
        current = self.getCurrent()
        return loan[LOAN_ENUM_purpose] in current
