'''
Created on May 30, 2013

@author: gczajkow
'''

from Filter import Filter
from LoanEnum import LOAN_ENUM_grade

class CreditGrade(Filter):
    '''
    '''

    def __init__(self, args, current=None):
        '''
        Construct a set similar to this based on the passed in grades
         'A',
         'AB',
         'ABC',
         'ABCD',
         'ABCDE',
         'ABCDEF',
         'ABCDEFG',
         'B',
         'BC',
         'BCD',
         'BCDE',
         'BCDEF',
         'BCDEFG',
         'C',
         'CD',
         'CDE',
         'CDEF',
         'CDEFG',
         'D',
         'DE',
         'DEF',
         'DEFG',
         'E',
         'EF',
         'EFG',
         'F',
         'FG',
         'G'
        '''
        Filter.__init__(self, args, current)

        l = []
        num_grades = len(args.grades)
        for i in range(1, num_grades + 1):
            for j in range(num_grades):
                if (j + i) <= num_grades:
                    l.append(args.grades[j:j + i])
        self.options = l

    def apply(self, loan):
        current = self.getCurrent()
        return loan[LOAN_ENUM_grade] in current
