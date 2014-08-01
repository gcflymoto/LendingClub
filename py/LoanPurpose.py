"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum, Filter


class LoanPurpose(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "purpose"
    query = "((purpose & ?) > 0)"
    named_query = "((purpose & :purpose) > 0)"
    def __init__(self, args, current=None):
        """
        Constructor
        """
        purpose_bitmap = dict(other=1 << 0, debt_consolidation=1 << 1, educational=1 << 2, credit_card=1 << 3,
                              car=1 << 4, home_improvement=1 << 5, small_business=1 << 6, vacation=1 << 7,
                              moving=1 << 8, wedding=1 << 9, house=1 << 10, medical=1 << 11, major_purchase=1 << 12,
                              renewable_energy=1 << 13)

        self.conversion_table = purpose_bitmap.copy()

        options = self.power_bitset(purpose_bitmap.values())

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return self.conversion_table[raw_data]

    def __str__(self):
        l = []
        for key, val in self.conversion_table.items():
            if val & self.current > 0:
                l.append(key)
        return str(l)

    def apply(self, loan, purpose=LoanEnum.LOAN_ENUM_purpose):
        return (loan[purpose] & self.get_current()) > 0
