"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""


import itertools
import sys

if sys.version < '3':
    import codecs
    def u(x):
        return codecs.unicode_escape_decode(x)[0]
    def s(x):
        return x.encode('ascii')
else:
    def u(x):
        return x
    def s(x):
        return x.encode('ascii')

class Filter(object):
    """
    classdocs
    """
    def __init__(self, args, options, current=None):
        """
        Constructor
        """
        self.args = args
        self.options = options
        self.size = len(options)
        self.size_plus_one = self.size + 1
        if current is None:
            self.current = self.size
        else:
            self.current = current

    def convert(self, str_data):
        return float(str_data)

    def __str__(self):
        return str(self.get_current())

    def get_count(self):
        return self.size

    def next(self):
        #
        # Original algorithm used 'None'
        #
        #if self.current is None:
        #    self.current = 0
        #elif self.current + 1 < self.size:
        #    self.current += 1
        #else:
        #    self.current = None
        #return self.current is not None
        self.current = (self.current + 1) % self.size_plus_one

        return self.current != self.size

        # #
        # # Switched to an algorithm which uses an index that is out of bounds as None
        # #
        # self.current = (self.current + 1) % (self.size + 1)
        # return self.current != self.size

    def power_set(self, options):
        # http://docs.python.org/2/library/itertools.html#recipes
        # print("Creating powerset for", options)
        l = list(itertools.chain.from_iterable(itertools.combinations(options, r) for r in range(len(options) + 1)))
        # print("Done creating powerset for", options)
        return l

    def power_bitset(self, options):
        l = self.power_set(options)
        # No we need to sum up each individual set_tupple
        return [sum(set_tupple) for set_tupple in l]

    def increment(self, filters):
        """
        ###
        ### Original algorithm which went backwards through the filters
        ###        
        n = 1
        num = len(filters)
        while (not filters[num - n].next()):
           n += 1
        return filters[num - n]
        
        ###
        ### My algorithm goes from front to back through the filters and is more pythonistic
        ###
        """

        for lc_filter in filters:
            if not lc_filter.next():
                continue
            else:
                return lc_filter
        return None

    def get_current(self):
        return self.options[self.current]

    def get_name(self):
        return self.__class__.__name__

    def get_details(self):
        return self.get_name() + '=' + str(self.get_current())

    def get_string_value(self):
        return str(self.get_current())
