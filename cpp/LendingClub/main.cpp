#include <vector>
#include <thread>
#include <boost/algorithm/string.hpp>
#include "Arguments.hpp"
#include "LCBT.hpp"
#include "LCGA.hpp"

using namespace lc;
using namespace std;

int lcmain(int argc, char* argv[])
{
    // Declare the supported options.
    boost::program_options::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "Produce help message")
        ("verbose,v", "Set verbosity level")
        ("version,V", boost::program_options::value<string>()->default_value("0.7"), "Program version")
        ("grades,g", boost::program_options::value<string>()->default_value("ABCDEFG"), "String with the allowed credit grades")
        ("seed,s", boost::program_options::value<unsigned>()->default_value(100), "Random Number Generator Seed")
        ("data,d", boost::program_options::value<string>()->default_value("https://www.lendingclub.com/fileDownload.action?file=LoanStatsNew.csv&type=gen"), "Download path for the notes data file")
        ("stats,l", boost::program_options::value<string>()->default_value("LoanStatsNew.csv"), "Input Loan Stats CSV file")
        ("csvresults,c", boost::program_options::value<string>()->default_value("lc_best.csv"), "Output best results CSV file")
        ("population_size,p", boost::program_options::value<unsigned>()->default_value(512), "population size")
        ("iterations,i", boost::program_options::value<unsigned>()->default_value(4096), "how many Genetic Algorithm iterations to perform")
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

    auto& args = LCArguments::Get();

    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, desc), args);
    try {
        boost::program_options::notify(args);
    }
    catch (std::exception& e) {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    if (args.count("help")) {
        cout << desc << '\n';
        return 1;
    }

    // Check that the grades are legal values
    string grades = args["grades"].as<string>();
    boost::erase_all(grades, "ABCDEFG");
    if (!grades.empty()) {
        cout << "Unknown grade classes specified: " << grades << '\n';
        return 1;
    }

    string data = args["data"].as<string>();
    if (data.empty()) {
        cout << "The name of the data URL is empty\n";
        return 1;
    }

    string stats = args["stats"].as<string>();
    if (stats.empty()) {
        cout << "The name of the CSV input data file is empty\n";
        return 1;
    }

    string csvresults = args["csvresults"].as<string>();
    if (csvresults.empty()) {
        cout << "The name of the CSV output results file is empty\n";
        return 1;
    }

    // Check that the population size is greater than the number of workers and a multiple of workers
    // so we can easily split the work
    //
    unsigned workers = args["workers"].as<unsigned>();
    if (workers == 0) {
        cout << "The number of workers must be greater than 0\n";
        return 1;
    }

    unsigned population_size = args["population_size"].as<unsigned>();
    
    if ((workers > population_size) || (population_size % workers != 0)) {
        cout << "The population size: " << population_size << " must be a multiple of the number of workers: " << workers << '\n';
        return 1;
    }

    unsigned iterations = args["iterations"].as<unsigned>();
    if (iterations == 0) {
        cout << "The number of iterations must be greater than 0\n";
        return 1;
    }

    double elite_rate = args["elite_rate"].as<double>();
    if (elite_rate == 0.0 || elite_rate > 1.0) {
        cout << "The elite rate must be greater than 0 and less than 1.0\n";
        return 1;
    }

    double mutation_rate = args["mutation_rate"].as<double>();
    if (mutation_rate == 0.0 || mutation_rate > 1.0) {
        cout << "The mutation rate must be greater than 0 and less than 1.0\n";
        return 1;
    }

    unsigned fitness_sort_size = args["fitness_sort_size"].as<unsigned>();
    if (fitness_sort_size == 0) {
        cout << "The number of fitness sort size must be greater than 0\n";
        return 1;
    }

    unsigned young_loans_in_days = args["young_loans_in_days"].as<unsigned>();
    if (young_loans_in_days == 0) {
        cout << "The number of young loans days must be greater than 0\n";
        return 1;
    }

    unsigned work_batch = args["work_batch"].as<unsigned>();
    if (work_batch == 0) {
        cout << "Work batch must be greater than 0\n";
        return 1;
    }

    srand(args["seed"].as<unsigned>());

    std::vector<Loan::LoanType> conversion_filters;
    conversion_filters.push_back(Loan::ACC_OPEN_PAST_24MTHS);
    conversion_filters.push_back(Loan::FUNDED_AMNT);
    conversion_filters.push_back(Loan::ANNUAL_INCOME);
    conversion_filters.push_back(Loan::GRADE);
    conversion_filters.push_back(Loan::DEBT_TO_INCOME_RATIO);
    conversion_filters.push_back(Loan::DELINQ_2YRS);
    conversion_filters.push_back(Loan::EARLIEST_CREDIT_LINE);
    conversion_filters.push_back(Loan::EMP_LENGTH);
    conversion_filters.push_back(Loan::HOME_OWNERSHIP);
    conversion_filters.push_back(Loan::INCOME_VALIDATED);
    conversion_filters.push_back(Loan::INQ_LAST_6MTHS);
    conversion_filters.push_back(Loan::PURPOSE);
    conversion_filters.push_back(Loan::MTHS_SINCE_LAST_DELINQ);
    conversion_filters.push_back(Loan::PUB_REC);
    conversion_filters.push_back(Loan::REVOL_UTILIZATION);
    conversion_filters.push_back(Loan::ADDR_STATE);
    conversion_filters.push_back(Loan::TOTAL_ACC);
    conversion_filters.push_back(Loan::DESC_WORD_COUNT);

    LCBT lcbt(conversion_filters, -1);
    lcbt.initialize();

    std::vector<Loan::LoanType> backtest_filters = conversion_filters;

    GATest ga_test(backtest_filters, lcbt);
    ga_test.run();

    return 0;
}
