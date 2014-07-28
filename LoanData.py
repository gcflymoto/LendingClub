"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""

import os
import sys
import datetime
import pickle
import csv
import re
import math

import utilities
from LoanEnum import *
import sqlite3

def init_sqlite_db(db_name):
    # Read database to tempfile
    con = sqlite3.connect(db_name)
    tempfile = utilities.stringio()
    for line in con.iterdump():
        tempfile.write('%s\n' % line)
    con.close()
    tempfile.seek(0)

    # Create a database in memory and import from tempfile
    mem_db = sqlite3.connect(":memory:")
    mem_db.cursor().executescript(tempfile.read())
    mem_db.commit()
    return mem_db

__author__ = 'gczajkow'

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

    def info_msg(self, msg):
        sys.stdout.write("Worker[%d] %s\n" % (self.worker_idx, msg))

    def initialize(self):
        # Check serialized file cache.
        csv_mod = os.path.getmtime(self.args.stats)

        pickle_name = "loans_serialized_python%d.bin" % sys.version_info.major

        serialized_mod = os.path.getmtime(pickle_name) if os.path.exists(pickle_name) else 0
        if serialized_mod and serialized_mod > csv_mod:
            self.info_msg("Initializing from %s ..." % pickle_name)
            with open(pickle_name, "rb") as fh:
                self.loans = pickle.load(fh)

            self.info_msg("Initializing sqlite")
            self.create_sqlite_db(self.loans)
            self.info_msg("Initializing done")

        # double cache miss
        elif os.path.exists(self.args.stats):
            self.info_msg("Initializing from %s ..." % self.args.stats)

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

            self.info_msg("Initializing sqlite")
            self.create_sqlite_db(loans)

            self.loans = loans

            self.info_msg("Initializing done. Loans Found:" +
                          str(len(loans)) + " Skipped non-parseable Loans:" + str(self.skipped_loans) +
                          " Skipped Young Loans:" + str(self.young_loans) + " Skipped Removed or Expired Loans:" +
                          str(self.removed_expired_loans))
        else:
            self.info_msg(self.args.stats + " not found")
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
            self.info_msg("Skipping loan, did not find issue_d:" + str(loan))
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

    def create_sqlite_db(self, loans):
        #db_name = "loans_serialized_python%d.db" % sys.version_info.major

        self.con = sqlite3.connect(":memory:")
        self.cursor = self.con.cursor()
        self.cursor.execute("PRAGMA temp_store = MEMORY")
        self.cursor.execute("PRAGMA count_changes = OFF")
        self.cursor.execute("PRAGMA PAGE_SIZE = 4096")
        self.cursor.execute("DROP TABLE IF EXISTS Loans")

        create_sql = "CREATE TABLE Loans("
        insert_sql = "INSERT INTO  Loans VALUES("

        for filter in self.filters:
            create_sql += filter.name + " " + filter.sqlite_type + ", "
            insert_sql += "?, "
        create_sql = create_sql[:-2] + ")"
        #print(create_sql)
        self.cursor.execute(create_sql)

        insert_sql = insert_sql[:-2] + ")"
        #print(insert_sql)
        num_filters = len(self.filters)
        self.cursor.executemany(insert_sql, (loan[:num_filters] for loan in loans))
        self.con.commit()

        for filter in self.filters:
            index_sql = "CREATE INDEX index_%s ON Loans(%s)" % (filter.name, filter.name)
            self.cursor.execute(index_sql)
            self.con.commit()


    def open_sqlite_db(self):
        db_name = "loans_serialized_python%d.db" % sys.version_info.major
        self.info_msg("Initializing sqlite3 %s" % db_name)

        if utilities.check_for_sqlite():
            self.con = init_sqlite_db(db_name)
        else:
            self.con = sqlite3.connect(db_name)
        self.cursor = self.con.cursor()

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
