/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_ERROR_HPP__
#define __LC_ERROR_HPP__

#include <string>
#include <iostream>
#include <exception>

namespace lc
{
    namespace error
    {
        struct Base : std::exception
        {
            virtual void format_error_message() const = 0;

            const char*what() const throw() {
                format_error_message();
                return error_message.c_str();
            }

            std::string error_message;
        };

        struct WithFileName
        {
            WithFileName() {}

            void set_file_name(std::string& new_file_name) 
            {
                file_name = new_file_name;
            }

            std::string file_name;
        };

        struct WithFileLine {
            WithFileLine() {
                file_line = -1;
            }

            void set_file_line(int new_file_line){
                file_line = new_file_line;
            }

            int file_line;
        };

        struct WithField {
            WithField() {}

            void set_field_name(std::string& new_field_name){
                field_name = new_field_name;
            }

            std::string field_name;
        };

    }
}

#endif // __LC_ERROR_HPP__