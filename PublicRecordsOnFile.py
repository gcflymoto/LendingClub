"""
Created on May 30, 2013

@author: gczajkow
"""

import Filter
import LoanEnum

DEFAULT_PUBIC_RECORDS = 0


class PublicRecordsOnFile(Filter.Filter):
    """
    classdocs
    """
    def __init__(self, args, current=None):
        """
        Constructor
        """
        options = ["Exclude loans with public records"]

        Filter.Filter.__init__(self, args, options, current)

    def convert(self, raw_data):
        return int(raw_data) if raw_data else DEFAULT_PUBIC_RECORDS

    def apply(self, loan, pub_rec=LoanEnum.LOAN_ENUM_pub_rec):
        # Does not mutate
        return not loan[pub_rec]
