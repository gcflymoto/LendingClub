#!/usr/local/bin/python2.7 -u
# encoding: utf-8
"""
lcbt -- Lending Club Back Testing based on Genetic Algorithm

lcbt is a tool which analyzes Lending Club loan data and uses a genetic algorithm to determine the best set of 
criteria for finding the loans which have the lowest default rate and highest profit

@author:     Gregory Czajkowski
        
@copyright:  2013 Freedom. All rights reserved.
        
@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com

Setting up Pypy
$ curl -O http://python-distribute.org/distribute_setup.py
$ curl -O https://raw.githubusercontent.com/pypa/pip/master/contrib/get-pip.py
$ ./pypy-2.1/bin/pypy distribute_setup.py
$ ./pypy-2.1/bin/pypy get-pip.py
$ ./pypy-2.1/bin/pip install pygments
$ ./pypy-2.1/bin/pip install zmqpy
$ ./pypy-2.1/bin/pip install pyzmq
"""

import sys
import os
import random
import pickle
import csv
import re
import datetime
import math
import functools

# from pretty_decimal import pretty_decimal
# from decimal import Decimal

from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter
from multiprocessing import Process, cpu_count

import AccountsOpenPast24Months
import AmountRequested
import AnnualIncome
import CreditGrade
import DebtToIncomeRatio
import Delinquencies
import EarliestCreditLine
import EmploymentLength
import HomeOwnership
import IncomeValidated
import InquiriesLast6Months
import LoanPurpose
import MonthsSinceLastDelinquency
import PublicRecordsOnFile
import RevolvingLineUtilization
import State
import TotalCreditLines
import WordsInDescription

import Filter
import utilities
from LoanEnum import *

ConversionFilters = {
    LOAN_ENUM_acc_open_past_24mths: AccountsOpenPast24Months.AccountsOpenLast24Months,
    LOAN_ENUM_funded_amnt: AmountRequested.AmountRequested,
    LOAN_ENUM_annual_income: AnnualIncome.AnnualIncome,
    LOAN_ENUM_grade: CreditGrade.CreditGrade,
    LOAN_ENUM_debt_to_income_ratio: DebtToIncomeRatio.DebtToIncomeRatio,
    LOAN_ENUM_delinq_2yrs: Delinquencies.Delinquencies,
    LOAN_ENUM_earliest_credit_line: EarliestCreditLine.EarliestCreditLine,
    LOAN_ENUM_emp_length: EmploymentLength.EmploymentLength,
    LOAN_ENUM_home_ownership: HomeOwnership.HomeOwnership,
    LOAN_ENUM_income_validated: IncomeValidated.IncomeValidated,
    LOAN_ENUM_inq_last_6mths: InquiriesLast6Months.InquiriesLast6Months,
    LOAN_ENUM_purpose: LoanPurpose.LoanPurpose,
    LOAN_ENUM_mths_since_last_delinq: MonthsSinceLastDelinquency.MonthsSinceLastDelinquency,
    LOAN_ENUM_pub_rec: PublicRecordsOnFile.PublicRecordsOnFile,
    LOAN_ENUM_revol_utilization: RevolvingLineUtilization.RevolvingLineUtilization,
    LOAN_ENUM_addr_state: State.State,
    LOAN_ENUM_total_acc: TotalCreditLines.TotalCreditLines,
    LOAN_ENUM_desc_word_count: WordsInDescription.WordsInDescription,
}

#----------------------------------------------------------------------------------------------------------------------

enable_workers = utilities.check_for_zmqpy() or utilities.check_for_pyzmq()

#----------------------------------------------------------------------------------------------------------------------

__all__ = []
__version__ = 1.1
__date__ = '2013-05-29'
__updated__ = '2014-06-14'

DEBUG = 0
TESTRUN = 0
PROFILE = 0

