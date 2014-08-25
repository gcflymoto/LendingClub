"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter


class Delinquencies(Filter.Filter):
    """
    """
    sqlite_type = "INT"
    name = "delinq_2yrs"
    query = "((delinq_2yrs & ?) > 0)"
    named_query = "((delinq_2yrs & :delinq_2yrs) > 0)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [
            1 << 0,                                      # 0
            1 << 0 | 1 << 1 | 1 << 2 | 1 << 3,           # 0-3
            1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4,  # 0-4
            1 << 1 | 1 << 2 | 1 << 3,                    # 1-3
            1 << 1 | 1 << 2 | 1 << 3 | 1 << 4,           # 1-4
            1 << 4,                                      # 4
            1 << 5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 10 | 1 << 11,  # 5 - 11
        ]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        delinq = int(raw_data) if raw_data else 0

        if delinq <= 11:
            return 1 << delinq
        else:
            return 1 << 11

    def __str__(self):
        value = self.get_value()

        delinq_list = []
        for i in range(12):
            if ((1 << i) & value) > 0:
                delinq_list.append(i)
        return '"' + str(tuple(delinq_list)) + '"'

    def apply(self, loan, delinq_2yrs=LoanEnum.LOAN_ENUM_delinq_2yrs):
        return (loan[delinq_2yrs] & self.get_value()) > 0
