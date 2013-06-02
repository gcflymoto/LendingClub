#!/usr/local/bin/python2.7 -u
# encoding: utf-8
'''
lcbt -- Lending Club Back Testing based on Genetic Algorithm

lcbt is a tool which analyzes Lending Club loan data and uses a genetic algorithm to determine the best set of criteria for finding the loans which have the lowest default rate and highest profit

@author:     Greg Czajkowski
        
@copyright:  2013 organization_name. All rights reserved.
        
@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
'''

import sys
import os
import random
import pickle
import csv
import re
import datetime
import math
import functools
import codecs
import time

# from pretty_decimal import pretty_decimal
# from decimal import Decimal

from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter

import AmountRequested
import CreditGrade
import DebtToIncomeRatio
import Delinquencies
import EarliestCreditLine
import HomeOwnership
import Inquiries
import LoanPurpose
import MonthsSinceLastDelinquency
import PublicRecordsOnFile
import RevolvingLineUtilization
import State
import TotalCreditLines
import WordsInDescription
from LoanEnum import *

__all__ = []
__version__ = 1.0
__date__ = '2013-05-29'
__updated__ = '2013-05-29'

DEBUG = 0
TESTRUN = 0
PROFILE = 0

DEFAULT_REVOLING_UTILIZATION = 0.0
DEFAULT_EARLIEST_CREDIT_LINE = 0
DEFAULT_MTHS_SINCE_LAST_DELINQ = 61
DEFAULT_INQ_LAST_6MTHS = 0
DEFAULT_PUBIC_RECORDS = 0
DEFAULT_TOTAL_ACCOUNTS = 0
DEFAULT_DELINQUENCIES_LAST_2YEARS = 0

class Unbuffered:
    def __init__(self, stream):
        self.stream = stream
    def write(self, data):
        self.stream.write(data)
        self.stream.flush()
    def __getattr__(self, attr):
        return getattr(self.stream, attr)

if sys.platform == "win32":
    # On Windows, the best timer is time.clock()
    default_timer = time.clock
else:
    # On most other platforms the best timer is time.time()
    default_timer = time.time

class Timer(object):
    def __enter__(self, timer=default_timer):
        self.__start = timer()

    def __exit__(self, _, value, traceback, timer=default_timer):
        # Error handling here
        self.__finish = timer()

    def duration_in_seconds(self):
        return self.__finish - self.__start

if sys.version_info.major < 3:
    class UTF8Recoder(object):
        """
        Iterator that reads an encoded stream and re-encodes the input to UTF-8
        """
        def __init__(self, f, encoding):
            self.reader = codecs.getreader(encoding)(f)

        def __iter__(self):
            return self

        def next(self):
            return self.reader.next().encode("utf-8")

    class UnicodeReader(object):
        """
        A CSV reader which will iterate over lines in the CSV file "f",
        which is encoded in the given encoding.
        """
        def __init__(self, f, dialect=csv.excel, encoding="utf-8", **kwds):
            f = UTF8Recoder(f, encoding)
            self.reader = csv.reader(f, dialect=dialect, **kwds)

        # Do this weird way to get at unicode so pyDev does not complain about undefined function under python3.2
        def next(self, unicode=__builtins__.__dict__['unicode']):
            row = self.reader.next()
            return [unicode(s, "utf-8") for s in row]

        def __iter__(self):
            return self


class CLIError(Exception):
    '''Generic exception to raise and log different fatal errors.'''
    def __init__(self, msg):
        super(CLIError).__init__(type(self))
        self.msg = "E: %s" % msg
    def __str__(self):
        return self.msg
    def __unicode__(self):
        return self.msg