class ProfileGuidedOptimization(object):
    def __init__(self, f, intial_value):
        self.f = f
        self.initial_value = intial_value
        self.current_value = intial_value
        self.exponent = 1.0
        self.backoff = 0.5
        self.last_duration = 0

    def __call__(self):
        # sys.stdout.write("timing function call duration for %s\n" % self.f.__name__)
        t = utilities.Timer()
        with t:
            self.f(self.current_value)
        current_duration = t.duration_in_seconds()

        if not self.last_duration:
            # This is the first time we called this function just store our duration for next time
            self.last_duration = current_duration
            return

        # compare the current duration to the previous
        # if the current duration is less keep incrementing exponentially
        sys.stdout.write(
            "function call duration for %s is %.02f while the last duration was %.02f exponent=%.02f backoff=%.02f\n" %
            (self.f.__name__, current_duration, self.last_duration, self.exponent, self.backoff))
        if current_duration < self.last_duration:
            self.exponent += 1 / (10 + self.backoff)
            self.backoff += 1 / (10 + self.backoff)
        else:
            # if the current duration is more keep decrementing exponentially
            self.backoff -= self.backoff / 2
            self.exponent -= self.backoff
        self.current_value = self.initial_value ** self.exponent

class GATest:
    population = []

    def __init__(self, conversion_filters, lcbt, args):
        """
        :param conversion_filters:
        :param lcbt:
        :param args:
        """
        self.iteration = 0
        self.iteration_time = 0
        self.conversion_filters = conversion_filters
        self.lcbt = lcbt
        self.args = args

        csv_field_names = []
        for _ in range(self.args.population_size):
            filters = [lc_filter(args) for lc_filter in conversion_filters.values()]
            for lc_filter in filters:
                csv_field_names.append(lc_filter.get_name())
                lc_filter.current = random.randint(0, lc_filter.get_count() - 1)
            self.population.append(dict(filters=filters))

        csv_field_names.extend([
            'expected_apy',
            'num_loans',
            'num_defaulted',
            'pct_defaulted',
            'avg_default_loss',
            'net_apy'
        ])

        self.csvwriter = csv.DictWriter(utilities.Unbuffered(open(self.args.csvresults, 'w')), fieldnames=csv_field_names,
                                        extrasaction='ignore')
        self.csvwriter.writeheader()

    def run(self):

        self.iteration_time = 0

        for i in range(self.args.iterations):
            self.iteration = i
            #sys.stdout.write("Calculating fitness of iteration %d\n" % i)

            timer = utilities.Timer()

            with timer:
                self.calculate_fitness()
                self.sort_by_fitness()

            self.iteration_time += timer.duration_in_seconds()

            self.print_best()
            self.mate()

    # Python27 does not have decorator support like we want instead use wrapper
    # functions which do additional manipulation
    def calculate_fitness(self):
        self.calculate()

    def calculate(self):
        for citizen in self.population:
            citizen['results'] = self.lcbt.test(citizen['filters'])
            sys.stdout.write('.')
        sys.stdout.write('\n')

    def sort_by_fitness(self):

        def net_apy_cmp(a, b, config_fitness_sort_num_loans=self.args.fitness_sort_size):
            if a['results']['num_loans'] < config_fitness_sort_num_loans:
                a_fit = 0
            else:
                a_fit = int(a['results']['net_apy'] * 100)

            if b['results']['num_loans'] < config_fitness_sort_num_loans:
                b_fit = 0
            else:
                b_fit = int(b['results']['net_apy'] * 100)

            return b_fit - a_fit

        if sys.version_info.major >= 3:
            self.population.sort(key=functools.cmp_to_key(net_apy_cmp))
        else:
            self.population.sort(cmp=net_apy_cmp)

    def print_best(self):

        best_results = self.population[0]['results']

        filters = ''
        for lc_filter in self.population[0]['filters']:
            filter_name = lc_filter.get_name()
            filter_val_str = str(lc_filter)
            best_results[filter_name] = filter_val_str
            filters += filter_name + '=' + filter_val_str + ','

        # Replace the last ',' with a newline
        sys.stdout.write('Best Filter: ' + filters[:-1] + '\n')

        sys.stdout.write('[iteration %d/%d %.2f sec/iter] Matched %d/%d loans (%d/mo.) test at %0.2f%% APY. ' % (
            self.iteration + 1,
            self.args.iterations,
            self.iteration_time / (self.iteration + 1),
            best_results['num_loans'],
            len(self.lcbt.loan_data.loans),
            best_results['loans_per_month'],
            best_results['expected_apy']))
        sys.stdout.write('%d loans defaulted (%0.2f%%, $%0.2f avg loss) ' % (
            best_results['num_defaulted'],
            best_results['pct_defaulted'],
            best_results['avg_default_loss']))
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
                    lc_filter.current = random.randint(0, lc_filter.get_count() - 1)
        self.population = final_population


