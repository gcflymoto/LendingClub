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
#include "Types.hpp"
#include "Loan.hpp"
#include "Filters.hpp"
#include "csv.h"

namespace lc
{

class LoanData
{
public:
    LoanData(
        const LoanTypeVector& conversion_filters,
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
        _loan_infos.reserve(350000);
        
        // Create each of the filters and use its conversion utility for normalizing the data
        //
        for (auto& filter_type : conversion_filters) {
            _filters.resize(filter_type + 1);
            FilterPtrVector::iterator filter_it = _filters.begin() + filter_type;
            construct_filter(filter_type, filter_it);
        }

        _now = boost::posix_time::second_clock::local_time();
        boost::gregorian::date_duration delta(_args["young_loans_in_days"].as<unsigned>());

        _last_date_for_full_month_for_volume = _now.date() - delta;
    }

    virtual void info_msg(const LCString& msg) const
    {
        std::cout << "Worker[" << _worker_idx << "] " << msg << '\n';
    }

    virtual void error_msg(const LCString& msg) const
    {
        std::cerr << "Worker[" << _worker_idx << "] error: " << msg << '\n';
        exit(-1);
    }

    virtual void mid_stage_initialization() {}

    struct RawLoan
    {
        LCString open_acc;
        LCString funded_amnt;
        LCString annual_inc;
        LCString grade;
        LCString dti;
        LCString delinq_2yrs;
        LCString earliest_cr_line;
        LCString emp_length;
        LCString home_ownership;
        LCString is_inc_v;
        LCString inq_last_6mths;
        LCString purpose;
        LCString mths_since_last_delinq;
        LCString pub_rec;
        LCString revol_util;
        LCString addr_state;
        LCString total_acc;
        LCString desc;
        LCString loan_status;
        LCString issue_d;
        LCString term;
        LCString installment;
        LCString int_rate;
        LCString total_pymnt;
        LCString out_prncp;
        LCString out_prncp_inv;
        LCString total_rec_int;
        LCString total_rec_prncp;        
        LCString to_str() const 
        {
            LCString str;
            str += "OPEN_ACCOUNTS=" + open_acc + ',';
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
        std::vector<LCString> stats_paths;
        boost::split(stats_paths, _args["stats"].as<LCString>(), boost::is_any_of(","));

        for (auto& stats_path : stats_paths) {

            boost::filesystem::path stats_file_path = stats_path;
            if (boost::filesystem::exists(stats_file_path)) {
                info_msg("Initializing from " + stats_file_path.string());

                io::CSVReader<28, io::trim_chars<' ', '\t'>, io::double_quote_escape<',', '\"'>, io::throw_on_overflow, io::single_line_comment<'N', 'L', '\0'> > in(stats_file_path.string().c_str());
                try
                {
                    in.read_header(io::ignore_extra_column, "open_acc", "funded_amnt", "annual_inc", "grade",
                        "dti", "delinq_2yrs", "earliest_cr_line", "emp_length", "home_ownership", "is_inc_v", "inq_last_6mths",
                        "purpose", "mths_since_last_delinq", "pub_rec", "revol_util", "addr_state", "total_acc", "desc",
                        "loan_status", "issue_d", "term", "installment", "int_rate", "total_pymnt", "out_prncp", "out_prncp_inv",
                        "total_rec_int", "total_rec_prncp");
                }
                catch (io::error::base& err)
                {
                    error_msg(err.what());
                }

                RawLoan raw_loan;

                while (true) {
                    try {
                        bool result = in.read_row(
                            raw_loan.open_acc,
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
                            raw_loan.total_rec_prncp);

                        if (!result) {
                            break;
                        }
                    }
                    catch (io::error::too_few_columns&) {
                        std::stringstream ss;
                        ss << "missing column while parsing row " << in.get_file_line() << " in file " 
                            << stats_file_path.string() << " skipping.";
                        info_msg(ss.str());
                        continue;
                    }
                    
                    try
                    {
                        bool parsed_row_ok = check_loan(raw_loan);
                        if (!parsed_row_ok) {
                            continue;
                        }
                    }
                    catch (std::exception &err)
                    {
                        error_msg(err.what());
                    }

                    Loan loan;
                    LoanInfo loan_info;

                    bool parsed_loan_ok = normalize_loan_data(raw_loan, loan, loan_info);
                    if (parsed_loan_ok) {
                        // Assign the rowid of the loan to be the current last index in the loans list
                        //
                        loan.rowid = _loans.size();
                        _loans.push_back(loan);
                        _loan_infos.push_back(loan_info);
                    }
                }

                info_msg("Initializing from " + stats_file_path.string() + " done.");
                find_average(Loan::OPEN_ACCOUNTS);
                find_average(Loan::FUNDED_AMNT);
                find_average(Loan::ANNUAL_INCOME);
                find_average(Loan::DEBT_TO_INCOME_RATIO);
                find_average(Loan::DELINQ_2YRS);
                find_average(Loan::EARLIEST_CREDIT_LINE);
                find_average(Loan::EMP_LENGTH);
                find_average(Loan::INQ_LAST_6MTHS);
                find_average(Loan::MTHS_SINCE_LAST_DELINQ);
                find_average(Loan::REVOL_UTILIZATION);
                find_average(Loan::TOTAL_ACC);
                find_average(Loan::DESC_WORD_COUNT);
            }
            else {
                error_msg(stats_file_path.string() + " not found");
            }
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
        // We hack this a bit as issue_d has the format of "Dec-2011" so parse it as 1-Dec-2011
        boost::gregorian::date issue_d(boost::gregorian::from_uk_string("1-" + loan.issue_d));
        if (issue_d.is_not_a_date()) {
            info_msg("Skipping loan, did not find issue_d:" + loan.to_str());
            ++_skipped_loans;
            return false;
        }

        // Ignore loans that are too young for consideration
        //
        boost::gregorian::date_duration young_loans_in_days(_args["young_loans_in_days"].as<unsigned>());
        boost::gregorian::date young_date = _now.date() - young_loans_in_days;

        if (young_date < issue_d) {
            ++_young_loans;
            return false;
        }

        // Ignore loans that didn't event start
        //
        if ((loan.loan_status == "Removed") || (loan.loan_status == "Expired")) {
            ++_removed_expired_loans;
            return false;
        }
        return true;
    }

    virtual void find_average(Loan::LoanType loan_value_type) const
    {
        LoanValueVector data;
        data.reserve(_loans.size());

        // Step 1. Pull all the values in a new list
        //
        for (auto& loan : _loans) {
            const LoanValue* lv = &(loan.rowid);
            // Move the pointer from the struct variable in the loan to the one we are looking to average
            //
            lv += loan_value_type;
            data.push_back(*lv);
        }

        // Step 2. Sort the values
        //
        std::sort(data.begin(), data.end());

        // Step 3. Ignore the top and bottom 5% outliers
        //
        size_t start_index = static_cast<size_t>(data.size() * 0.05);
        size_t end_index = static_cast<size_t>(data.size() * 0.95);

        // Step 4. Find the sum
        //
        LoanValue sum = 0;
        for (size_t i = start_index; i < end_index; ++i) {
            sum += data[i];
        }

        FilterPtrVector filter(1);
        construct_filter(loan_value_type, filter.begin());
        FilterValueVector filter_value;
        FilterValue avg = sum / (end_index - start_index + 1);
        filter_value.push_back(avg);


        const FilterValueVector& static_options = filter[0]->get_options();
        filter[0]->set_options(&filter_value);

        info_msg("Avg " + filter[0]->get_name() + "=" + boost::lexical_cast<LCString>(static_cast<double>(sum) / (end_index - start_index + 1)) + " filter is " + filter[0]->get_string_value());

        filter[0]->set_options(&static_options);
    }

    virtual bool normalize_loan_data(const RawLoan& raw_loan, Loan& loan, LoanInfo& loan_info)
    {
        loan.open_acc = _filters[Loan::OPEN_ACCOUNTS]->convert(raw_loan.open_acc);
        loan.funded_amnt = _filters[Loan::FUNDED_AMNT]->convert(raw_loan.funded_amnt);
        loan.annual_income = _filters[Loan::ANNUAL_INCOME]->convert(raw_loan.annual_inc);
        loan.grade = _filters[Loan::GRADE]->convert(raw_loan.grade);
        loan.debt_to_income_ratio = _filters[Loan::DEBT_TO_INCOME_RATIO]->convert(raw_loan.dti);
        loan.delinq_2yrs = _filters[Loan::DELINQ_2YRS]->convert(raw_loan.delinq_2yrs);
        loan.earliest_credit_line = _filters[Loan::EARLIEST_CREDIT_LINE]->convert(raw_loan.earliest_cr_line);
        loan.emp_length = _filters[Loan::EMP_LENGTH]->convert(raw_loan.emp_length);
        loan.home_ownership = _filters[Loan::HOME_OWNERSHIP]->convert(raw_loan.home_ownership);
        loan.income_validated = _filters[Loan::INCOME_VALIDATED]->convert(raw_loan.is_inc_v);
        loan.inq_last_6mths = _filters[Loan::INQ_LAST_6MTHS]->convert(raw_loan.inq_last_6mths);
        loan.purpose = _filters[Loan::PURPOSE]->convert(raw_loan.purpose);
        loan.mths_since_last_delinq = _filters[Loan::MTHS_SINCE_LAST_DELINQ]->convert(raw_loan.mths_since_last_delinq);
        loan.pub_rec = _filters[Loan::PUB_REC]->convert(raw_loan.pub_rec);
        loan.revol_utilization = _filters[Loan::REVOL_UTILIZATION]->convert(raw_loan.revol_util);
        loan.addr_state = _filters[Loan::ADDR_STATE]->convert(raw_loan.addr_state);
        loan.total_acc = _filters[Loan::TOTAL_ACC]->convert(raw_loan.total_acc);
        loan.desc_word_count = _filters[Loan::DESC_WORD_COUNT]->convert(raw_loan.desc);

        loan_info.loan_status = raw_loan.loan_status;
        // 
        // Have to hack the date a bout as it's given in the form of Dec-2011 so normalize it
        // to 1-Dec-2011 so boost can parse it
        // 
        loan_info.issue_datetime = boost::gregorian::date(boost::gregorian::from_uk_string("1-" + raw_loan.issue_d));		

        if (raw_loan.term == " 36 months") {
            loan_info.number_of_payments = 36;
        }
        else if (raw_loan.term == " 60 months") {
            loan_info.number_of_payments = 60;
        } else {
            std::cerr << "unknown number of payments: " << raw_loan.term << "expecting 36 or 60, skipping\n";
            return false;
        }

        loan_info.installment = strtod(raw_loan.installment.c_str(), nullptr);

        std::size_t found = raw_loan.int_rate.find_first_not_of(" ");
        loan_info.int_rate = strtod(raw_loan.int_rate.substr(found, raw_loan.int_rate.length() - 1 - found).c_str(), nullptr);
        loan_info.total_pymnt = strtod(raw_loan.total_pymnt.c_str(), nullptr);
        loan_info.out_prncp = strtod(raw_loan.out_prncp.c_str(), nullptr);
        loan_info.out_prncp_inv = strtod(raw_loan.out_prncp_inv.c_str(), nullptr);
        double total_received_interest = strtod(raw_loan.total_rec_int.c_str(), nullptr);
        double total_received_principal = strtod(raw_loan.total_rec_prncp.c_str(), nullptr);

        double defaulted_amount = 0.0;
        if ((loan_info.loan_status == "Charged Off") || (loan_info.loan_status == "Default")) {
            defaulted_amount = ((loan_info.number_of_payments * loan_info.installment) - 
                                (total_received_interest + total_received_principal)) * 0.99;
            loan_info.lost = defaulted_amount;
            loan_info.defaulted = 1;
        } else {
            loan_info.lost = 0.0;
            loan_info.defaulted = 0;
        }

        double loan_profit = 0.0, loan_principal = 0.0, loan_lost = 0.0;
        unsigned elapsed = loan_info.number_of_payments;
        double balance = static_cast<double>(loan.funded_amnt);
        double monthly_payment = loan_info.installment;
        double rate = loan_info.int_rate;
        double ratio = 25.0 / balance;
        double payments = 0.0;
        while (elapsed > 0) {
            --elapsed;
            // Interest and service charge for the whole loan (not just me)
            //
            double interest = balance * rate / 1200.0;
            double service = 0.01 * monthly_payment;
            payments += monthly_payment;
            if (loan_info.defaulted && payments > loan_info.total_pymnt) {
                loan_profit -= defaulted_amount * ratio;
                loan_lost += defaulted_amount * ratio;
                break;
            }

            // Compute my ratio of the profit
            //
            loan_profit += (interest - service) * ratio;
            loan_principal += balance * ratio;
            balance -= monthly_payment;
        }

        // sys.stderr.write("Calculated profit=%.2f principal=%.2f loss=%.2f ... ")
        // sys.stderr.write("csv total_received_interest=%.2f total_received_principal=%.2f loss=%.2f\n" %
        // (loan_profit, loan_principal, loan_lost, total_received_interest,
        // total_received_principal, loan_array[LOAN_ENUM_lost]))
        loan_info.profit = loan_profit;
        loan_info.principal = loan_principal;
        loan_info.lost = loan_lost;
        return true;
    }

    virtual LoanReturn get_nar(const LoanValueVector& invested) const
    {
        unsigned defaulted = 0, per_month = 0;
        double profit = 0.0, principal = 0.0, lost = 0.0, rate = 0.0;

        for (auto row_id : invested) {
            unsigned idx = static_cast<unsigned>(row_id);
            profit += _loan_infos[idx].profit;
            principal += _loan_infos[idx].principal;
            lost += _loan_infos[idx].lost;
            defaulted += _loan_infos[idx].defaulted;
            rate += _loan_infos[idx].int_rate;
            // Count loan volume
            //
            if ((_loan_infos[idx].issue_datetime.year() == _last_date_for_full_month_for_volume.year()) &&
                (_loan_infos[idx].issue_datetime.month() == _last_date_for_full_month_for_volume.month())) {
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
                //
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

    virtual const LoanVector& get_loans() const
    {
        return _loans;
    }

    virtual unsigned num_loans() const
    {
        return _loans.size();
    }

private:
        const Arguments&                        _args;
        FilterPtrVector                         _filters;
        const int								_worker_idx;
        unsigned								_row;
        unsigned								_skipped_loans;
        unsigned								_young_loans;
        unsigned								_removed_expired_loans;
        boost::gregorian::date					_last_date_for_full_month_for_volume;		
        StringVector                            _labels;
        LoanVector	        					_loans;
        LoanInfoVector                          _loan_infos;
        boost::posix_time::ptime				_now;
};

};

#endif // __LC_LOANDATA_HPP__
