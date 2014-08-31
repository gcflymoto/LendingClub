"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""
import Filter


class StubFilter(Filter.Filter):
    """
    classdocs
    """
    sqlite_type = "INT"
    name = "Id"
    query = ""
    named_query = ""

    def __init__(self, args, current=None):
        """
        Constructor
        """
        Filter.Filter.__init__(self, args, [1], current)

    def apply(self, loan):
        return True