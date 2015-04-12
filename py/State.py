"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com

/**
  * TX and NY are good.
  *
  * States with substantial volume and higher-than-average returns:
  * CA, AZ, FL, GA, IL, MD, MO, NV,
  */
"""
import LoanEnum
import Filter


class State(Filter.Filter):
    """
    class doc
    """
    sqlite_type = "INT"
    name = "addr_state"
    query = "((addr_state & ?) > 0)"
    named_query = "((addr_state & :addr_state) > 0)"

    def __init__(self, args, current=None):
        """
        Constructor
        """
        state_bitmap = dict(AK=1 << 0, AL=1 << 1, AR=1 << 2, AZ=1 << 3, CA=1 << 4, CO=1 << 5, CT=1 << 6,
                            DC=1 << 7, DE=1 << 8, FL=1 << 9, GA=1 << 10, HI=1 << 11, IA=1 << 12, ID=1 << 13,
                            IL=1 << 14, IN=1 << 15, KS=1 << 16, KY=1 << 17, LA=1 << 18, MA=1 << 19, MD=1 << 20,
                            ME=1 << 21, MI=1 << 22, MN=1 << 23, MO=1 << 24, MS=1 << 25, MT=1 << 26, NC=1 << 27,
                            NE=1 << 28, NH=1 << 29, NJ=1 << 30, NM=1 << 31, NV=1 << 32, NY=1 << 33, OH=1 << 34,
                            OK=1 << 35, OR=1 << 36, PA=1 << 37, RI=1 << 38, SC=1 << 39, SD=1 << 40, TN=1 << 41,
                            TX=1 << 42, UT=1 << 43, VA=1 << 44, VT=1 << 45, WA=1 << 46, WI=1 << 47, WV=1 << 48,
                            WY=1 << 49, NULL=1 << 50)

        self.conversion_table = state_bitmap

        options = self.power_bitset(
            [state_bitmap[state] for state in args.states.split(",")])

        # This is too costly to compute
        # self.options = self.powerSet(["AK", "AL", "AR", "AZ", "CA", "CO", "CT", "DC", "DE", "FL", "GA",
        # "HI", "IA", "ID", "IL", "IN", "KS", "KY", "LA", "MA", "MD", "ME",
        # "MI", "MN", "MO", "MS", "MT", "NC", "NE", "NH", "NJ", "NM", "NV",
        # "NY", "OH", "OK", "OR", "PA", "RI", "SC", "SD", "TN", "TX", "UT",
        # "VA", "VT", "WA", "WI", "WV", "WY", "NULL"])

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return self.conversion_table[raw_data] if raw_data else self.conversion_table["NULL"]

    def __str__(self):
        value = self.get_value()
        l = []
        for key, val in self.conversion_table.items():
            if val & value > 0:
                l.append(key)
        return '"' + ','.join(l) + '"'

    def apply(self, loan, addr_state=LoanEnum.LOAN_ENUM_addr_state):
        return (loan[addr_state] & self.get_value()) > 0
