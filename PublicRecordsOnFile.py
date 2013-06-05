'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_pub_rec

DEFAULT_PUBIC_RECORDS = 0

class PublicRecordsOnFile(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = ["Exclude loans with public records"]

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_PUBIC_RECORDS

    def apply(self, loan, LOAN_ENUM_pub_rec=LOAN_ENUM_pub_rec):
        # Does not mutate
        return not loan[LOAN_ENUM_pub_rec]