class ZmqGATest(GATest):
    def __init__(self, conversion_filters, lcbt, args):
        GATest.__init__(self, conversion_filters, lcbt, args)

        self.range_filters = range(len(conversion_filters))

        # Initialize a zeromq context
        self.context = utilities.zmq.Context()

        # Set up a channel to send work
        self.ventilator_send = self.context.socket(utilities.zmq.PUSH)
        self.ventilator_send.bind("tcp://127.0.0.1:5557")

        # Set up a channel to receive results
        self.results_receiver = self.context.socket(utilities.zmq.PULL)
        self.results_receiver.bind("tcp://127.0.0.1:5558")

        # Set up a channel to send control commands
        self.control_sender = self.context.socket(utilities.zmq.PUB)
        self.control_sender.bind("tcp://127.0.0.1:5559")

        # Give everything a second to spin up and connect
        sys.stdout.write('Worker[-1] Waiting for all the workers to get setup\n')
        # Wait for all workers to send a message indicating they are ready
        for idx in range(self.args.workers):
            self.results_receiver.recv_json()
            if self.args.verbose:
                sys.stdout.write('Worker[-1] Received Setup confirmation from %d\n' % idx)
        sys.stdout.write('Worker[-1] Workers are setup\n')
        self.pgo = ProfileGuidedOptimization(self.calculate, 100)

    def run(self):
        GATest.run(self)

        # Signal to all workers that we are finished
        self.control_sender.send_unicode(Filter.u("FINISHED"))

    def calculate_fitness(self):
        self.pgo()

    def calculate(self, min_work_messages=100):
        citizen_idx = 0

        # Bundle up the filters
        work_messages = []

        for citizen in self.population:

            json_filters = [0] * len(self.conversion_filters)
            for i in self.range_filters:
                json_filters[i] = citizen['filters'][i].current

            work_messages.append((citizen_idx, json_filters))

            if len(work_messages) > int(min_work_messages):
                self.ventilator_send.send_json(work_messages)
                work_messages = []

            # citizen['results'] = self.lcbt.test(citizen['filters'])
            # sys.stdout.write('.')

            citizen_idx += 1

        # Send the tail
        if work_messages:
            self.ventilator_send.send_json(work_messages)

        # Send control message to signal sending back the results
        self.control_sender.send_unicode(Filter.u("SEND_RESULTS"))
        results_receiver = self.results_receiver
        for _ in range(self.args.workers):
            result_message = results_receiver.recv_json()
            if self.args.verbose:
                sys.stdout.write("Worker[%i] returned %d results\n" %
                                 (result_message['worker'], len(result_message['results'])))
            for citizen_idx, results in result_message['results']:
                # sys.stderr.write("results[%d]=%s\n" % (citizen_idx, results))
                self.population[citizen_idx]['results'] = results
        sys.stdout.write('\n')


