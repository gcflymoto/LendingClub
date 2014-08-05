#!/usr/local/bin/python2.7 -u
# encoding: utf-8
"""
lcbt -- Lending Club Back Testing based on Genetic Algorithm

lcbt is a tool which analyzes Lending Club loan data and uses a genetic algorithm to determine the best set of
criteria for finding the loans which have the lowest default rate and highest return

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

C:\Users\gczajkow\boost\boost_1_53_0>.\b2 toolset=msvc-12.0 install --prefix=C:\Users\gczajkow\boost\boost_1_53_0.release
"""
import sys
import os
import random
import csv
import math
import functools
import copy

# from pretty_decimal import pretty_decimal
# from decimal import Decimal

from argparse import ArgumentParser
from argparse import RawDescriptionHelpFormatter
from multiprocessing import Process, cpu_count
from multiprocessing import Queue as MultiProcessingQueue

import AccountsOpenPast24Months, AmountRequested, Delinquencies, MonthsSinceLastDelinquency, DebtToIncomeRatio, \
    HomeOwnership, LoanPurpose, IncomeValidated, PublicRecordsOnFile, EarliestCreditLine, CreditGrade, EmploymentLength, \
    InquiriesLast6Months, AnnualIncome, TotalCreditLines, LoanData, SqliteLoanData, \
    RevolvingLineUtilization, State, WordsInDescription, utilities, StubFilter
from LoanEnum import *

