"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum
import Filter


class IncomeValidated(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "verification_status"
    query = "(verification_status != ?)"
    named_query = "(verification_status != :verification_status)"

    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [0, 1]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        if raw_data == 'Source Verified' or raw_data == 'Verified':
            return 1
        else:
            return 0

    def __str__(self):
        value = self.get_value()
        if value:
            return 'income validated'
        else:
            return 'income unvalidated'

    def apply(self, loan, income_validated=LoanEnum.LOAN_ENUM_income_validated):
        return loan[income_validated] != self.get_value()
