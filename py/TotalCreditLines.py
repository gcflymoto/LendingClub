"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter

DEFAULT_TOTAL_ACCOUNTS = 0


class TotalCreditLines(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "total_acc"
    query = "(total_acc <= ?)"
    named_query = "(total_acc <= :total_acc)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [10, 20, 30]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_TOTAL_ACCOUNTS

    def apply(self, loan, total_acc=LoanEnum.LOAN_ENUM_total_acc):
        return loan[total_acc] <= self.get_current()
