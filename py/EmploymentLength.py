"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter, utilities


class EmploymentLength(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "emp_length"
    query = "(emp_length <= ?)"
    named_query = "(emp_length <= :emp_length)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = [i for i in range(0, 12)]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        raw_data_s = utilities.s(raw_data)
        if raw_data == utilities.u('n/a'):
            return 0
        elif raw_data == utilities.u('< 1 year'):
            return 1
        elif raw_data == utilities.u('10+ years'):
            return 11
        return int(raw_data[0]) + 1

    def __str__(self):
        current = self.get_current()
        if current == 0:
            return '< 1 year'
        elif current == 10:
            return '10+ years'
        elif current == 1:
            return '1 year'
        return '%d years' % current

    def apply(self, loan, emp_length=LoanEnum.LOAN_ENUM_emp_length):
        return loan[emp_length] <= self.get_current()
