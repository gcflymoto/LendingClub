"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter


class AccountsOpenLast24Months(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "acc_open_past_24mths"
    query = "(acc_open_past_24mths <= ?)"
    named_query = "(acc_open_past_24mths <= :acc_open_past_24mths)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [i for i in range(0, 25,5)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data) if raw_data else 0

    def apply(self, loan, acc_open_past_24mths=LoanEnum.LOAN_ENUM_acc_open_past_24mths):
        return loan[acc_open_past_24mths] <= self.get_value()
