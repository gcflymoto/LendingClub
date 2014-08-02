"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter


class AnnualIncome(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "annual_inc"
    query = "(annual_inc <= ?)"
    named_query = "(annual_inc <= :annual_inc)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [i for i in range(0, 300000, 25000)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data[0]) if raw_data else 0

    def apply(self, loan, annual_income=LoanEnum.LOAN_ENUM_annual_income):
        return loan[annual_income] <= self.get_value()
