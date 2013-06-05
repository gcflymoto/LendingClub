'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_desc_word_count
import re

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

    def convert(self, raw_data, word_re=re.compile(r"[\w']+|[.,!?;%#\$&\-\(\)_]").findall):
        return len([word_maybe for word_maybe in word_re(raw_data) if word_maybe not in ".,!?;%#\$&-()_"])

    def apply(self, loan, LOAN_ENUM_desc_word_count=LOAN_ENUM_desc_word_count):
        current = self.getCurrent()
        return loan[LOAN_ENUM_desc_word_count] >= current