class LCLoanData:
    def __init__(self, conversion_filters, args, worker_idx):

        # Create each of the filters and use its conversion utility for normalizing the data
        self.filters = [None] * len(conversion_filters)
        for filter_idx, lc_filter in conversion_filters.items():
            self.filters[filter_idx] = lc_filter(args)

        self.args = args
        self.worker_idx = worker_idx

        self.row = 0
        self.datetime = datetime.datetime(2013, 1, 30)
        self.now = self.datetime.now()

        delta = datetime.timedelta(days=(args.young_loans_in_days + 30))
        self.last_date_for_full_month_for_volume = self.datetime.now() - delta

        self.labels = []
        self.results = []
        self.loans = []

        self.skipped_loans = 0
        self.young_loans = 0
        self.removed_expired_loans = 0

    def initialize(self):
        # Check serialized file cache.
        csv_mod = os.path.getmtime(self.args.stats)

        pickle_name = "loans_serialized_python%d.bin" % sys.version_info.major

        serialized_mod = os.path.getmtime(pickle_name) if os.path.exists(pickle_name) else 0
        if serialized_mod and serialized_mod > csv_mod:
            sys.stdout.write("Worker[%d] Initializing from %s ...\n" % (self.worker_idx, pickle_name))
            with open(pickle_name, "rb") as fh:
                self.loans = pickle.load(fh)

            sys.stdout.write("Worker[%d] done\n" % self.worker_idx)
        # double cache miss
        elif os.path.exists(self.args.stats):
            sys.stdout.write("Worker[%d] Initializing from %s ...\n" % (self.worker_idx, self.args.stats))

            loans = []

            if sys.version_info.major >= 3:
                fh = open(self.args.stats, newline='', encoding="latin-1")
                csv_reader = csv.reader(fh)
            else:
                fh = open(self.args.stats)
                csv_reader = utilities.UnicodeReader(fh, encoding="latin-1")

            for data in csv_reader:
                # Skip the first row after reading the first rows field labels
                if self.get_labels(data):
                    continue
                raw_loan_dict = self.get_loan(data)
                loan, parsed_ok = self.normalize_loan_data(raw_loan_dict)
                if parsed_ok:
                    # Assign the rowid of the loan to be the current last index in the loans list
                    loan[LOAN_ENUM_rowid] = len(loans)
                    loans.append(loan)

            fh.close()

            with open(pickle_name, "wb") as fh:
                pickle.dump(loans, fh)

            self.loans = loans

            sys.stdout.write("Worker[%d] done initializing. Loans Found:" % self.worker_idx +
                             str(len(loans)) + " Skipped non-parseable Loans:" + str(self.skipped_loans) +
                             " Skipped Young Loans:" + str(self.young_loans) + " Skipped Removed or Expired Loans:" +
                             str(self.removed_expired_loans) + '\n')
        else:
            sys.stdout.write(self.args.stats + " not found.\n")
            sys.exit(-1)

    def get_labels(self, data):
        # First row in the data is a garbage comment
        # Second row is where we have the labels
        self.row += 1

        # Print some status to the user that we are processing data
        if self.row % 10000 == 0:
            sys.stdout.write('.')
        if self.row > 2:
            return False
        if self.row == 2:
            self.labels = data[:]

        return True

    def get_loan(self, data, issue_d_re=re.compile("^\d{4}-\d{2}-\d{2}$").match):
        loan = dict()
        for c in range(len(data)):
            loan[self.labels[c]] = data[c]

        if "loan_status" not in loan or not loan["loan_status"] or "funded_amnt" not in loan or not loan["funded_amnt"]:
            # sys.stdout.write("Skipping loan, did not find loan_status or funded_amnt" + str(loan) + '\n')
            self.skipped_loans += 1
            return False

        # Only look at loans > 3 months old
        if not issue_d_re(loan["issue_d"]):
            sys.stdout.write("Skipping loan, did not find issue_d:" + str(loan))
            self.skipped_loans += 1
            return False

        # Ignore loans that are too young for consideration
        tot_seconds = (self.now - self.datetime.strptime(loan["issue_d"], "%Y-%m-%d")).total_seconds()
        if tot_seconds < (86400 * self.args.young_loans_in_days):
            self.young_loans += 1
            return False

        # Ignore loans that didn't event start
        if loan["loan_status"] == "Removed" or loan["loan_status"] == "Expired":
            self.removed_expired_loans += 1
            return False

        # loan["rowid"] = self.row
        # loan["row_data"] = data

        return loan

    def get_nar(self, invested):

        defaulted = 0
        profit = principal = lost = 0.0

        for loan in invested:
            profit += loan[LOAN_ENUM_profit]
            principal += loan[LOAN_ENUM_principal]
            lost += loan[LOAN_ENUM_lost]
            defaulted += loan[LOAN_ENUM_defaulted]

        num_loans = len(invested)

        if num_loans:

            if principal == 0:
                nar = 0.0
            else:
                # nar = pretty_decimal(Decimal(100 * math.pow(1 + profit / rounded_principal, 12) - 1), significance=4)
                nar = 100.0 * (math.pow(1 + profit / principal, 12) - 1.0)

            rate = 0.0
            for loan in invested:
                rate += loan[LOAN_ENUM_int_rate]

            # expect = pretty_decimal(Decimal(rate / num_loans), significance=2)
            expect = rate / num_loans

            # default_percent = pretty_decimal(Decimal(100 * defaulted / num_loans), significance=1)
            default_percent = 100 * defaulted / num_loans

            # avg_loss = pretty_decimal(Decimal(lost / defaulted), significance=2)
            #            if defaulted > 0 else pretty_decimal(Decimal(0.0), significance=2)
            avg_loss = (lost / defaulted) if defaulted > 0 else 0.0

            # Count loan volume
            per_month = 0
            for loan in invested:
                if loan[LOAN_ENUM_issue_datetime].year == self.last_date_for_full_month_for_volume.year and \
                   loan[LOAN_ENUM_issue_datetime].month == self.last_date_for_full_month_for_volume.month:
                    per_month += 1
        else:
            nar = 0.0
            expect = 0.0
            default_percent = 0.0
            avg_loss = 0.0
            per_month = 0

        return dict(num_loans=num_loans, loans_per_month=per_month, expected_apy=expect, num_defaulted=defaulted,
                    pct_defaulted=default_percent, avg_default_loss=avg_loss, net_apy=nar)

    def normalize_loan_data(self, raw_loan_dict):
        loan_dict = LOAN_ENUM_default_data[:]
        conversion_filters = self.filters
        if raw_loan_dict:
            try:
                loan_dict[LOAN_ENUM_acc_open_past_24mths] = conversion_filters[LOAN_ENUM_acc_open_past_24mths].convert(raw_loan_dict["acc_open_past_24mths"])
                loan_dict[LOAN_ENUM_funded_amnt] = conversion_filters[LOAN_ENUM_funded_amnt].convert(raw_loan_dict["funded_amnt"])
                loan_dict[LOAN_ENUM_annual_income] = conversion_filters[LOAN_ENUM_annual_income].convert(raw_loan_dict["annual_inc"])
                loan_dict[LOAN_ENUM_grade] = conversion_filters[LOAN_ENUM_grade].convert(raw_loan_dict["grade"])
                loan_dict[LOAN_ENUM_debt_to_income_ratio] = conversion_filters[LOAN_ENUM_debt_to_income_ratio].convert(raw_loan_dict["dti"])
                loan_dict[LOAN_ENUM_delinq_2yrs] = conversion_filters[LOAN_ENUM_delinq_2yrs].convert(raw_loan_dict["delinq_2yrs"])
                loan_dict[LOAN_ENUM_earliest_credit_line] = conversion_filters[LOAN_ENUM_earliest_credit_line].convert(raw_loan_dict["earliest_cr_line"])
                loan_dict[LOAN_ENUM_emp_length] = conversion_filters[LOAN_ENUM_emp_length].convert(raw_loan_dict["emp_length"])
                loan_dict[LOAN_ENUM_home_ownership] = conversion_filters[LOAN_ENUM_home_ownership].convert(raw_loan_dict["home_ownership"])
                loan_dict[LOAN_ENUM_income_validated] = conversion_filters[LOAN_ENUM_income_validated].convert(raw_loan_dict["is_inc_v"])
                loan_dict[LOAN_ENUM_inq_last_6mths] = conversion_filters[LOAN_ENUM_inq_last_6mths].convert(raw_loan_dict["inq_last_6mths"])
                loan_dict[LOAN_ENUM_purpose] = conversion_filters[LOAN_ENUM_purpose].convert(raw_loan_dict["purpose"])
                loan_dict[LOAN_ENUM_mths_since_last_delinq] = conversion_filters[LOAN_ENUM_mths_since_last_delinq].convert(raw_loan_dict["mths_since_last_delinq"])
                loan_dict[LOAN_ENUM_pub_rec] = conversion_filters[LOAN_ENUM_pub_rec].convert(raw_loan_dict["pub_rec"])
                loan_dict[LOAN_ENUM_revol_utilization] = conversion_filters[LOAN_ENUM_revol_utilization].convert(raw_loan_dict["revol_util"])
                loan_dict[LOAN_ENUM_addr_state] = conversion_filters[LOAN_ENUM_addr_state].convert(raw_loan_dict["addr_state"])
                loan_dict[LOAN_ENUM_total_acc] = conversion_filters[LOAN_ENUM_total_acc].convert(raw_loan_dict["total_acc"])
                loan_dict[LOAN_ENUM_desc_word_count] = conversion_filters[LOAN_ENUM_desc_word_count].convert(raw_loan_dict["desc"])

                loan_dict[LOAN_ENUM_loan_status] = raw_loan_dict["loan_status"]
                loan_dict[LOAN_ENUM_issue_datetime] = self.datetime.strptime(raw_loan_dict["issue_d"], "%Y-%m-%d")
                loan_dict[LOAN_ENUM_number_of_payments] = int(raw_loan_dict["term"].strip().split()[0])
                loan_dict[LOAN_ENUM_installment] = float(raw_loan_dict["installment"])
                loan_dict[LOAN_ENUM_int_rate] = float(raw_loan_dict["int_rate"][:-1])
                loan_dict[LOAN_ENUM_total_pymnt] = float(raw_loan_dict["total_pymnt"])
                loan_dict[LOAN_ENUM_out_prncp] = float(raw_loan_dict["out_prncp"])
                loan_dict[LOAN_ENUM_out_prncp_inv] = float(raw_loan_dict["out_prncp_inv"])

                total_received_interest = float(raw_loan_dict["total_rec_int"])
                total_received_principal = float(raw_loan_dict["total_rec_prncp"])

                if loan_dict[LOAN_ENUM_loan_status] == "Charged Off" or loan_dict[LOAN_ENUM_loan_status] == "Default":
                    defaulted_amount = ((loan_dict[LOAN_ENUM_number_of_payments] * loan_dict[LOAN_ENUM_installment]) -
                                        (total_received_interest + total_received_principal)) * .99
                    loan_dict[LOAN_ENUM_lost] = defaulted_amount
                    loan_dict[LOAN_ENUM_defaulted] = 1
                else:
                    defaulted_amount = loan_dict[LOAN_ENUM_lost] = 0.0
                    loan_dict[LOAN_ENUM_defaulted] = 0

                loan_profit = loan_principal = loan_lost = 0.0
                elapsed = loan_dict[LOAN_ENUM_number_of_payments]
                balance = loan_dict[LOAN_ENUM_funded_amnt]
                monthly_payment = loan_dict[LOAN_ENUM_installment]
                rate = loan_dict[LOAN_ENUM_int_rate]
                ratio = 25.0 / balance
                payments = 0.0
                while elapsed > 0:
                    elapsed -= 1
                    # Interest and service charge for the whole loan (not just me)
                    interest = balance * rate / 1200.0
                    service = 0.01 * monthly_payment
                    payments += monthly_payment
                    if loan_dict[LOAN_ENUM_defaulted] and payments > loan_dict[LOAN_ENUM_total_pymnt]:
                        loan_profit -= defaulted_amount * ratio
                        loan_lost += defaulted_amount * ratio
                        break

                    # Compute my ratio of the profit
                    loan_profit += (interest - service) * ratio
                    loan_principal += balance * ratio
                    balance -= monthly_payment

                # sys.stderr.write("Calculated profit=%.2f principal=%.2f loss=%.2f ... ")
                # sys.stderr.write("csv total_received_interest=%.2f total_received_principal=%.2f loss=%.2f\n" %
                # (loan_profit, loan_principal, loan_lost, total_received_interest,
                # total_received_principal, loan_array[LOAN_ENUM_lost]))
                loan_dict[LOAN_ENUM_profit] = loan_profit
                loan_dict[LOAN_ENUM_principal] = loan_principal
                loan_dict[LOAN_ENUM_lost] = loan_lost

                # Profit will be the interest minus the service fee
                # loan_array[LOAN_ENUM_profit] = total_received_interest * .99
                # loan_array[LOAN_ENUM_principal] = total_received_principal

            except:
                sys.stdout.write("Error in row " + str(self.row) + '\n')
                for key,val in raw_loan_dict.iteritems():
                    sys.stdout.write("    %s: %s\n" % (key, val))
                raise
            return loan_dict, True
        else:
            return loan_dict, False

