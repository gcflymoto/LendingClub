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

    def __init__(self, args, current=None):
        '''
        Constructor
        '''
        self.options = []
        self.current = current
        self.args = args

    def getCount(self):
        return len(self.options)

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

    def increment(self, filters):
        n = 1
        num = len(filters)
        while (not filters[num - n].next()):
            n += 1

        return filters[num - n]

    def getCurrent(self):
        return self.options[self.current]

    def getName(self):
        return self.__class__.__name__

    def getDetails(self):
        return self.getName() + '=' + str(self.getCurrent())

    def getStringValue(self):
        return str(self.getCurrent())
