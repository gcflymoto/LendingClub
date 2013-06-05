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
        purpose_bitmap = {'other':              1 << 0,
                          'debt_consolidation': 1 << 1,
                          'educational':        1 << 2,
                          'credit_card':        1 << 3,
                          'car':                1 << 4,
                          'home_improvement':   1 << 5,
                          'small_business':     1 << 6,
                          'vacation':           1 << 7,
                          'moving':             1 << 8,
                          'wedding':            1 << 9,
                          'house':              1 << 10,
                          'medical':            1 << 11,
                          'major_purchase':     1 << 12,
                          'renewable_energy':   1 << 13,
                          }

        self.conversion_table = purpose_bitmap.copy()

        options = self.powerBitSet(purpose_bitmap.values())

        Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return self.conversion_table[raw_data]

    def __str__(self):
        l = []
        for key, val in self.conversion_table.items():
            if val & self.current > 0:
                l.append(key)
        return str(l)

    def apply(self, loan, LOAN_ENUM_purpose=LOAN_ENUM_purpose):
        current = self.getCurrent()
        return loan[LOAN_ENUM_purpose] & current > 0
