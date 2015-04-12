"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum
import Filter


class OpenAccounts(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "open_acc"
    query = "(open_acc <= ?)"
    named_query = "(open_acc <= :open_acc)"

    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = list(range(0, 25, 5))

        Filter.Filter.__init__(self, args, options, current)

    def apply(self, loan, open_acc=LoanEnum.LOAN_ENUM_open_acc):
        return loan[open_acc] <= self.get_value()
