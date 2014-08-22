/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LOANDATA_HPP__
#define __LC_LOANDATA_HPP__

#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>
#include "Arguments.hpp"
#include "Loan.hpp"
#include "Filters.hpp"
#include "csv.h"

namespace lc {

class LCLoanData {
public:
    LCLoanData(
        const std::vector<LCLoan::LoanType>& conversion_filters,
        const int worker_idx) : 
            _args(LCArguments::Get()),
            _filters(conversion_filters.size()),
            _worker_idx(worker_idx),
            _row(0),
            _skipped_loans(0),
            _young_loans(0),
            _removed_expired_loans(0)
            { 
        _loans.reserve(350000);
        // Create each of the filters and use its conversion utility for normalizing the data
        for (auto& filter_type : conversion_filters) {
            _filters.resize(filter_type+1);
            std::vector<Filter*>::iterator filter_it = _filters.begin() + filter_type;
            construct_filter(filter_type, filter_it);
        }

        _now = boost::posix_time::second_clock::local_time(); //use the clock 
        boost::gregorian::date_duration delta(_args["young_loans_in_days"].as<unsigned>() + 30);

        _last_date_for_full_month_for_volume = _now.date() - delta;
    }

    void info_msg(const std::string& msg)
    {
        std::cout << "Worker[" << _worker_idx << "] " << msg << '\n';
    }

    virtual void mid_stage_initialization() {}

    struct RawLoan
    {
        std::string acc_open_past_24mths;
        std::string funded_amnt;
        std::string annual_inc;
        std::string grade;
        std::string dti;
        std::string delinq_2yrs;
        std::string earliest_cr_line;
        std::string emp_length;
        std::string home_ownership;
        std::string is_inc_v;
        std::string inq_last_6mths;
        std::string purpose;
        std::string mths_since_last_delinq;
        std::string pub_rec;
        std::string revol_util;
        std::string addr_state;
        std::string total_acc;
        std::string desc;
        std::string loan_status;
        std::string issue_d;
        std::string term;
        std::string installment;
        std::string int_rate;
        std::string total_pymnt;
        std::string out_prncp;
        std::string out_prncp_inv;
        std::string total_rec_int;
        std::string total_rec_prncp;        
        std::string to_str() const 
        {
            std::string str;
            str += "acc_open_past_24mths=" + acc_open_past_24mths + ',';
            str += "funded_amnt=" + funded_amnt + ',';
            str += "annual_inc=" + annual_inc + ',';
            str += "grade=" + grade + ',';
            str += "dti=" + dti + ',';
            str += "delinq_2yrs=" + delinq_2yrs + ',';
            str += "earliest_cr_line=" + earliest_cr_line + ',';
            str += "home_ownership=" + home_ownership + ',';
            str += "is_inc_v=" + is_inc_v + ',';
            str += "inq_last_6mths=" + inq_last_6mths + ',';
            str += "purpose=" + purpose + ',';
            str += "mths_since_last_delinq=" + mths_since_last_delinq + ',';
            str += "pub_rec=" + pub_rec +',';
            str += "revol_util=" + revol_util + ',';
            str += "addr_state=" + addr_state + ',';
            str += "total_acc=" + total_acc + ',';
            str += "desc=" + desc + ',';
            str += "loan_status=" + loan_status + ',';
            str += "issue_d=" + issue_d + ',';
            str += "term=" + term + ',';
            str += "installment=" + installment + ',';
            str += "int_rate=" + int_rate + ',';
            str += "total_pymnt=" + total_pymnt + ',';
            str += "out_prncp=" + out_prncp + ',';
            str += "out_prncp_inv=" + out_prncp_inv + ',';
            str += "total_rec_int=" + total_rec_int + ',';
            str += "total_rec_prncp=" + total_rec_prncp;
            return str;
        }
    };

