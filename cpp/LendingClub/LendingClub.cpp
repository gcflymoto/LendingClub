// LendingClub.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <vector>
#include <thread>
#include <boost/program_options.hpp>
#include "LCBT.hpp"

using namespace lc;
using namespace std;

int _tmain(int argc, _TCHAR* argv[])
{
	// Declare the supported options.
	boost::program_options::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Produce help message")
		("verbose,v", "Set verbosity level")
		("version,V", boost::program_options::value<string>()->default_value("0.0"), "Program version")
		("grades,g", boost::program_options::value<string>()->default_value("ABCDEF"), "String with the allowed credit grades")
		("seed,s", boost::program_options::value<int>()->default_value(100), "Random Number Generator Seed")
		("data,d", boost::program_options::value<string>()->default_value("https://www.lendingclub.com/fileDownload.action?file=LoanStatsNew.csv&type=gen"), "Download path for the notes data file")
	    ("stats,l", boost::program_options::value<string>()->default_value("LoanStatsNew.csv"), "Input Loan Stats CSV file")
		("csvresults,c", boost::program_options::value<string>()->default_value("lc_best.csv"), "Output best results CSV file")
		("population_size,p", boost::program_options::value<int>()->default_value(512), "population size")
		("iterations,i", boost::program_options::value<int>()->default_value(4096), "how many Genetic Algorithm iterations to perform")
		("elite_rate,e", boost::program_options::value<double>()->default_value(0.05), "elite rate")
		("mutation_rate,m", boost::program_options::value<double>()->default_value(0.05), "mutation rate")
		("check,k", boost::program_options::value<bool>()->default_value(false), "checking mode: open the CSV results file and filter the loans into a separate file")
		("checkresults,r", boost::program_options::value<string>()->default_value("LoanStatsNewFiltered.csv"), "Output best results CSV file")
		("zmq,z", boost::program_options::value<bool>()->default_value(false), "Use zmq libraries for multi-processing")
		("fitness_sort_size,f", boost::program_options::value<unsigned>()->default_value(1000), "number of loans to limit the fitness sort size, the larger the longer and more optimal solution")
		("young_loans_in_days,y", boost::program_options::value<unsigned>()->default_value(3*30), "filter young loans if they are younger than specified number of days")
		("workers,w", boost::program_options::value<unsigned>()->default_value(std::thread::hardware_concurrency()), "number of workers defaults to the number of cpu cores")
		("work_batch,b", boost::program_options::value<unsigned>()->default_value(75), "size of work batch size to give to each worker")
	;
	
	Arguments args;
	boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), args);
	boost::program_options::notify(args);    

	if (args.count("help")) {
		cout << desc << "\n";
		return 1;
	}

	std::vector<LCLoan::LoanType> conversion_filters;
	conversion_filters.push_back(LCLoan::ACC_OPEN_PAST_24MTHS);
	conversion_filters.push_back(LCLoan::FUNDED_AMNT);

	LCBT lcbt = LCBT(conversion_filters, args, -1);
	lcbt.initialize();

	return 0;
}

