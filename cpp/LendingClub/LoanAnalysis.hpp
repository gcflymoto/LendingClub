/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LOANALYSIS_HPP__
#define __LC_LOANALYSIS_HPP__

#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>
#include "Arguments.hpp"
#include "Types.hpp"
#include "Loan.hpp"
#include "Filters.hpp"

namespace lc
{
    class LoanAnalysis
    {
    public:
        LoanAnalysis(const LoanData& loan_data) :
            _loans(loan_data.get_loans()), 
            _loan_infos(loan_data.get_loan_infos()), 
            _args(LCArguments::Get()),
            _worker_idx(-1) 
        {
            auto _now = boost::posix_time::second_clock::local_time();
            boost::gregorian::date_duration delta(_args["young_loans_in_days"].as<unsigned>());

            _last_date_for_full_month_for_volume = _now.date() - delta;
        }

        virtual void info_msg(const LCString& msg) const
        {
            Reporter::Worker(_worker_idx).InfoMsg(msg);
        }

        virtual void error_msg(const LCString& msg) const
        {
            Reporter::Worker(_worker_idx).ErrorMsg(msg);
            exit(-1);
        }

        virtual void show_average(Loan::LoanType loan_value_type) const
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

            // Step 3. Ignore the top and bottom 2% outliers
            //
            size_t start_index = static_cast<size_t>(data.size() * 0.02);
            size_t end_index = static_cast<size_t>(data.size() * 0.98);

            // Step 4. Find the sum
            //
            LoanValue sum = 0;
            for (size_t i = start_index; i < end_index; ++i) {
                sum += data[i];
            }

            // Step 5. Find the average that can be used a filter constraint and the average as a double
            FilterValue avg = sum / (end_index - start_index + 1);
            auto average = static_cast<double>(sum) / (end_index - start_index + 1);

            // Step 6. Push the value into a value vector to be added to the filter
            FilterValueVector filter_value;
            filter_value.push_back(avg);

            // Step 7. Construct the filter and sets its constraint value to the average
            FilterPtrVector filter(1);
            construct_filter(loan_value_type, filter.begin());
            const FilterValueVector& static_options = filter[0]->get_options();
            filter[0]->set_options(&filter_value);

            // Step 8. Show the average value
            info_msg("Avg " + filter[0]->get_name() + "=" + boost::lexical_cast<LCString>(average)+" filter is " + filter[0]->get_string_value());

            filter[0]->set_options(&static_options);
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

    private:
        LoanVector	        					_loans;
        LoanInfoVector                          _loan_infos;
        const Arguments&                        _args;
        const int								_worker_idx;
        boost::gregorian::date					_last_date_for_full_month_for_volume;
    };

}

#endif // __LC_LOANALYSIS_HPP__
