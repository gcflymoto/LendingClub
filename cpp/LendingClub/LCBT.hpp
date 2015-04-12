/*
Created on July 27, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LCBT_HPP__
#define __LC_LCBT_HPP__

#include <cmath>
#include <future>
#include <malloc.h>
#include "Types.hpp"
#include "Loan.hpp"
#include "LoanData.hpp"

namespace lc
{

class LCBT
{
public:
    LCBT(const int worker_idx) :
        _args(LCArguments::Get()),
        _loan_data(NULL),
        _worker_idx(worker_idx),
        _start_range(0),
        _end_range(0)
    {
        _verbose = _args["verbose"].as<bool>();
    }

    virtual void initialize()
    {
        _start_range = 0;
        _end_range = _loan_data->num_loans();
    }

    virtual void old_process_loans(FilterPtrVector& test_filters)
    {
        _invested.clear();

        unsigned num_filters = test_filters.size();

        auto first_filter_p = &(test_filters[0]);

        auto& loans = _loan_data->get_loans();
        const Loan* loan = &(loans[_start_range]);

        for (auto i = _start_range; i < _end_range; ++i, ++loan) {
            Filter** f_p = first_filter_p;
            bool filter_matches_b = true;
            for (auto j = num_filters; filter_matches_b && j != 0; --j, ++f_p) {
                filter_matches_b = (*f_p)->apply(*loan);
            }

            if (filter_matches_b) {
                _invested.push_back(loan->rowid);
            }
        }
    }


    virtual void process_loans(FilterPtrVector& test_filters)
    {        
        //
        // What I want this algorithm to do is convert all relations to a linear code so that we don't have to
        // do the switch. This also allows us to iterate over individual loan data values using vectorization
        // 
        // The magic is to convert filter relations like 
        //
        // A >  B
        // A >= B
        // A <  B
        // A <= B
        // A == B
        // A &  B

        _invested.clear();

        unsigned num_filters = test_filters.size();

        FilterValue* filter_values = static_cast<FilterValue*>(alloca(num_filters * sizeof(FilterValue)));
        assert(filter_values);

        for (unsigned i = 0, size = num_filters; i < size; ++i) {
            filter_values[i] = test_filters[i]->get_value();
        }

        Filter::Relation* relations = static_cast<Filter::Relation*>(alloca(num_filters * sizeof(Filter::Relation)));
        assert(relations);

        for (unsigned i = 0, size = num_filters; i < size; ++i) {
            relations[i] = test_filters[i]->get_relation();
        }

        auto first_relation_p = &(relations[0]);
        auto first_filter_p = &(test_filters[0]);
        auto first_filter_value_p = &(filter_values[0]);

        auto& loans = _loan_data->get_loans();
        const Loan* loan = &(loans[_start_range]);        

        for (auto i = _start_range; i < _end_range; ++i, ++loan) {

            const LoanValue* loan_data_value_p = &(loan->open_acc);
            auto filter_value_p = first_filter_value_p;
            auto relation_value_p = first_relation_p;

            LoanValue filter_matches = 1;
            for (int k = num_filters; filter_matches && k; --k, ++relation_value_p, ++loan_data_value_p, ++filter_value_p) {
                switch (*relation_value_p) {
                case Filter::Relation::LESS_THAN_EQUAL:    filter_matches = ((*loan_data_value_p) <= (*filter_value_p)); break;
                case Filter::Relation::LESS_THAN:          filter_matches = ((*loan_data_value_p) <  (*filter_value_p)); break;
                case Filter::Relation::GREATER_THAN:       filter_matches = ((*loan_data_value_p) >  (*filter_value_p)); break;
                case Filter::Relation::GREATER_THAN_EQUAL: filter_matches = ((*loan_data_value_p) >= (*filter_value_p)); break;
                case Filter::Relation::MASK:               filter_matches = ((*loan_data_value_p) &  (*filter_value_p)); break;
                case Filter::Relation::EQUAL:              filter_matches = ((*loan_data_value_p) == (*filter_value_p)); break;
                case Filter::Relation::NOT_EQUAL:          filter_matches = ((*loan_data_value_p) != (*filter_value_p)); break;
                }
            }

            if (filter_matches) {
                _invested.push_back(loan->rowid);
            }
        }
    }

    virtual LoanReturn test(FilterPtrVector& test_filters)
    {
        process_loans(test_filters);
        return get_loan_data().get_nar(_invested);
    }

    virtual void finish()
    {
        // Called to indicate to this class we are finished processing
    }

    void debug_msg(const LCString& msg)
    {
        if (_verbose) {
            std::cout << "Worker[" << _worker_idx << "] " << msg << '\n';
        }
    }
    unsigned num_loans() const
    {
        return _loan_data->num_loans();
    }
    const LoanData& get_loan_data() const
    {
        return *_loan_data;
    }

    void set_loan_data(LoanData* loan_data)
    {
        _loan_data = loan_data;
    }

    int get_worker_idx() const
    {
        return _worker_idx;
    }

    void set_range(unsigned start_range, unsigned end_range)
    {
        _start_range = start_range;
        _end_range = end_range;
    }

    const LoanValueVector& get_invested() const
    {
        return _invested;
    }    

private:
    //const LoanTypeVector&                   _conversion_filters;
    const Arguments&                        _args;
    bool                                    _verbose;
    const int                               _worker_idx;
    unsigned                                _start_range;
    unsigned                                _end_range;
    LoanData*                               _loan_data;
    LoanValueVector                         _invested;
};

class ParallelWorkerLCBT;

struct LCBT_ThreadData {
    ParallelWorkerLCBT*                     lcbt = nullptr;
    FilterPtrVector*                        test_filters = nullptr;
    std::thread*                            t = nullptr;                // The thread object
    std::condition_variable                 cv;                         // The condition variable to wait for threads
    std::mutex                              m;                          // Mutex used for avoiding data races
    std::promise<bool>                      p;                          // use to indicate completion back to the managing thread
    std::future<bool>                       f;                          // future which holds the promise
    bool                                    stop = false;               // When set, this flag tells the thread that it should exit
    bool                                    process_loans = false;      // When set, this flag tells the thread to process loans
};

class ParallelWorkerLCBT : public LCBT
{
public:
    ParallelWorkerLCBT(const int worker_idx) : LCBT(worker_idx)
    {
    }

    virtual void initialize() {
        LCBT::initialize();

        auto& loans = get_loan_data().get_loans();
        auto num_loans = loans.size();

        // Split up the work among all the threads
        // we use ceil here to so that at worst case end range is a bit over the number of loans
        //
        auto work_size = static_cast<size_t>(std::ceil(static_cast<double>(num_loans) / LCArguments::Get()["workers"].as<unsigned>()));

        auto start_range = work_size * get_worker_idx();

        // This makes sure we do not ever go past the number of loans
        //
        auto end_range = std::min(num_loans, start_range + work_size);

        set_range(start_range, end_range);
    }

    // The thread function executed by each worker thread
    //
    static void work_function(LCBT_ThreadData* thread_data)
    {
        std::unique_lock<std::mutex> l(thread_data->m, std::defer_lock);

        while (true) {
            l.lock();

            // Wait until this thread should process loans or stop
            thread_data->cv.wait(l, [thread_data]() {
                return (thread_data->process_loans || thread_data->stop);
            });

            // If we are done with work, finish up
            //
            if (thread_data->stop) { return; }

            // Clear the flag which tells us to process the loans
            thread_data->process_loans = false;

            // We are done modifying thread state
            l.unlock();

            // Process our set of loans
            thread_data->lcbt->process_loans(*thread_data->test_filters);

            // Indicate back we are done with work by updating the promise
            //
            thread_data->p.set_value(true);
        }
    }
};


class ParallelManagerLCBT : public LCBT
{
public:
    ParallelManagerLCBT() : LCBT(-1),
        _num_workers(LCArguments::Get()["workers"].as<unsigned>())
    {
    }

    virtual void initialize()
    {
        LCBT::initialize();

        LoanData* loan_data = const_cast<LoanData*>(&get_loan_data());

        _threads.resize(_num_workers);

        for (size_t i = 0; i < _num_workers; ++i) {            
            _threads[i] = new LCBT_ThreadData;

            auto& td = *(_threads[i]);

            // Create all the workers
            //
            td.lcbt = new ParallelWorkerLCBT(i);

            // Set their data pointer
            //
            td.lcbt->set_loan_data(loan_data);

            // Initialize the worker
            //
            td.lcbt->initialize();

            // Put the worker in a thread
            //
            td.t = new std::thread(ParallelWorkerLCBT::work_function, &td);
        }
    }

    virtual LoanReturn test(FilterPtrVector& test_filters)
    {
        _parallel_invested.clear();

        // Tell the workers to start processing
        //
        for (unsigned i = 0; i < _num_workers; ++i) 
        {
            auto& td = *(_threads[i]);
            {
                std::unique_lock<std::mutex> l(td.m);
                td.p = std::promise<bool>();
                td.f = td.p.get_future();
                td.test_filters = &test_filters;
                td.process_loans = true;
            }
            td.cv.notify_one();
        }

        // Wait for all workers to finish processing
        //
        for (unsigned i = 0; i < _num_workers; ++i) {
            _threads[i]->f.get();
            
            // For each worker grab the invested loan information to determine the NAR
            //
            auto results = _threads[i]->lcbt->get_invested();
            _parallel_invested.reserve(_parallel_invested.size() + results.size());
            _parallel_invested.insert(_parallel_invested.end(), results.begin(), results.end());
        }
        
        return get_loan_data().get_nar(_parallel_invested);
    }

    virtual void finish()
    {
        // Send stop signal to all threads and join them...
        //
        for (unsigned i = 0; i < _num_workers; ++i)
        {
            auto& td = *(_threads[i]);
            {
                std::unique_lock<std::mutex> l(td.m);
                td.stop = true;
            }
            td.cv.notify_one();
            td.t->join();
        }
    }

private:
    const unsigned                          _num_workers;
    LoanValueVector                         _parallel_invested;
    ThreadDataVector                        _threads;
};

};

#endif // __LC_LCBT_HPP__