class GA_Test:
    population = []

    def __init__(self, lcbt, args):
        self.args = args
        self.lcbt = lcbt

        csv_field_names = []
        for _ in range(self.args.population_size):
            filters = [
                       AmountRequested.AmountRequested(args),
                       CreditGrade.CreditGrade(args),
                       DebtToIncomeRatio.DebtToIncomeRatio(args),
                       Delinquencies.Delinquencies(args),
                       EarliestCreditLine.EarliestCreditLine(args),
                       HomeOwnership.HomeOwnership(args),
                       Inquiries.Inquiries(args),
                       LoanPurpose.LoanPurpose(args),
                       MonthsSinceLastDelinquency.MonthsSinceLastDelinquency(args),
                       PublicRecordsOnFile.PublicRecordsOnFile(args),
                       RevolvingLineUtilization.RevolvingLineUtilization(args),
                       State.State(args),
                       TotalCreditLines.TotalCreditLines(args),
                       WordsInDescription.WordsInDescription(args)
                    ]
            for lc_filter in filters:
                csv_field_names.append(lc_filter.getName())
                lc_filter.current = random.randint(0, lc_filter.getCount() - 1)
            self.population.append({'filters':filters})

        csv_field_names.extend([
                                'expected_apy',
                                'num_loans',
                                'num_defaulted',
                                'pct_defaulted',
                                'avg_default_loss',
                                'net_apy'
                                ])

        self.csvwriter = csv.DictWriter(Unbuffered(open(self.args.csvresults, 'w')), fieldnames=csv_field_names, extrasaction='ignore')
        self.csvwriter.writeheader()

    def run(self):

        self.iteration_time = 0

        for i in range(self.args.iterations):
            self.iteration = i
            sys.stdout.write("Calculating fitness\n")

            timer = Timer()

            with timer:
                self.calculateFitness()
                self.sortByFitness()

            self.iteration_time += timer.duration_in_seconds()

            self.printBest()
            self.mate()

    def calculateFitness(self):
        for citizen in self.population:
            citizen['results'] = self.lcbt.test(citizen['filters'])
            sys.stdout.write('.')
        sys.stdout.write('\n')

    def sortByFitness(self):

        def net_apy_cmp(a, b, CONFIG_FITNESS_SORT_NUM_LOANS=self.args.fitness_sort_size):
            if (a['results']['num_loans'] < CONFIG_FITNESS_SORT_NUM_LOANS):
                a_fit = 0
            else:
                a_fit = int(a['results']['net_apy'] * 100)

            if (b['results']['num_loans'] < CONFIG_FITNESS_SORT_NUM_LOANS):
                b_fit = 0
            else:
                b_fit = int(b['results']['net_apy'] * 100)

            return b_fit - a_fit

        if sys.version_info.major >= 3:
            self.population.sort(key=functools.cmp_to_key(net_apy_cmp))
        else:
            self.population.sort(cmp=net_apy_cmp)

    def printBest(self):

        best_results = self.population[0]['results']

        filters = ''
        for lc_filter in self.population[0]['filters']:
            filter_name = lc_filter.getName()
            filter_val_str = str(lc_filter.getCurrent())
            best_results[filter_name] = filter_val_str
            filters += filter_name + '=' + filter_val_str + ','

        # Replace the last ',' with a newline
        sys.stdout.write(filters[:-1] + '\n')

        sys.stdout.write('[iteration %d/%d %.2f sec/iter] %d loans test at %0.2f%% APY. ' % (self.iteration + 1, self.args.iterations, self.iteration_time / (self.iteration + 1), best_results['num_loans'], best_results['expected_apy']))
        sys.stdout.write('%d loans defaulted (%0.2f%%, $%0.2f avg loss) ' % (best_results['num_defaulted'], best_results['pct_defaulted'], best_results['avg_default_loss']))
        sys.stdout.write('%0.4f%% net APY\n' % best_results['net_apy'])

        self.csvwriter.writerow(best_results)

    def mate(self):
        # Save the elite
        num_elite = int(self.args.elite_rate * self.args.population_size)
        final_population = self.population[:]

        mate_size = math.floor(self.args.population_size / 5.0)

        for i in range(num_elite, self.args.population_size):

            filters = final_population[i]['filters']

            for j in range(len(filters)):
                # Mate with 20% of population
                partner = random.randint(0, mate_size)

                filters[j].current = self.population[partner]['filters'][j].current

                # Mutate! Once in a blue moon
                if not random.randint(0, 1.0 / self.args.mutation_rate):
                    lc_filter = filters[j]
                    lc_filter.current = random.randint(0, lc_filter.getCount() - 1)
        self.population = final_population

