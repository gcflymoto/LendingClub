/*
Created on July 27, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_LCGA_HPP__
#define __LC_LCGA_HPP__

#include <vector>
#include <string>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <boost/any.hpp>
#include "LCBT.hpp"

namespace lc 
{

class GATest {
    typedef std::vector<std::pair<LoanReturn, std::vector<Filter*>>> PopulationType;
public:
    GATest(
            const std::vector<LCLoan::LoanType>& backtest_filters,
            LCBT& lcbt,
            const Arguments& args
            ) :
            _lcbt(lcbt),
            _args(args),
            _iteration(0),
            _iteration_time(0)
    {
        unsigned population_size = args["population_size"].as<unsigned>();
        _population.reserve(population_size);
        _mate_population.reserve(population_size);
        _iterations = _args["iterations"].as<unsigned>();

        for (unsigned i = 0; i < population_size; ++i) {
            std::vector<Filter*> filters(backtest_filters.size());
            std::vector<Filter*> mate_filters(backtest_filters.size());
            // Create each of the filters and use its conversion utility for normalizing the data
            unsigned j = 0;
            for (auto& filter_type : backtest_filters) {
                std::vector<Filter*>::iterator filter_it = filters.begin() + j;
                construct_filter(filter_type, args, filter_it);
                (*filter_it)->set_current(randint(0, (*filter_it)->get_count() - 1));

                std::vector<Filter*>::iterator mate_filter_it = mate_filters.begin() + j;
                construct_filter(filter_type, args, mate_filter_it);

                ++j;
            }
            _population.push_back(std::make_pair(LoanReturn(), filters));
            _mate_population.push_back(std::make_pair(LoanReturn(), mate_filters));
        }

        assert(population_size > 0);

        _csv_file.open(args["csvresults"].as<std::string>().c_str());

        std::vector<std::string> csv_field_names; 
        for (auto& lc_filter : _population[0].second) {
            _csv_file << lc_filter->get_name() << ',';
        }

        _csv_file << "expected_apy,num_loans,num_defaulted,pct_defaulted,avg_default_loss,net_apy\n";

    }

    void run()
    {
        _iteration_time = _iteration_time.zero();
        
        for (_iteration = 0; _iteration < _iterations; ++_iteration) {
            std::chrono::time_point<std::chrono::system_clock> start = std::chrono::system_clock::now();

            calculate_fitness();
            sort_by_fitness();

            std::chrono::time_point<std::chrono::system_clock> end = std::chrono::system_clock::now();

            _iteration_time += end - start;

            print_best();
            mate();
        }
    }

    void calculate_fitness()
    {
        for (auto& citizen : _population) {
            citizen.first = _lcbt.test(citizen.second);
        }
    }

    struct net_apy_cmp
    {
        net_apy_cmp(unsigned config_fitness_sort_num_loans) : config_fitness_sort_num_loans(config_fitness_sort_num_loans) {}

        inline bool operator() (const std::pair<LoanReturn, std::vector<Filter*>>& a, const std::pair<LoanReturn, std::vector<Filter*>>& b)
        {
            unsigned a_fit = 0;
            if (a.first.num_loans >= config_fitness_sort_num_loans) {
                a_fit = boost::numeric_cast<unsigned>(a.first.net_apy * 100);
            }

            unsigned b_fit = 0;
            if (b.first.num_loans >= config_fitness_sort_num_loans) {
                b_fit = boost::numeric_cast<unsigned>(b.first.net_apy * 100);
            }

            return b_fit < a_fit;
        }

        unsigned config_fitness_sort_num_loans;
    };

    void sort_by_fitness()
    {
        auto config_fitness_sort_num_loans = _args["fitness_sort_size"].as<unsigned>();
        std::sort(_population.begin(), _population.end(), net_apy_cmp(config_fitness_sort_num_loans));
    }

    void print_best() 
    {
        auto best_results = _population[0].first;

        std::string filters = "";
        for (auto& lc_filter : _population[0].second) {
            auto filter_name = lc_filter->get_name();
            auto filter_val_str = lc_filter->get_string_value();
            _csv_file << filter_val_str << ',';
            filters += filter_name + '=' + filter_val_str + ',';
        }

        std::cout << "Best Filter: " << filters << '\n';

        std::cout << "[iteration " << (_iteration + 1) << '/' << _iterations << ' ' << std::setprecision(4) << _iteration_time.count() / (_iteration + 1);
        std::cout << " sec/iter] Matched " << best_results.num_loans << '/' << _lcbt.total_loans() << " loans ";
        std::cout << "(" << best_results.loans_per_month << "/mo.) test at " << std::setprecision(4) << best_results.expected_apy << "% APY. ";
        std::cout << std::setprecision(4) << best_results.num_defaulted << " loans defaulted (" << best_results.pct_defaulted << "%, $";
        std::cout << best_results.avg_default_loss << " avg loss) " << best_results.net_apy << "% net APY\n";
    }

    void copy_population(const PopulationType& from, PopulationType& to)
    {
        unsigned i = 0;
        for (auto& lc_pair : from) {
            unsigned j = 0;
            for (auto& lc_filter : lc_pair.second) {
                to[i].second[j]->set_current(lc_filter->get_current());
                ++j;
            }

            ++i;
        }
    }

    void mate()
    {
        // Save the elite
        auto num_elite = boost::numeric_cast<unsigned>(_args["elite_rate"].as<double>() * _population.size());
        auto mate_size = boost::numeric_cast<unsigned>(std::floor(_population.size() / 5.0));
        auto mutation_possibility = boost::numeric_cast<unsigned>(1.0 / _args["mutation_rate"].as<double>());
        copy_population(_population, _mate_population);

        for (size_t i = num_elite; i < _population.size(); ++i) {

            auto& filters = _mate_population[i].second;

            for (size_t j = 0; j < filters.size(); ++j) {
                // Mate with 20 % of population
                auto partner = randint(0, mate_size);

                filters[j]->set_current(_population[partner].second[j]->get_current());

                // Mutate! Once in a blue moon
                if (!randint(0, mutation_possibility)) {
                    auto& lc_filter = filters[j];
                    lc_filter->set_current(randint(0, lc_filter->get_count() - 1));
                }
            }
        }

        copy_population(_mate_population, _population);
    }

    LCBT&                                                       _lcbt;
    const Arguments&						                    _args;
    std::vector<Filter*>					                    _filters;
    PopulationType                                              _population;
    PopulationType                                              _mate_population;
    unsigned                                                    _iteration;
    unsigned                                                    _iterations;
    std::chrono::duration<double>                               _iteration_time;
    std::ofstream                                               _csv_file;
};

};

#endif // __LC_LCGA_HPP__
