"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter


class DebtToIncomeRatio(Filter.Filter):
    """
    """
    sqlite_type = "INT"
    name = "dti"
    query = "(dti <= ?)"
    named_query = "(dti <= :dti)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [float(p) for p in range(10*100, 35*100, 5*100)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        # Convert DTI 19.48 into 1948
        raw_value = float(raw_data[:-1]) if raw_data[-1] == '%' else float(raw_data)
        return int(raw_value * 100)

    def __str__(self):
        return '<=' + str(float(self.get_value()) / 100)

    def apply(self, loan, debt_to_income_ratio=LoanEnum.LOAN_ENUM_debt_to_income_ratio):
        return loan[debt_to_income_ratio] <= self.get_value()