    virtual void initialize() 
    {
        boost::filesystem::path stats_file_path = _args["stats"].as<std::string>();
        if (boost::filesystem::exists(stats_file_path)) {
            info_msg("Initializing from " + stats_file_path.string());
            
            io::CSVReader<28, io::trim_chars<' ', '\t'>, io::double_quote_escape<',', '\"'>, io::throw_on_overflow, io::single_line_comment<'N','L','\0'> > in(stats_file_path.string().c_str());
            in.read_header(io::ignore_extra_column, "acc_open_past_24mths", "funded_amnt", "annual_inc", "grade", 
                "dti", "delinq_2yrs", "earliest_cr_line", "emp_length", "home_ownership", "is_inc_v", "inq_last_6mths",
                "purpose", "mths_since_last_delinq", "pub_rec", "revol_util", "addr_state", "total_acc", "desc",
                "loan_status", "issue_d", "term", "installment", "int_rate", "total_pymnt", "out_prncp", "out_prncp_inv",
                "total_rec_int", "total_rec_prncp");

            RawLoan raw_loan;

            while (in.read_row(
                raw_loan.acc_open_past_24mths,
                raw_loan.funded_amnt,
                raw_loan.annual_inc,
                raw_loan.grade,
                raw_loan.dti,
                raw_loan.delinq_2yrs, 
                raw_loan.earliest_cr_line, 
                raw_loan.emp_length, 
                raw_loan.home_ownership,
                raw_loan.is_inc_v,
                raw_loan.inq_last_6mths,
                raw_loan.purpose, 
                raw_loan.mths_since_last_delinq, 
                raw_loan.pub_rec,
                raw_loan.revol_util, 
                raw_loan.addr_state,
                raw_loan.total_acc,
                raw_loan.desc,
                raw_loan.loan_status,
                raw_loan.issue_d,
                raw_loan.term,
                raw_loan.installment, 
                raw_loan.int_rate,
                raw_loan.total_pymnt,
                raw_loan.out_prncp,
                raw_loan.out_prncp_inv,
                raw_loan.total_rec_int,
                raw_loan.total_rec_prncp)) {

                bool parsed_row_ok = check_loan(raw_loan);
                if (!parsed_row_ok) {
                    continue;
                }

                LCLoan loan;
                
                bool parsed_loan_ok = normalize_loan_data(raw_loan, loan);
                if (parsed_loan_ok) {
                    // Assign the rowid of the loan to be the current last index in the loans list
                    loan.rowid = _loans.size();
                    _loans.push_back(loan);
                }
            }

            info_msg("Initializing from " + stats_file_path.string() + " done.");

        } else {
            info_msg("error: " + stats_file_path.string() + " not found");
            exit(-1);
        }
    }

    virtual bool check_loan(RawLoan& loan)
    {
        // SKip loans without a loan status or funded amount
        if (loan.loan_status.empty()) {
            ++_skipped_loans;
            return false;
        }

        if (loan.funded_amnt.empty()) {
            ++_skipped_loans;
            return false;
        }

        if (loan.issue_d.empty()) {
            ++_skipped_loans;
            return false;
        }

        // Only look at loans with a valid issue date
        boost::gregorian::date issue_d(boost::gregorian::from_simple_string(loan.issue_d));		
        if (issue_d.is_not_a_date()) {
            info_msg("Skipping loan, did not find issue_d:" + loan.to_str());
            ++_skipped_loans;
            return false;
        }

        // Ignore loans that are too young for consideration
        boost::gregorian::date_duration young_loans_in_days(_args["young_loans_in_days"].as<unsigned>());
        boost::gregorian::date young_date = _now.date() - young_loans_in_days;

        if (young_date < issue_d) {
            ++_young_loans;
            return false;
        }

        // Ignore loans that didn't event start
        if ((loan.loan_status == "Removed") || (loan.loan_status == "Expired")) {
            ++_removed_expired_loans;
            return false;
        }
        return true;
    }
                  