BackTestFilters = {
    LOAN_ENUM_rowid: StubFilter.StubFilter,
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

ConversionFilters = copy.copy(BackTestFilters)

__all__ = []
__version__ = 1.1
__date__ = '2013-05-29'
__updated__ = '2014-06-14'

DEBUG = 0
TESTRUN = 0
PROFILE = 0

class ProfileGuidedOptimization(object):
    def __init__(self, f, initial_value):
        self.f = f
        self.initial_value = initial_value
        self.current_value = initial_value
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
    mate_population = []

    def __init__(self, backtest_filters, lcbt, args):
        """
        :param backtest_filters:
        :param lcbt:
        :param args:
        """
        self.iteration = 0
        self.iteration_time = 0
        self.backtest_filters = backtest_filters
        self.lcbt = lcbt
        self.args = args

        for _ in range(self.args.population_size):
            filters  = []
            mate_filters = []
            for backtest_filter in backtest_filters.values():
                lc_filter = backtest_filter(args)
                lc_filter.set_current(random.randint(0, lc_filter.get_count() - 1))
                filters.append(lc_filter)

                mate_filter = backtest_filter(args)
                mate_filters.append(mate_filter)

            self.population.append(dict(filters=filters))
            self.mate_population.append(dict(filters=mate_filters))

        csv_field_names = []
        for lc_filter in self.population[0]["filters"]:
            csv_field_names.append(lc_filter.get_name())

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
            citizen['result'] = self.lcbt.test(citizen['filters'])
            #sys.stdout.write('.')
        #sys.stdout.write('\n')

    def sort_by_fitness(self):

        def net_apy_cmp(a, b, config_fitness_sort_num_loans=self.args.fitness_sort_size):
            if a['result']['num_loans'] < config_fitness_sort_num_loans:
                a_fit = 0
            else:
                a_fit = int(a['result']['net_apy'] * 100)

            if b['result']['num_loans'] < config_fitness_sort_num_loans:
                b_fit = 0
            else:
                b_fit = int(b['result']['net_apy'] * 100)

            return b_fit - a_fit

        if sys.version_info.major >= 3:
            self.population.sort(key=functools.cmp_to_key(net_apy_cmp))
        else:
            self.population.sort(cmp=net_apy_cmp)

    def print_best(self):

        best_results = self.population[0]['result']

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

    def copy_population(self, from_population, to_population):
        for i in range(len(self.population)):
            for j in range(len(self.population[i]['filters'])):
                to_population[i]['filters'][j].set_current(from_population[i]['filters'][j].get_current())

    def mate(self):
        # Save the elite
        num_elite = int(self.args.elite_rate * self.args.population_size)

        self.copy_population(self.population, self.mate_population)
        final_population = self.mate_population

        mate_size = math.floor(self.args.population_size / 5.0)

        for i in range(num_elite, self.args.population_size):

            filters = final_population[i]['filters']

            for j in range(len(filters)):
                # Mate with 20% of population
                partner = random.randint(0, mate_size)

                filters[j].set_current(self.population[partner]['filters'][j].get_current())

                # Mutate! Once in a blue moon
                if not random.randint(0, 1.0 / self.args.mutation_rate):
                    lc_filter = filters[j]
                    lc_filter.current = random.randint(0, lc_filter.get_count() - 1)

        self.copy_population(final_population, self.population)

    def debug_msg(self, msg):
        if self.args.verbose:
            sys.stdout.write('GA: %s\n' % msg)

    def info_msg(self, msg):
        sys.stdout.write('GA: %s\n' % msg)


class ParallelGATest(GATest):
    def __init__(self, backtest_filters, lcbt, args, work_queue, response_queue):
        GATest.__init__(self, backtest_filters, lcbt, args)

        self.work_queue = work_queue
        self.response_queue = response_queue

        self.range_filters = range(len(backtest_filters))
        self.pgo = ProfileGuidedOptimization(self.calculate, args.work_batch)

    def run(self):
        GATest.run(self)

        # Signal to all workers that we are finished
        for _ in range(self.args.workers):
            self.work_queue.put(None)

    def calculate_fitness(self):
        #self.pgo()
        self.calculate(self.args.work_batch)

    def calculate(self, min_work_messages=100):
        citizen_idx = 0

        # Bundle up the filters
        work_messages = []

        for citizen in self.population:

            filter_values = [citizen['filters'][i].current for i in self.range_filters]

            work_messages.append((citizen_idx, filter_values))

            if len(work_messages) >= int(min_work_messages):
                self.work_queue.put(work_messages)
                work_messages = []

            # citizen['results'] = self.lcbt.test(citizen['filters'])
            # sys.stdout.write('.')

            citizen_idx += 1

        # Send the tail
        if work_messages:
            self.work_queue.put(work_messages)

        # Get all the results
        results_count = 0
        population_count = len(self.population)
        while results_count < population_count:
            self.debug_msg("Population Count: %d Results Count: %d" % (population_count, results_count))
            result_message = self.response_queue.get()
            new_results_count = len(result_message['results'])
            self.debug_msg("Worker[%i] returned %d results" %
                          (result_message['worker'], new_results_count))
            for citizen_idx, result in result_message['results']:
                # sys.stderr.write("results[%d]=%s\n" % (citizen_idx, result))
                self.population[citizen_idx]['result'] = result
            results_count += new_results_count
        self.debug_msg('Calculate complete')


class ZmqGATest(GATest):
    def __init__(self, backtest_filters, lcbt, args):
        GATest.__init__(self, backtest_filters, lcbt, args)

        self.range_filters = range(len(backtest_filters))

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
        self.info_msg('Waiting for all the workers to get setup')
        # Wait for all workers to send a message indicating they are ready
        for idx in range(self.args.workers):
            self.results_receiver.recv_json()
            self.debug_msg('Received Setup confirmation from %d' % idx)
        self.info_msg('Workers are setup')
        self.pgo = ProfileGuidedOptimization(self.calculate, args.work_batch)

    def run(self):
        GATest.run(self)

        # Signal to all workers that we are finished
        self.control_sender.send_unicode(utilities.u("FINISHED"))

    def calculate_fitness(self):
        #self.pgo()
        self.calculate(self.args.work_batch)

    def calculate(self, min_work_messages=100):
        citizen_idx = 0

        # Bundle up the filters
        work_messages = []

        for citizen in self.population:

            filter_values = [citizen['filters'][i].current for i in self.range_filters]

            work_messages.append((citizen_idx, filter_values))

            if len(work_messages) >= int(min_work_messages):
                self.ventilator_send.send_json(work_messages)
                work_messages = []

            # citizen['results'] = self.lcbt.test(citizen['filters'])
            # sys.stdout.write('.')

            citizen_idx += 1

        # Send the tail
        if work_messages:
            self.ventilator_send.send_json(work_messages)

        # Send control message to signal sending back the results
        self.control_sender.send_unicode(utilities.u("SEND_RESULTS"))
        results_receiver = self.results_receiver
        for _ in range(self.args.workers):
            results_message = results_receiver.recv_json()
            self.debug_msg("Worker[%i] returned %d results" %
                          (results_message['worker'], len(results_message['results'])))
            for citizen_idx, result in results_message['results']:
                # sys.stderr.write("results[%d]=%s\n" % (citizen_idx, result))
                self.population[citizen_idx]['result'] = result
        self.info_msg('Calculate complete')


class LCBT:
    def __init__(self, conversion_filters, args, worker_idx):

        # Create each of the filters
        self.filters = [None] * len(conversion_filters)
        for filter_idx, lc_filter in conversion_filters.items():
            #print(filter_idx, lc_filter.name)
            self.filters[filter_idx] = lc_filter(args)

        if args.sqlite:
            self.test = self.test_sqlite
            # Generate the Sqlite Query
            self.sql_query = "SELECT Id FROM Loans WHERE " + ' AND '.join([lc_filter.query for lc_filter in self.filters if lc_filter.query])
            #print(self.sql_query)

            self.named_sql_query = "SELECT Id FROM Loans WHERE " + ' AND '.join([lc_filter.named_query for lc_filter in self.filters if lc_filter.query])

            """
            This ends up being a lot slower
            SELECT * FROM table1 WHERE rowid IN
             (SELECT rowid FROM table1 WHERE a=5
               INTERSECT SELECT rowid FROM table1 WHERE b=11);
            """
            # http://www.sqlite.org/cvstrac/wiki?p=PerformanceTuning
            self.expanded_query = "SELECT Id FROM Loans WHERE Id IN\n (" + '\n INTERSECT '.join(['SELECT Id from Loans WHERE %s' % lc_filter.query for lc_filter in self.filters if lc_filter.query]) + ')'
            #print(self.expanded_query)

            self.loan_data = SqliteLoanData.SqliteLCLoanData(conversion_filters, args, worker_idx)
        else:
            self.test = self.test_python
            self.loan_data = LoanData.LCLoanData(conversion_filters, args, worker_idx)

        self.args = args
        self.worker_idx = worker_idx

    def initialize(self):
        self.loan_data.initialize()

    def test_python(self, filters):
        self.filters = filters
        invested = [loan for loan in self.loan_data.loans if self.consider(loan)]

        return self.loan_data.get_nar(invested)

    def test_sqlite(self, filters):
        #print(self.sql_query)
        params = tuple(lc_filter.get_value() for lc_filter in filters if '?' in lc_filter.query)
        #print([type(param) for param in params])
        self.loan_data.cursor.execute(self.sql_query, params)
        invested = [self.loan_data.loans[row[0]] for row in self.loan_data.cursor.fetchall()]

        #params = dict()
        #for lc_filter in filters:
        #    if '?' in lc_filter.query:
        #        params[lc_filter.name] = lc_filter.get_current()
        ##print(self.named_sql_query)
        #self.loan_data.cursor.execute(self.named_sql_query, params)
        #invested = [self.loan_data.loans[row[0]] for row in self.loan_data.cursor.fetchall()]

        return self.loan_data.get_nar(invested)

    def consider(self, loan):
        for lc_filter in self.filters:
            #if lc_filter.current != lc_filter.size and not lc_filter.apply(loan):
            if not lc_filter.apply(loan):
                return False
        return True

    def debug_msg(self, msg):
        if self.args.verbose:
            sys.stdout.write("Worker[%i] %s\n" % (self.worker_idx, msg))


class ParallelLCBT(LCBT, Process):
    def __init__(self, conversion_filters, args, worker_idx, work_queue, response_queue):
        LCBT.__init__(self, conversion_filters, args, worker_idx)
        Process.__init__(self)
        self.work_queue = work_queue
        self.response_queue = response_queue

    def run(self):
        self.debug_msg("Started")

        # do some initialization here
        self.initialize()

        # do computation
        for work_messages in iter(self.work_queue.get, None):
            # Use data
            self.debug_msg("Received %d work_messages" % len(work_messages))

            answer_message = dict(worker=self.worker_idx)
            results = []

            for citizen_idx, filters in work_messages:
                for i in range(len(self.filters)):
                    self.filters[i].current = filters[i]

                result = self.test(self.filters)
                #sys.stdout.write(str(self.worker_idx))
                results.append((citizen_idx, result))

            answer_message['results'] = results
            self.response_queue.put_nowait(answer_message)

        self.debug_msg("Finished receiving work")


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
        self.control_receiver.setsockopt_string(utilities.zmq.SUBSCRIBE, utilities.u(""))

        # Set up a poller to multiplex the work receiver and control receiver channels
        self.poller = utilities.zmq.Poller()
        self.poller.register(self.work_receiver, utilities.zmq.POLLIN)
        self.poller.register(self.control_receiver, utilities.zmq.POLLIN)

        # Send a message that we are done with the setup and ready to receive work
        answer_message = dict(worker=self.worker_idx, message='READY')
        self.results_sender.send_json(answer_message)

    def run(self):
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

def zmq_worker(worker_idx, args):
    random.seed(args.seed)
    lcbt = ZmqLCBT(ConversionFilters, args, worker_idx)
    lcbt.initialize()
    lcbt.setup_zmq()
    lcbt.run()

def mp_worker(worker_idx, args, work_queue, response_queue):
    random.seed(args.seed)
    lcbt = ParallelLCBT(ConversionFilters, args, worker_idx, work_queue, response_queue)
    lcbt.start()

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

    #----------------------------------------------------------------------------------------------------------------------
    enable_workers = (cpu_count() > 1)
    #----------------------------------------------------------------------------------------------------------------------

    # Setup argument parser
    parser = ArgumentParser(description=program_license, formatter_class=RawDescriptionHelpFormatter)
    parser.add_argument("-v", "--verbose", dest="verbose", action="count",
                        help="set verbosity level [default: %(default)s]")
    parser.add_argument('-V', '--version', action='version', version=program_version_message)
    parser.add_argument('-g', '--grades', default='ABCDEF',
                        help="String with the allowed credit grades [default: %(default)s]")
    parser.add_argument('-s', '--seed', default=100, help="Random Number Generator Seed [default: %(default)s]")
    parser.add_argument('-d', '--data',
                        default="https://www.lendingclub.com/fileDownload.action?file=LoanStatsNew.csv&type=gen",
                        help="Download path for the notes data file [default: %(default)s]")
    parser.add_argument('-l', '--stats', default="LoanStatsNew.csv",
                        help="Input Loan Stats CSV file [default: %(default)s]")
    parser.add_argument('-c', '--csvresults', default="lc_best.csv",
                        help="Output best results CSV file [default: %(default)s]")
    parser.add_argument('-p', '--population_size', default=512, type=int, help="population size [default: %(default)s]")
    parser.add_argument('-i', '--iterations', default=4096, type=int, help="how many Genetic Algorithm iterations to perform [default: %(default)s]")
    parser.add_argument('-e', '--elite_rate', default=0.05, type=float, help="elite rate [default: %(default)s]")
    parser.add_argument('-m', '--mutation_rate', default=0.05, type=float, help="mutation rate [default: %(default)s]")
    parser.add_argument('-k', '--check', default=False, action='store_true', help="checking mode: open the CSV results file and filter the loans into a separate file [default: %(default)s]")
    parser.add_argument('-r', '--checkresults', default="LoanStatsNewFiltered.csv", help="file name for the filtered results used during checking mode [default: %(default)s]")
    parser.add_argument('-z', '--zmq', default=False, action='store_true', help="Use zmq libraries for multi-processing [default: %(default)s]")
    parser.add_argument('-q', '--sqlite', default=1, type=int, help="Use sqlite as the core processing engine for the backtesting [default: %(default)s]")
    parser.add_argument('-f', '--fitness_sort_size', default=1000, type=int,
                        help="number of loans to limit the fitness sort size, the larger the longer and more optimal solution [default: %(default)s]")
    parser.add_argument('-y', '--young_loans_in_days', default=3 * 30, type=int,
                        help="filter young loans if they are younger than specified number of days [default: %(default)s]")
    parser.add_argument('-w', '--workers', default=enable_workers * cpu_count(), type=int,
                        help="number of workers defaults to the number of cpu cores [default: %(default)s]")
    parser.add_argument('-b', '--work_batch', default=75, type=int,
                        help="size of work batch size to give to each worker [default: %(default)s]")

    # Process arguments
    args = parser.parse_args()

    if (args.population_size < args.workers):
        args.workers = 1
        enable_workers = 0

    if args.workers > 0 and args.zmq:
        enable_zmq = utilities.check_for_zmqpy() or utilities.check_for_pyzmq()
    else:
        enable_zmq = False

    random.seed(args.seed)

    if os.path.exists(args.stats):
        sys.stdout.write("Using %s as the data file\n" % args.stats)
    else:
        utilities.download_data(args.data, args.stats)

    if enable_workers:
        if enable_zmq:
            lcbt = ZmqLCBT(ConversionFilters, args, worker_idx=-1)
            lcbt.initialize()
            for worker_idx in range(args.workers):
                Process(target=zmq_worker, args=(worker_idx, args)).start()
            ga_test = ZmqGATest(BackTestFilters, lcbt, args)
            ga_test.run()
        else:
            # only need this one to initialize the data
            lcbt = LCBT(ConversionFilters, args, worker_idx=-1)
            lcbt.initialize()
            work_queue = MultiProcessingQueue()
            response_queue = MultiProcessingQueue()
            for worker_idx in range(args.workers):
                mp_worker(worker_idx, args, work_queue, response_queue)
            ga_test = ParallelGATest(BackTestFilters, lcbt, args, work_queue, response_queue)
            ga_test.run()
    else:
        lcbt = LCBT(ConversionFilters, args, worker_idx=-1)
        lcbt.initialize()
        ga_test = GATest(BackTestFilters, lcbt, args)
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
