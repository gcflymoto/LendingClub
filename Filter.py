'''
Created on May 29, 2013

@author: gczajkow
'''
import itertools

class Filter(object):
    '''
    classdocs
    '''
    pass

    def __init__(self, args, options, current=None):
        '''
        Constructor
        '''
        self.args = args
        self.options = options
        self.size = len(options)
        self.current = current

    def convert(self, str_data):
        return float(str_data)

    def __str__(self):
        return str(self.getCurrent())

    def getCount(self):
        return self.size

    def next(self):
        if self.current is None:
            self.current = 0
        elif self.current + 1 < len(self.options):
            self.current += 1
        else:
            self.current = None
        return self.current is not None

    def powerSet(self, options):
        # http://docs.python.org/2/library/itertools.html#recipes
        # print("Creating powerset for", options)
        l = list(itertools.chain.from_iterable(itertools.combinations(options, r) for r in range(len(options) + 1)))
        # print("Done creating powerset for", options)
        return l

    def powerBitSet(self, options):
        l = self.powerSet(options)
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
        return lc_filter

    def getCurrent(self):
        return self.options[self.current]

    def getName(self):
        return self.__class__.__name__

    def getDetails(self):
        return self.getName() + '=' + str(self.getCurrent())

    def getStringValue(self):
        return str(self.getCurrent())