class LCBT:
    def __init__(self, conversion_filters, args, worker_idx):

        # Create each of the filters
        self.filters = [None] * len(conversion_filters)
        for filter_idx, lc_filter in conversion_filters.items():
            self.filters[filter_idx] = lc_filter(args)

        self.loan_data = LCLoanData(conversion_filters, args, worker_idx)
        self.args = args
        self.worker_idx = worker_idx

    def initialize(self):
        self.loan_data.initialize()

    def test(self, filters):
        self.filters = filters
        invested = [loan for loan in self.loan_data.loans if self.consider(loan)]

        return self.loan_data.get_nar(invested)

    def consider(self, loan):
        for lc_filter in self.filters:
            if lc_filter.current != lc_filter.size and not lc_filter.apply(loan):
                return False
        return True

    def debug_msg(self, msg):
        if self.args.verbose:
            sys.stdout.write("Worker[%i] %s\n" % (self.worker_idx, msg))


class ZmqLCBT(LCBT):
    def setup_zmq(self):

        # Initialize a zeromq context
        self.context = utilities.zmq.Context()

        # Set up a channel to receive work from the ventilator
        self.work_receiver = self.context.socket(utilities.zmq.PULL)
        self.work_receiver.connect("tcp://127.0.0.1:5557")

        # Set up a channel to send result of work to the results reporter
        self.results_sender = self.context.socket(utilities.zmq.PUSH)
        self.results_sender.connect("tcp://127.0.0.1:5558")

        # Set up a channel to receive control messages over
        self.control_receiver = self.context.socket(utilities.zmq.SUB)
        self.control_receiver.connect("tcp://127.0.0.1:5559")
        self.control_receiver.setsockopt_string(utilities.zmq.SUBSCRIBE, Filter.u(""))

        # Set up a poller to multiplex the work receiver and control receiver channels
        self.poller = utilities.zmq.Poller()
        self.poller.register(self.work_receiver, utilities.zmq.POLLIN)
        self.poller.register(self.control_receiver, utilities.zmq.POLLIN)

        # Send a message that we are done with the setup and ready to receive work
        answer_message = dict(worker=self.worker_idx, message='READY')
        self.results_sender.send_json(answer_message)

    def work(self):
        # Loop and accept messages from both channels, acting accordingly
        poller = self.poller
        work_receiver = self.work_receiver
        results_sender = self.results_sender
        control_receiver = self.control_receiver

        answer_message = dict(worker=self.worker_idx, results=[])

        while True:
            socks = dict(poller.poll())

            # If the message came from work_receiver channel, perform the work
            # and send the answer to the results reporter
            if socks.get(work_receiver) == utilities.zmq.POLLIN:
                work_messages = work_receiver.recv_json()

                self.debug_msg("received work_messages of length: %d" % len(work_messages))

                for citizen_idx, filters in work_messages:
                    for i in range(len(self.filters)):
                        self.filters[i].current = filters[i]

                    result = self.test(self.filters)
                    sys.stdout.write(str(self.worker_idx))
                    answer_message['results'].append((citizen_idx, result))
            # If the message came over the control channel, process the message type
            elif socks.get(control_receiver) == utilities.zmq.POLLIN:
                control_message = control_receiver.recv().decode('utf-8')
                self.debug_msg("control message received: %s" % control_message)

                if control_message == "SEND_RESULTS":
                    self.debug_msg("sending answer message")
                    results_sender.send_json(answer_message)
                    # Clear our results array
                    answer_message['results'] = []
                    continue

                if control_message == "FINISHED":
                    self.debug_msg("received FINSHED, quitting!")
                    break

                self.debug_msg("Exiting. Unknown control_message received %s", control_message)
                sys.exit(-1)


