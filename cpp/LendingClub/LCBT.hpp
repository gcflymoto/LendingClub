/*
Created on July 27, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LCBT_HPP__
#define __LC_LCBT_HPP__

#include <vector>
#include <string>
#include <cmath>
#include <future>
#include "Loan.hpp"
#include "LoanData.hpp"

namespace lc
{

class LCBT
{
public:
    LCBT(const LoanTypeVector& conversion_filters, const int worker_idx) :
        _conversion_filters(conversion_filters),
        _args(LCArguments::Get()),
        _filters(conversion_filters.size()),
        _loan_data(NULL),
        _worker_idx(worker_idx),
        _start_range(0),
        _end_range(0)
    {
        _verbose = _args["verbose"].as<bool>();
    }

    virtual void initialize()
    {
        // This is the non-parallel version of initialize
        //
        _loan_data = new LoanData(_conversion_filters, _worker_idx);
        _loan_data->initialize();
        _start_range = 0;
        _end_range = _loan_data->num_loans();
    }

    virtual void process_loans(FilterPtrVector& test_filters)
    {        
        //
        // What this algorithm does is convert all relations to a linear code so that we don't have to call
        // the filters themselves, or call their virtual functions. This also allows us to iterate over the 
        // individual loan data values here in this tight loop
        // 
        // The magic is to convert filter relations like 
        //
        // A >  B
        // A >= B
        // A <  B
        // A <= B
        //

        _invested.clear();

        auto first_filter = &(test_filters[0]);

        auto& loans = _loan_data->get_loans();
        const Loan* loan = &(loans[_start_range]);

        auto num_filters = test_filters.size();

        for (auto i = _start_range; i < _end_range; ++i, ++loan) {
            Filter** f = first_filter;
            size_t j = num_filters;
            for (; j != 0 && ((*f)->apply(*loan)); --j, ++f) {
                ; // everything is done in the for loop declaration
            }
            if (j == 0) {
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
    const LoanTypeVector&                   _conversion_filters;
    const Arguments&                        _args;
    bool                                    _verbose;
    FilterPtrVector                         _filters;
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
    ParallelWorkerLCBT(const LoanTypeVector& conversion_filters, const int worker_idx) : LCBT(conversion_filters, worker_idx),
        _args(LCArguments::Get())
    {
    }

    virtual void initialize() {
        // Worker threads do not need to initialize the data        
        //
        auto& loans = get_loan_data().get_loans();
        auto num_loans = loans.size();

        // Split up the work among all the threads
        // we use ceil here to so that at worst case end range is a bit over the number of loans
        //
        auto work_size = static_cast<size_t>(std::ceil(static_cast<double>(num_loans) / _args["workers"].as<unsigned>()));

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

private:
    const Arguments&                        _args;
    FilterPtrVector*                        _test_filters;
};


class ParallelManagerLCBT : public LCBT
{
public:
    ParallelManagerLCBT(const LoanTypeVector& conversion_filters) : LCBT(conversion_filters, -1),
        _conversion_filters(conversion_filters),
        _num_workers(LCArguments::Get()["workers"].as<unsigned>())
    {
    }

    virtual void initialize()
    {
        // Initialize the data
        LCBT::initialize();

        LoanData* loan_data = const_cast<LoanData*>(&get_loan_data());

        _threads.resize(_num_workers);

        for (size_t i = 0; i < _num_workers; ++i) {            
            _threads[i] = new LCBT_ThreadData;

            auto& td = *(_threads[i]);

            // Create all the workers
            //
            td.lcbt = new ParallelWorkerLCBT(_conversion_filters, i);

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
            // For each worker grab the invested loan information to determine the nar
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
    const LoanTypeVector&                   _conversion_filters;
    const unsigned                          _num_workers;
    LoanValueVector                         _parallel_invested;
    std::vector<LCBT_ThreadData*>           _threads;
};

};

#endif // __LC_LCBT_HPP__
