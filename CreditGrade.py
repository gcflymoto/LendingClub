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
        grade_values = {'A': 1 << 0,
                        'B': 1 << 1,
                        'C': 1 << 2,
                        'D': 1 << 3,
                        'E': 1 << 4,
                        'F': 1 << 5,
                        'G': 1 << 6,
                        }

        self.conversion_table = grade_values.copy()
        self.reverse_table = {v:k for k, v in grade_values.items()}
        num_grades = len(args.grades)
        for i in range(1, num_grades + 1):
            for j in range(num_grades):
                if (j + i) <= num_grades:
                    grades = args.grades[j:j + i]
                    grades_bit_value = 0
                    for grade in grades:
                        grades_bit_value += grade_values[grade]
                    l.append(grades_bit_value)
                    self.conversion_table[grades] = grades_bit_value
                    self.reverse_table[grades_bit_value] = grades

        # Now do a conversion from a string

        self.options = l

    def convert(self, str_grades):
        # Convert a string of grades to a bit value
        # A = 1 << 0
        # B = 1 << 1
        # C = 1 << 2
        # ...
        return self.conversion_table[str(str_grades)]

    def __str__(self):
        return self.reverse_table[self.getCurrent()]

    def apply(self, loan):
        current = self.getCurrent()
        return loan[LOAN_ENUM_grade] & current > 0