class LCBT:
    def __init__(self, args):
        self.row = 0
        self.labels = False
        self.results = []
        self.filters = []
        self.loans = []
        self.datetime = datetime.datetime(2013, 1, 30)
        self.now = self.datetime.now()
        self.args = args
        self.skipped_loans = 0
        self.young_loans = 0
        self.removed_expired_loans = 0

    def initialize(self):
        # Check in-memory cache.
        if not self.loans:
            # Check serialized file cache.
            csv_mod = os.path.getmtime(self.args.stats)

            pickle_name = "loans_serialized_python%d.bin" % sys.version_info.major

            serialized_mod = os.path.getmtime(pickle_name) if os.path.exists(pickle_name) else 0
            if serialized_mod and serialized_mod > csv_mod:
                sys.stdout.write("Initializing from cache...")
                with open(pickle_name, "rb") as fh:
                    self.loans = pickle.load(fh)
                sys.stdout.write(" done\n")
            # double cache miss
            elif os.path.exists(self.args.stats):
                sys.stdout.write("Initializing from csv...")

                loans = []

                if sys.version_info.major >= 3:
                    fh = open(self.args.stats, newline='', encoding="latin-1")
                    csvreader = csv.reader(fh)
                else:
                    # fh = codecs.open(self.args.stats, mode='rU', encoding="utf-8")
                    # fh = codecs.open(self.args.stats, mode='U', encoding="latin-1")
                    # fh = codecs.open(self.args.stats, mode='rU', encoding="latin-1")
                    # fh = codecs.open(self.args.stats, 'rU', encoding="latin-1")
                    # csvreader = csv.reader(fh)
                    # fh = open(self.args.stats, "rU")
                    # csvreader = UnicodeDictReader(fh, encoding="latin-1")
                    fh = open(self.args.stats)
                    csvreader = UnicodeReader(fh, encoding="latin-1")

                for data in csvreader:
                    if not self.getLabels(data):
                        continue
                    raw_loan_dict = self.getLoan(data)
                    loan, parsed_ok = self.normalizeLoanData(raw_loan_dict)
                    if parsed_ok:
                        loans.append(loan)

                fh.close()

                with open(pickle_name, "wb") as fh:
                    pickle.dump(loans, fh)
                self.loans = loans
                sys.stdout.write(" done. Loans Found:" + str(len(loans)) + " Skipped Loans:" + str(self.skipped_loans) + " Skipped Young Loans:" + str(self.young_loans) + " Skipped Removed or Expired Loans:" + str(self.removed_expired_loans) + '\n')
            else:
                sys.stdout.write(self.args.stats + " not found.\n")
                sys.exit(-1)

    def test(self, filters):
        self.initialize()

        self.filters = filters
        invested = []

        for loan in self.loans:
            invest = self.consider(loan)
            if invest:
                invested.append(loan)
                # statuses[loan[LOAN_ENUM_loan_status]] += 1

        return self.getNAR(invested)

    def getNAR(self, invested):

        defaulted = 0
        profit = principal = lost = 0.0

        for loan in invested:
            if not loan[LOAN_ENUM_profit_calculated]:
                loan_defaulted = loan_lost = 0
                loan_profit = loan_principal = 0.0
                elapsed = loan[LOAN_ENUM_elapsed_months_since_issue_date]
                balance = loan[LOAN_ENUM_funded_amnt]
                monthly_payment = loan[LOAN_ENUM_installment]
                rate = loan[LOAN_ENUM_int_rate]
                ratio = 25.0 / balance
                payments = 0.0
                default = self.defaultedAmount(loan)
                while elapsed > 0:
                    elapsed -= 1
                    # Interest and service charge for the whole loan (not just me)
                    interest = balance * rate / 1200.0
                    service = 0.01 * monthly_payment
                    payments += monthly_payment
                    if (default and payments > loan[LOAN_ENUM_total_pymnt]):
                        loan_profit -= default * ratio
                        loan_lost += default * ratio
                        loan_defaulted = 1
                        break

                    # Compute my ratio of the profit
                    loan_profit += (interest - service) * ratio
                    loan_principal += balance * ratio
                    balance -= monthly_payment
                #
                # cache some of the calculated amounts
                #
                loan[LOAN_ENUM_profit] = loan_profit
                loan[LOAN_ENUM_principal] = loan_principal
                loan[LOAN_ENUM_lost] = loan_lost
                loan[LOAN_ENUM_defaulted] = loan_defaulted
                loan[LOAN_ENUM_profit_calculated] = 1
                #
                #
            profit += loan[LOAN_ENUM_profit]
            principal += loan[LOAN_ENUM_principal]
            lost += loan[LOAN_ENUM_lost]
            defaulted += loan[LOAN_ENUM_defaulted]

        num_loans = len(invested)

        if num_loans:

            # nar = pretty_decimal(Decimal(100 * math.pow(1 + profit / rounded_principal, 12) - 1), significance=4)
            nar = 100.0 * (math.pow(1 + profit / principal, 12) - 1.0)

            rate = 0.0
            for loan in invested:
                rate += loan[LOAN_ENUM_int_rate]

            # expect = pretty_decimal(Decimal(rate / num_loans), significance=2)
            expect = rate / num_loans

            # default_percent = pretty_decimal(Decimal(100 * defaulted / num_loans), significance=1)
            default_percent = 100 * defaulted / num_loans

            # avg_loss = pretty_decimal(Decimal(lost / defaulted), significance=2) if defaulted > 0  else pretty_decimal(Decimal(0.0), significance=2)
            avg_loss = (lost / defaulted) if defaulted > 0 else 0.0

            # # Count loan volume
            # per_month = 0
            # for loan in invested:
            #    if loan[LOAN_ENUM_issue_datetime].year == self.now.year and loan[LOAN_ENUM_issue_datetime].month == self.now.month:
            #        per_month += 1
        else:
            nar = 0.0
            rate = 0.0
            expect = 0.0
            default_percent = 0.0
            avg_loss = 0.0
            # per_month = 0

        return {
         'num_loans': num_loans,
         # 'loans_per_month': per_month,
         'expected_apy': expect,
         'num_defaulted': defaulted,
         'pct_defaulted': default_percent,
         'avg_default_loss': avg_loss,
         'net_apy': nar
         }

    def defaultedAmount(self, loan):
        if loan[LOAN_ENUM_loan_status] == "Charged Off" or loan[LOAN_ENUM_loan_status] == "Default":
            return loan[LOAN_ENUM_out_prncp]
        return 0

    def consider(self, loan):
        for lc_filter in self.filters:
            if lc_filter.current is not None and not lc_filter.apply(loan):
                return False
        return True

    def normalizeLoanData(self, raw_loan_dict):
        loan_array = LOAN_ENUM_default_data[:]
        if raw_loan_dict:
            try:
                loan_array[LOAN_ENUM_purpose] = raw_loan_dict["purpose"]
                loan_array[LOAN_ENUM_desc_word_count] = len([word_maybe for word_maybe in re.findall(r"[\w']+|[.,!?;%#\$&\-\(\)_]", raw_loan_dict["desc"]) if word_maybe not in ".,!?;%#\$&-()_"])
                loan_array[LOAN_ENUM_earliest_credit_line] = (self.now - self.datetime.strptime(raw_loan_dict["earliest_cr_line"], "%Y-%m-%d %H:%M")).total_seconds() / 60 / 60 / 24 / 365 if raw_loan_dict["earliest_cr_line"] else DEFAULT_EARLIEST_CREDIT_LINE
                loan_array[LOAN_ENUM_addr_state] = raw_loan_dict["addr_state"] if raw_loan_dict["addr_state"] else "NULL"
                loan_array[LOAN_ENUM_revol_utilization] = float(raw_loan_dict["revol_util"][:-1]) if raw_loan_dict["revol_util"] else DEFAULT_REVOLING_UTILIZATION
                loan_array[LOAN_ENUM_debt_to_income_ratio] = float(raw_loan_dict["dti"][:-1]) if raw_loan_dict["dti"][-1] == '%' else float(raw_loan_dict["dti"])
                loan_array[LOAN_ENUM_funded_amnt] = float(raw_loan_dict["funded_amnt"])
                loan_array[LOAN_ENUM_grade] = raw_loan_dict["grade"]
                loan_array[LOAN_ENUM_delinq_2yrs] = int(raw_loan_dict["delinq_2yrs"]) if raw_loan_dict["delinq_2yrs"] else DEFAULT_DELINQUENCIES_LAST_2YEARS
                loan_array[LOAN_ENUM_home_ownership] = raw_loan_dict["home_ownership"] if raw_loan_dict["home_ownership"] else "NULL"
                loan_array[LOAN_ENUM_inq_last_6mths] = int(raw_loan_dict["inq_last_6mths"]) if raw_loan_dict["inq_last_6mths"] else DEFAULT_INQ_LAST_6MTHS
                loan_array[LOAN_ENUM_mths_since_last_delinq] = int(raw_loan_dict["mths_since_last_delinq"]) if raw_loan_dict["mths_since_last_delinq"] else DEFAULT_MTHS_SINCE_LAST_DELINQ
                loan_array[LOAN_ENUM_pub_rec] = int(raw_loan_dict["pub_rec"]) if raw_loan_dict["pub_rec"] else DEFAULT_PUBIC_RECORDS
                loan_array[LOAN_ENUM_total_acc] = int(raw_loan_dict["total_acc"]) if raw_loan_dict["total_acc"] else DEFAULT_TOTAL_ACCOUNTS
                loan_array[LOAN_ENUM_loan_status] = raw_loan_dict["loan_status"]
                loan_array[LOAN_ENUM_issue_datetime] = self.datetime.strptime(raw_loan_dict["issue_d"], "%Y-%m-%d")
                loan_array[LOAN_ENUM_elapsed_months_since_issue_date] = math.floor((self.now - loan_array[LOAN_ENUM_issue_datetime]).total_seconds() / 86400 / 30)
                loan_array[LOAN_ENUM_installment] = float(raw_loan_dict["installment"])
                loan_array[LOAN_ENUM_int_rate] = float(raw_loan_dict["int_rate"][:-1])
                loan_array[LOAN_ENUM_total_pymnt] = float(raw_loan_dict["total_pymnt"])
                loan_array[LOAN_ENUM_out_prncp] = float(raw_loan_dict["out_prncp"])
                loan_array[LOAN_ENUM_profit_calculated] = 0
                loan_array[LOAN_ENUM_profit] = 0.0
                loan_array[LOAN_ENUM_principal] = 0.0
                loan_array[LOAN_ENUM_lost] = 0.0
                loan_array[LOAN_ENUM_defaulted] = 0

            except:
                sys.stdout.write("Error in row" + str(self.row) + '\n')
                sys.stdout.write("    home_ownership:" + str(raw_loan_dict["home_ownership"]) + '\n')
                sys.stdout.write("    addr_state:" + str(raw_loan_dict["addr_state"]) + '\n')
                sys.stdout.write("    revol_util:" + str(raw_loan_dict["revol_util"]) + '\n')
                sys.stdout.write("    dti:" + str(raw_loan_dict["dti"]) + '\n')
                sys.stdout.write("    desc:" + str(raw_loan_dict["desc"]) + '\n')
                sys.stdout.write("    earliest_cr_line:" + str(raw_loan_dict["earliest_cr_line"]) + '\n')
                sys.stdout.write("    issue_d:" + str(raw_loan_dict["issue_d"]) + '\n')
                raise
            return loan_array, True
        else:
            return loan_array, False

    def getLoan(self, data, issue_d_re=re.compile("^\d{4}-\d{2}-\d{2}$").match):
        loan = {}
        for c in range(len(data)):
            loan[self.labels[c]] = data[c]

        if "loan_status" not in loan or not loan["loan_status"] or "funded_amnt" not in loan or not loan["funded_amnt"]:
            sys.stdout.write("Skipping loan, did not find loan_status or funded_amnt" + str(loan) + '\n')
            self.skipped_loans += 1
            return False

        # Only look at loans > 3 months old
        if not issue_d_re(loan["issue_d"]):
            sys.stdout.write("Skipping loan, did not find issue_d:" + str(loan))
            self.skipped_loans += 1
            return False
        if (self.now - self.datetime.strptime(loan["issue_d"], "%Y-%m-%d")).total_seconds() < (86400 * self.args.young_loans_in_days):
            self.young_loans += 1

            return False
        # Ignore loans that didn't event start
        if loan["loan_status"] == "Removed" or loan["loan_status"] == "Expired":
            self.removed_expired_loans += 1
            return False

        # loan["rowid"] = self.row
        # loan["row_data"] = data

        return loan


    def getLabels(self, data):
        self.row += 1
        if self.row % 10000 == 0:
            sys.stdout.write('.')
        if self.row > 2:
            return True
        if self.row == 2:
            self.labels = [0] * len(data)
            for i in range(len(data)):
                self.labels[i] = data[i]

        return False


