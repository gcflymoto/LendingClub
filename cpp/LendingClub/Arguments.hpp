/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_ARGUMENTS_HPP__
#define __LC_ARGUMENTS_HPP__

#include <vector>
#include <thread>
#include <iostream>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>
#include "Types.hpp"
#include "Reporter.hpp"

namespace lc
{
typedef boost::program_options::variables_map Arguments;

class LCArguments
{
public:
    LCArguments(int argc, char* argv[]) : desc("Allowed options"), argc(argc), argv(argv) {}

    bool parse()
    {
        // Declare the supported options.
        desc.add_options()
            ("help,h", "Produce help message")
            ("verbose,v", boost::program_options::bool_switch()->default_value(false), "Verbose mode")
            ("version,V", boost::program_options::value<LCString>()->default_value("2.5"), "Program version")
            ("grades,g", boost::program_options::value<LCString>()->default_value("A,B,C,D,E,F,G"), "Comma separated list of credit grades to test")
            ("states,a", boost::program_options::value<LCString>()->default_value("CA,AZ,FL,GA,IL,MD,NV,TX,NY"), "Comma separated list of states to test")
            ("terms,t", boost::program_options::value<LCString>()->default_value("36,60"), "Comma separated list of loan terms to test")
            ("seed,s", boost::program_options::value<unsigned>()->default_value(100), "Random Number Generator Seed")
            ("data,d", boost::program_options::value<LCString>()->default_value("https://resources.lendingclub.com/LoanStats3a.csv.zip,https://resources.lendingclub.com/LoanStats3b.csv.zip,https://resources.lendingclub.com/LoanStats3c.csv.zip"), "Comma separated download paths for the notes data files")
            ("stats,l", boost::program_options::value<LCString>()->default_value("LoanStats3a_securev1.csv,LoanStats3b_securev1.csv,LoanStats3c_securev1.csv"), "Comma separated list of input Loan Stats CSV files")
            ("csvresults,c", boost::program_options::value<LCString>()->default_value("lc_best.csv"), "Output best results CSV file")
            ("population_size,p", boost::program_options::value<unsigned>()->default_value(512), "population size")
            ("iterations,i", boost::program_options::value<unsigned>()->default_value(4096), "how many Genetic Algorithm iterations to perform")
            ("elite_rate,e", boost::program_options::value<double>()->default_value(0.05), "elite rate")
            ("mutation_rate,m", boost::program_options::value<double>()->default_value(0.05), "mutation rate")
            ("fitness_sort_size,f", boost::program_options::value<unsigned>()->default_value(1000), "number of loans to limit the fitness sort size, the larger the longer and more optimal solution")
            ("young_loans_in_days,y", boost::program_options::value<unsigned>()->default_value(3 * 30), "filter young loans if they are younger than specified number of days")
            ("workers,w", boost::program_options::value<unsigned>()->default_value(std::thread::hardware_concurrency()), "number of workers defaults to the number of cpu cores")
            ("work_batch,b", boost::program_options::value<unsigned>()->default_value(75), "size of work batch size to give to each worker")
            ;

        auto& args = LCArguments::Get();

        boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), args);
        try {
            boost::program_options::notify(args);
        }
        catch (std::exception& e) {
            Reporter::ExceptionMsg(e.what());
            return 1;
        }

        return 0;
    }

    bool check() 
    {
        auto& args = LCArguments::Get();

        if (args.count("help")) {
            std::stringstream ss;
            ss << desc;
            Reporter::InfoMsg(ss.str());
            return 1;
        }

        // Check that the grades are legal values
        LCString grades = args["grades"].as<LCString>();
        boost::erase_all(grades, "A");
        boost::erase_all(grades, "B");
        boost::erase_all(grades, "C");
        boost::erase_all(grades, "D");
        boost::erase_all(grades, "E");
        boost::erase_all(grades, "F");
        boost::erase_all(grades, "G");
        boost::erase_all(grades, ",");
        if (!grades.empty()) {
            Reporter::ErrorMsg("Unknown grade classes specified: " + grades);
            return 1;
        }

        LCString data = args["data"].as<LCString>();
        if (data.empty()) {
            Reporter::ErrorMsg("The name of the data URL argument is empty");
            return 1;
        }

        LCString stats = args["stats"].as<LCString>();
        if (stats.empty()) {
            Reporter::ErrorMsg("The name of the CSV input data file argument is empty");
            return 1;
        }

        LCString csvresults = args["csvresults"].as<LCString>();
        if (csvresults.empty()) {
            Reporter::ErrorMsg("The name of the CSV output results file argument is empty");
            return 1;
        }

        LCString test_terms = args["terms"].as<LCString>();
        StringVector terms;
        boost::split(terms, test_terms, boost::is_any_of(","));
        for (auto& term : terms) {
            if (term != "36" && term != "60") {
                Reporter::ErrorMsg("not a valid loan term: " + term);
                return 1;
            }
        }

        // Check that the population size is greater than the number of workers and a multiple of workers
        // so we can easily split the work
        //
        unsigned workers = args["workers"].as<unsigned>();
        if (workers == 0) {
            Reporter::ErrorMsg("The number of workers argument must be greater than 0");
            return 1;
        }

        unsigned population_size = args["population_size"].as<unsigned>();

        if (workers > population_size) {
            std::stringstream ss;
            ss << "The population size argument: " << population_size << " must be a multiple of the number of workers: " << workers;
            Reporter::ErrorMsg(ss.str());
            return 1;
        }

        unsigned iterations = args["iterations"].as<unsigned>();
        if (iterations == 0) {
            Reporter::ErrorMsg("The number of iterations argument must be greater than 0");
            return 1;
        }

        double elite_rate = args["elite_rate"].as<double>();
        if (elite_rate == 0.0 || elite_rate >= 1.0) {
            Reporter::ErrorMsg("The elite rate argument must be greater than 0 and less than 1.0");
            return 1;
        }

        double mutation_rate = args["mutation_rate"].as<double>();
        if (mutation_rate == 0.0 || mutation_rate >= 1.0) {
            Reporter::ErrorMsg("The mutation rate argument must be greater than 0 and less than 1.0");
            return 1;
        }

        unsigned fitness_sort_size = args["fitness_sort_size"].as<unsigned>();
        if (fitness_sort_size == 0) {
            Reporter::ErrorMsg("The number of fitness sort size argument must be greater than 0");
            return 1;
        }

        /*
        unsigned young_loans_in_days = args["young_loans_in_days"].as<unsigned>();
        if (young_loans_in_days == 0) {
            Reporter::ErrorMsg("The number of young loans days must be greater than 0");
            return 1;
        }
        */

        unsigned work_batch = args["work_batch"].as<unsigned>();
        if (work_batch == 0) {
            Reporter::ErrorMsg("Work batch argument must be greater than 0");
            return 1;
        }

        return 0;
    }
    static Arguments& Get()
    {
        static Arguments args;
        return args;
    }    
    private:
        boost::program_options::options_description desc;
        int                                         argc;
        char**                                      argv;
};

};

#endif // __LC_ARGUMENTS_HPP__