    virtual bool normalize_loan_data(const RawLoan& raw_loan, LCLoan& loan)
    {
        loan.acc_open_past_24mths = _filters[LCLoan::ACC_OPEN_PAST_24MTHS]->convert(raw_loan.acc_open_past_24mths);
        loan.funded_amnt = _filters[LCLoan::FUNDED_AMNT]->convert(raw_loan.funded_amnt);
        loan.annual_income = _filters[LCLoan::ANNUAL_INCOME]->convert(raw_loan.annual_inc);
        loan.grade = _filters[LCLoan::GRADE]->convert(raw_loan.grade);
        loan.debt_to_income_ratio = _filters[LCLoan::DEBT_TO_INCOME_RATIO]->convert(raw_loan.dti);
        loan.delinq_2yrs = _filters[LCLoan::DELINQ_2YRS]->convert(raw_loan.delinq_2yrs);
        loan.earliest_credit_line = _filters[LCLoan::EARLIEST_CREDIT_LINE]->convert(raw_loan.earliest_cr_line);
        loan.emp_length = _filters[LCLoan::EMP_LENGTH]->convert(raw_loan.emp_length);
        loan.home_ownership = _filters[LCLoan::HOME_OWNERSHIP]->convert(raw_loan.home_ownership);
        loan.income_validated = _filters[LCLoan::INCOME_VALIDATED]->convert(raw_loan.is_inc_v);
        loan.inq_last_6mths = _filters[LCLoan::INQ_LAST_6MTHS]->convert(raw_loan.inq_last_6mths);
        loan.purpose = _filters[LCLoan::PURPOSE]->convert(raw_loan.purpose);
        loan.mths_since_last_delinq = _filters[LCLoan::MTHS_SINCE_LAST_DELINQ]->convert(raw_loan.mths_since_last_delinq);
        loan.pub_rec = _filters[LCLoan::PUB_REC]->convert(raw_loan.pub_rec);
        loan.revol_utilization = _filters[LCLoan::REVOL_UTILIZATION]->convert(raw_loan.revol_util);
        loan.addr_state = _filters[LCLoan::ADDR_STATE]->convert(raw_loan.addr_state);
        loan.total_acc = _filters[LCLoan::TOTAL_ACC]->convert(raw_loan.total_acc);
        loan.desc_word_count = _filters[LCLoan::DESC_WORD_COUNT]->convert(raw_loan.desc);

        loan.loan_status = raw_loan.loan_status;
        loan.issue_datetime = boost::gregorian::date(boost::gregorian::from_simple_string(raw_loan.issue_d));		

        if (raw_loan.term == " 36 months") {
            loan.number_of_payments = 36;
        }
        else if (raw_loan.term == " 60 months") {
            loan.number_of_payments = 60;
        } else {
            std::cout << "unknown number of payments: " << raw_loan.term << "expecting 36 or 60, skipping\n";
            return false;
        }

        loan.installment = strtod(raw_loan.installment.c_str(), nullptr);

        std::size_t found = raw_loan.int_rate.find_first_not_of(" ");
        loan.int_rate = strtod(raw_loan.int_rate.substr(found, raw_loan.int_rate.length() - 1 - found).c_str(), nullptr);
        loan.total_pymnt = strtod(raw_loan.total_pymnt.c_str(), nullptr);
        loan.out_prncp = strtod(raw_loan.out_prncp.c_str(), nullptr);
        loan.out_prncp_inv = strtod(raw_loan.out_prncp_inv.c_str(), nullptr);
        double total_received_interest = strtod(raw_loan.total_rec_int.c_str(), nullptr);
        double total_received_principal = strtod(raw_loan.total_rec_prncp.c_str(), nullptr);

        double defaulted_amount = 0.0;
        if ((loan.loan_status == "Charged Off") || (loan.loan_status == "Default")) {
            defaulted_amount = ((loan.number_of_payments * loan.installment) - 
                                (total_received_interest + total_received_principal)) * 0.99;
            loan.lost = defaulted_amount;
            loan.defaulted = 1;
        } else {
            loan.lost = 0.0;
            loan.defaulted = 0;
        }

        double loan_profit = 0.0, loan_principal = 0.0, loan_lost = 0.0;
        unsigned elapsed = loan.number_of_payments;
        double balance = static_cast<double>(loan.funded_amnt);
        double monthly_payment = loan.installment;
        double rate = loan.int_rate;
        double ratio = 25.0 / balance;
        double payments = 0.0;
        while (elapsed > 0) {
            --elapsed;
            // Interest and service charge for the whole loan (not just me)
            double interest = balance * rate / 1200.0;
            double service = 0.01 * monthly_payment;
            payments += monthly_payment;
            if (loan.defaulted && payments > loan.total_pymnt) {
                loan_profit -= defaulted_amount * ratio;
                loan_lost += defaulted_amount * ratio;
                break;
            }

            // Compute my ratio of the profit
            loan_profit += (interest - service) * ratio;
            loan_principal += balance * ratio;
            balance -= monthly_payment;
        }

        // sys.stderr.write("Calculated profit=%.2f principal=%.2f loss=%.2f ... ")
        // sys.stderr.write("csv total_received_interest=%.2f total_received_principal=%.2f loss=%.2f\n" %
        // (loan_profit, loan_principal, loan_lost, total_received_interest,
        // total_received_principal, loan_array[LOAN_ENUM_lost]))
        loan.profit = loan_profit;
        loan.principal = loan_principal;
        loan.lost = loan_lost;
        return true;
    }

    LoanReturn get_nar(const std::vector<unsigned>& invested)
    {
        unsigned defaulted = 0, per_month = 0;
        double profit = 0.0, principal = 0.0, lost = 0.0, rate = 0.0;

        for (auto idx : invested) {
            profit += _loans[idx].profit;
            principal += _loans[idx].principal;
            lost += _loans[idx].lost;
            defaulted += _loans[idx].defaulted;
            rate += _loans[idx].int_rate;
            // Count loan volume
            if ((_loans[idx].issue_datetime.year() == _last_date_for_full_month_for_volume.year()) &&
                (_loans[idx].issue_datetime.month() == _last_date_for_full_month_for_volume.month())) {
                ++per_month;
            }
        }

        LoanReturn loan_return;
        loan_return.num_loans = invested.size();

        if (loan_return.num_loans > 0) {

            if (principal == 0.0) {
                loan_return.net_apy = 0.0;
            }
            else {
                // Calculate the Net APR
                loan_return.net_apy = 100.0 * (pow(1.0 + profit / principal, 12) - 1.0);
            }

            loan_return.expected_apy = rate / loan_return.num_loans;
            loan_return.pct_defaulted = 100.0 * defaulted / loan_return.num_loans;
            loan_return.avg_default_loss = (defaulted > 0) ? (lost / defaulted) : 0.0;
            loan_return.loans_per_month = per_month;
            loan_return.num_defaulted = defaulted;
        }
        return loan_return;
    }

    const std::vector<LCLoan>& get_loans() const 
    {
        return _loans;
    }

    unsigned total_loans() const
    {
        return _loans.size();
    }

private:
        const Arguments&						_args;
        std::vector<Filter*>					_filters;
        const int								_worker_idx;
        unsigned								_row;
        unsigned								_skipped_loans;
        unsigned								_young_loans;
        unsigned								_removed_expired_loans;
        boost::gregorian::date					_last_date_for_full_month_for_volume;		
        std::vector<std::string>                _labels;
        std::vector<LCLoan>						_loans;
        boost::posix_time::ptime				_now;
};

};

#endif // __LC_LOANDATA_HPP__
