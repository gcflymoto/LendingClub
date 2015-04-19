/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_REPORTER_HPP__
#define __LC_REPORTER_HPP__

#include <string>
#include <iostream>
#include "Types.hpp"

namespace lc
{
    class Reporter {
    public:
        Reporter() : worker_set(false) {}

        void worker(const LCString& new_worker_id)
        {
            worker_set = true;
            worker_id = new_worker_id;
        }

        void info_msg(const LCString& message)
        {
            if (worker_set) {
                std::cout << "Worker[" << worker_id << "]: " << message << '\n';
                worker_set = false;
            }
            else {
                std::cout << message << '\n';
            }            
        }

        void error_msg(const LCString& message)
        {
            if (worker_set) {
                std::cerr << "Worker[" << worker_id << "] Error: " << message << '\n';
                worker_set = false;
            }
            else {
                std::cerr << "Error: " << message << '\n';
            }
        }

        void exception_msg(const char* exception)
        {
            if (worker_set) {
                std::cerr << "Worker[" << worker_id << "] Exception: " << exception << '\n';
                worker_set = false;
            }
            else {
                std::cerr << "Exception: " << exception << '\n';
            }
        }

        static Reporter& Worker(const LCString& worker_id) {
            auto& reporter = Get();
            reporter.worker(worker_id);
            return reporter;
        }

        static Reporter& Worker(const int worker_idx) {
            auto& reporter = Get();
            reporter.worker(boost::lexical_cast<LCString>(worker_idx));
            return reporter;
        }

        static Reporter& ErrorMsg(const LCString& message)
        {
            auto& reporter = Get();
            reporter.error_msg(message);
            return reporter;
        }

        static Reporter& ExceptionMsg(const char* exception)
        {
            auto& reporter = Get();
            reporter.exception_msg(exception);
            return reporter;
        }

        static Reporter& InfoMsg(const LCString& message)
        {
            auto& reporter = Get();
            reporter.info_msg(message);
            return reporter;
        }

        static Reporter& Get()
        {
            static Reporter reporter;
            return reporter;
        }

    private:
        bool            worker_set;
        LCString        worker_id;
    };

}

#endif // __LC_REPORTER_HPP__