def worker(worker_idx, args):
    random.seed(args.random_seed)
    lcbt = ZmqLCBT(ConversionFilters, args, worker_idx)
    lcbt.initialize()
    lcbt.setup_zmq()
    lcbt.work()


def main(argv=None):  # IGNORE:C0111
    """Command line options."""

    if argv is not None:
        sys.argv.extend(argv)

    program_name = os.path.basename(sys.argv[0])
    program_version = "v%s" % __version__
    program_build_date = str(__updated__)
    program_version_message = '%%(prog)s %s (%s)' % (program_version, program_build_date)
    program_shortdesc = __import__('__main__').__doc__.split("\n")[1]
    program_license = '''%s - %s

  Created by user_name on %s.
  Copyright 2013 Freedom. All rights reserved.
  
  Licensed under the Apache License 2.0
  http://www.apache.org/licenses/LICENSE-2.0
  
  Distributed on an "AS IS" basis without warranties
  or conditions of any kind, either express or implied.

USAGE
''' % (program_name, program_shortdesc, str(__date__))

    # Setup argument parser
    parser = ArgumentParser(description=program_license, formatter_class=RawDescriptionHelpFormatter)
    parser.add_argument("-v", "--verbose", dest="verbose", action="count",
                        help="set verbosity level [default: %(default)s]")
    parser.add_argument('-V', '--version', action='version', version=program_version_message)
    parser.add_argument('-g', '--grades', default='ABCDEF',
                        help="String with the allowed credit grades [default: %(default)s]")
    parser.add_argument('-s', '--random_seed', default=100, help="Random Number Generator Seed [default: %(default)s]")
    parser.add_argument('-d', '--data',
                        default="https://www.lendingclub.com/fileDownload.action?file=LoanStatsNew.csv&type=gen",
                        help="Download path for the Data file [default: %(default)s]")
    parser.add_argument('-l', '--stats', default="LoanStatsNew.csv",
                        help="Input Loan Stats CSV file [default: %(default)s]")
    parser.add_argument('-c', '--csvresults', default="lc_best.csv",
                        help="Output best results CSV file [default: %(default)s]")
    parser.add_argument('-p', '--population_size', default=512, type=int, help="population size [default: %(default)s]")
    parser.add_argument('-i', '--iterations', default=4096, type=int, help="iterations [default: %(default)s]")
    parser.add_argument('-e', '--elite_rate', default=0.05, type=float, help="elite rate [default: %(default)s]")
    parser.add_argument('-m', '--mutation_rate', default=0.05, type=float, help="mutation rate [default: %(default)s]")
    parser.add_argument('-k', '--check', default=False, action='store_true', help="checking mode: open the CSV results file and filter the loans into a separate file [default: %(default)s]")
    parser.add_argument('-r', '--checkresults', default="LoanStatsNewFiltered.csv", help="file name for the filtered results used during checking mode [default: %(default)s]")
    parser.add_argument('-f', '--fitness_sort_size', default=1000, type=int,
                        help="number of loans to limit the fitness sort size, the larger the longer and more optimal solution [default: %(default)s]")
    parser.add_argument('-y', '--young_loans_in_days', default=3 * 30, type=int,
                        help="filter young loans if they are younger than specified number of days [default: %(default)s]")
    parser.add_argument('-w', '--workers', default=enable_workers * cpu_count(), type=int,
                        help="number of workers defaults to the number of cpu cores [default: %(default)s]")

    # Process arguments
    args = parser.parse_args()

    random.seed(args.random_seed)

    if os.path.exists(args.stats):
        sys.stdout.write("Using %s as the data file\n" % args.stats)
    else:
        utilities.download_data(args.data, args.stats)

    if enable_workers:
        lcbt = ZmqLCBT(ConversionFilters, args, worker_idx=-1)
        lcbt.initialize()
        for worker_idx in range(args.workers):
            Process(target=worker, args=(worker_idx, args)).start()
        ga_test = ZmqGATest(ConversionFilters, lcbt, args)
        ga_test.run()
    else:
        lcbt = LCBT(ConversionFilters, args, worker_idx=-1)
        lcbt.initialize()
        ga_test = GATest(ConversionFilters, lcbt, args)
        ga_test.run()

    return 0

if __name__ == "__main__":
    sys.stdout = utilities.Unbuffered(sys.stdout)
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
