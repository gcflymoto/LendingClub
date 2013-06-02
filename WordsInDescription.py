'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_desc_word_count

class WordsInDescription(Filter):
    '''
      > 75 looks best
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = list(range(25, 250, 50))

    def apply(self, loan, LOAN_ENUM_desc_word_count=LOAN_ENUM_desc_word_count):
        current = self.getCurrent()
        return loan[LOAN_ENUM_desc_word_count] >= current
