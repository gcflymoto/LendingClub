"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import LoanEnum
import Filter


class LoanTerm(Filter.Filter):
    """
    """
    sqlite_type = "INT"
    name = "term"
    query = "((term & ?) > 0)"
    named_query = "((term & :term) > 0)"

    def __init__(self, args, current=None):
        """
        """

        options = []
        terms = args.terms.split(',')
        for term in terms:
            if term == "36":
                options.append(1 << 0)
            elif term == "60":
                options.append(1 << 1)

        # Skip the bitset value of 0, which is nonsensical
        #
        Filter.Filter.__init__(self, args, self.power_bitset(options)[1:], current)

    def convert(self, str_term):
        if str_term == " 36 months":
            return 1 << 0
        if str_term == " 60 months":
            return 1 << 1
        assert 0, "unknown loan term: " + str_term
        return 0

    def __str__(self):
        value = self.get_value()
        result = ""
        if ((1 << 0) & value) > 0:
            result += "36,"
        if ((1 << 1) & value) > 0:
            result += "60,"
        return result[:-1]

    def apply(self, loan, term=LoanEnum.LOAN_ENUM_term):
        return (loan[term] & self.get_value()) > 0
