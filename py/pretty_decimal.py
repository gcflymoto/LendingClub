'''
Created on May 31, 2013

@author:  Jeroen Pulles

@organization: http://www.redslider.net/2011/2011-08-14-number_format-for-python.html

'''
from decimal import Decimal


def _plusone(digits):
    for i in reversed(range(len(digits))):
        digits[i] = digits[i] + 1
        if digits[i] == 10:
            digits[i] = 0
        else:
            return False
    digits.insert(0, 1)
    return True


def pretty_decimal(decimal_object, decimal_separator=',', mille_separator=None, significance=3):
    if not isinstance(decimal_object, Decimal):
        return decimal_object
    t = decimal_object.as_tuple()

    if t.digits == (0,):
        return '0'

    integrals = []
    decimals = []
    w = len(t.digits)
    e = t.exponent
    if t.exponent >= 0:
        # Also includes e.g. 1.2E7 => 12.000.000
        integrals.extend(t.digits)
    else:
        if w > significance:
            # Too many digits, round, possibly adding 1 to the integrals;
            # Rounding only applies to the decimals part, but take care of
            # the carry over digit
            start_at = significance  # the offset for the digit you want to loose
            if w + t.exponent > significance:  # integral larger than significance, round to the integral
                start_at = w + t.exponent  # first decimal
            l = list(t.digits[0:start_at])  # non-rounded selection of digits
            e = t.exponent + (w - start_at)
            v = t.digits[start_at]
            if v >= 5:
                if _plusone(l) and e < 0:
                    # This may have added another digit, which we can ignore
                    # if it is a decimal; 99.9999999999 => 100.0 instead of 100
                    del l[-1]
                    e = e + 1
        else:
            l = list(t.digits)
            e = t.exponent
        # Split integrals/decimals:
        integrals = l[:max(0, len(l) + e)]
        if e < 0:
            decimals = l[e:]

    # Add vanity zero's:
    if e > 0:
        integrals.extend([0 for v in range(e)])
    if (len(decimals) + e) < 0:
        decimals = [0 for v in range(abs(len(decimals) + e))] + decimals

    cars = []  # sign + integrals + separator + decimals
    if mille_separator:
        for i in reversed(range(len(integrals))):
            cars.insert(0, integrals[i])
            if (len(integrals) - i) % 3 == 0 and i > 0:
                cars.insert(0, mille_separator)
    else:
        cars.extend(integrals)

    if decimals:
        if not integrals:
            cars.append(0)
        cars.append(decimal_separator)
        cars.extend(decimals)

    if t.sign:
        cars.insert(0, '-')

    return ''.join(map(lambda x: str(x), cars))
