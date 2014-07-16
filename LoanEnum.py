"""
Created on May 30, 2013

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
"""

# These enums are used for conversions and backtesting

LOAN_ENUM_purpose = 0
LOAN_ENUM_desc_word_count = 1
LOAN_ENUM_earliest_credit_line = 2
LOAN_ENUM_addr_state = 3
LOAN_ENUM_revol_utilization = 4
LOAN_ENUM_debt_to_income_ratio = 5
LOAN_ENUM_funded_amnt = 6
LOAN_ENUM_grade = 7
LOAN_ENUM_delinq_2yrs = 8
LOAN_ENUM_home_ownership = 9
LOAN_ENUM_inq_last_6mths = 10
LOAN_ENUM_mths_since_last_delinq = 11
LOAN_ENUM_pub_rec = 12
LOAN_ENUM_emp_length = 13
LOAN_ENUM_annual_income = 14
LOAN_ENUM_income_validated = 15
LOAN_ENUM_acc_open_past_24mths = 16

# These enums are only used for conversions

LOAN_ENUM_total_acc = 17
LOAN_ENUM_loan_status = 18
LOAN_ENUM_issue_datetime = 19
LOAN_ENUM_number_of_payments = 20
LOAN_ENUM_installment = 21
LOAN_ENUM_int_rate = 22
LOAN_ENUM_total_pymnt = 23
LOAN_ENUM_out_prncp = 24
LOAN_ENUM_out_prncp_inv = 25
LOAN_ENUM_profit = 26
LOAN_ENUM_principal = 27
LOAN_ENUM_lost = 28
LOAN_ENUM_defaulted = 29

LOAN_ENUM_rowid = 30

LOAN_ENUM_SIZE = 31

LOAN_ENUM_default_data = [0] * LOAN_ENUM_SIZE

