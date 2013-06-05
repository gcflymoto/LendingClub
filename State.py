'''
Created on May 30, 2013

@author: gczajkow

/**
  * TX and NY are good.
  *
  * States with substantial volume and higher-than-average returns:
  * CA, AZ, FL, GA, IL, MD, MO, NV, 
  */
'''
from Filter import Filter
from LoanEnum import LOAN_ENUM_addr_state

class State(Filter):
    '''
    classdocs
    '''

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        Filter.__init__(self, args, current)
        self.options = self.powerSet(["CA", "AZ", "FL", "GA", "IL", "MD", "MO", "NV"])

        # self.options = self.powerSet(["AK", "AL", "AR", "AZ", "CA", "CO", "CT", "DC", "DE", "FL", "GA",
        # "HI", "IA", "ID", "IL", "IN", "KS", "KY", "LA", "MA", "MD", "ME",
        # "MI", "MN", "MO", "MS", "MT", "NC", "NE", "NH", "NJ", "NM", "NV",
        # "NY", "OH", "OK", "OR", "PA", "RI", "SC", "SD", "TN", "TX", "UT",
        # "VA", "VT", "WA", "WI", "WV", "WY", "NULL"])

    def convert(self, raw_data):
        return raw_data if raw_data else "NULL"

    def apply(self, loan, LOAN_ENUM_addr_state=LOAN_ENUM_addr_state):
        current = self.getCurrent()
        return loan[LOAN_ENUM_addr_state] not in current
