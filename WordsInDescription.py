"""
Created on May 30, 2013

@author: gczajkow
"""

import Filter
import LoanEnum
import re


class WordsInDescription(Filter.Filter):
    """
      > 75 looks best
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = list(range(25, 250, 50))

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data, word_re=re.compile(r"[\w']+|[.,!?;%#\$&\-\(\)_]").findall):
        return len([word_maybe for word_maybe in word_re(raw_data) if word_maybe not in ".,!?;%#\$&-()_"])

    def apply(self, loan, desc_word_count=LoanEnum.LOAN_ENUM_desc_word_count):
        return loan[desc_word_count] >= self.get_current()