def main(argv=None):  # IGNORE:C0111
    '''Command line options.'''

    if argv is None:
        argv = sys.argv
    else:
        sys.argv.extend(argv)

    program_name = os.path.basename(sys.argv[0])
    program_version = "v%s" % __version__
    program_build_date = str(__updated__)
    program_version_message = '%%(prog)s %s (%s)' % (program_version, program_build_date)
    program_shortdesc = __import__('__main__').__doc__.split("\n")[1]
    program_license = '''%s - %s

  Created by user_name on %s.
  Copyright 2013 organization_name. All rights reserved.
  
  Licensed under the Apache License 2.0
  http://www.apache.org/licenses/LICENSE-2.0
  
  Distributed on an "AS IS" basis without warranties
  or conditions of any kind, either express or implied.

USAGE
''' % (program_name, program_shortdesc, str(__date__))

    # Setup argument parser
    parser = ArgumentParser(description=program_license, formatter_class=RawDescriptionHelpFormatter)
    parser.add_argument("-v", "--verbose", dest="verbose", action="count", help="set verbosity level [default: %(default)s]")
    parser.add_argument('-V', '--version', action='version', version=program_version_message)
    parser.add_argument('-g', '--grades', default='ABCDEF', help="String with the allowed credit grades [default: %(default)s]")
    parser.add_argument('-r', '--random_seed', default=100, help="Random Number Generator Seed [default: %(default)s]")
    parser.add_argument('-s', '--stats', default="LoanStatsNew.csv", help="Input Loan Stats CSV file [default: %(default)s]")
    parser.add_argument('-c', '--csvresults', default="lc_best.csv", help="Output best results CSV file [default: %(default)s]")
    parser.add_argument('-p', '--population_size', default=512, type=int, help="population size [default: %(default)s]")
    parser.add_argument('-i', '--iterations', default=4096, type=int, help="iterations [default: %(default)s]")
    parser.add_argument('-e', '--elite_rate', default=0.05, type=float, help="elite rate [default: %(default)s]")
    parser.add_argument('-m', '--mutation_rate', default=0.05, type=float, help="mutation rate [default: %(default)s]")
    parser.add_argument('-f', '--fitness_sort_size', default=1000, type=int, help="number of loans to limit the fitness sort size, the larger the longer and more optimal solution [default: %(default)s]")
    parser.add_argument('-y', '--young_loans_in_days', default=3 * 30, type=int, help="filter young loans if they are younger than specified number of days [default: %(default)s]")

    # Process arguments
    args = parser.parse_args()

    random.seed(args.random_seed)

    lcbt = LCBT(args)
    lcbt.initialize()

    ga_test = GA_Test(lcbt, args)
    ga_test.run()

    return 0

if __name__ == "__main__":
    sys.stdout = Unbuffered(sys.stdout)
    if DEBUG:
        sys.argv.append("-v")
    if TESTRUN:
        import doctest
        doctest.testmod()
    if PROFILE:
        import cProfile
        import pstats
        profile_filename = 'lcbt_profile.prof'
        cProfile.run('main()', profile_filename)
        with open("profile_stats.txt", "wb") as statsfile:
            p = pstats.Stats(profile_filename, stream=statsfile)
            stats = p.strip_dirs().sort_stats('cumulative')
            stats.print_stats()
        sys.exit(0)
    sys.exit(main())
