"""
Created on May 30, 2013

@author: gczajkow
"""
import Filter
import LoanEnum

DEFAULT_ACCOUNTS_OPEN_PAST_24_MONTHS = 0


class AccountsOpenLast24Months(Filter.Filter):
    """
    classdocs
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [i for i in range(0, 25,5)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_ACCOUNTS_OPEN_PAST_24_MONTHS

    def apply(self, loan, acc_open_past_24mths=LoanEnum.LOAN_ENUM_acc_open_past_24mths):
        return loan[acc_open_past_24mths] <= self.get_current()
