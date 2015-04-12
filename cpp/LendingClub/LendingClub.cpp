#include "Arguments.hpp"
#include "LCBT.hpp"
#include "LCGA.hpp"

//https://github.com/noporpoise/BitArray/
//https://www.biicode.com/pablodev/pablodev/bitscan/master

using namespace lc;
using namespace std;

int lcmain(int argc, char* argv[])
{
    auto parser = LCArguments(argc, argv);
    if (parser.parse()) {
        return 1;
    }
    if (parser.check()) {
        return 1;
    }

    auto& args = LCArguments::Get();

    srand(args["seed"].as<unsigned>());

    LoanTypeVector conversion_filters;
    // The order of these must must must match the layout of the Loan struct, as the inner core algorithm makes some assumptions
    //
    conversion_filters.push_back(Loan::OPEN_ACCOUNTS);
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
    conversion_filters.push_back(Loan::TERM);

    LoanTypeVector backtest_filters = conversion_filters;

    // This is the non-parallel version of initialize
    //
    auto loan_data = new LoanData(conversion_filters, -1);
    loan_data->initialize();

    LCBT* lcbt = NULL;

    unsigned workers = args["workers"].as<unsigned>();
    if (workers > 1) {
        lcbt = new ParallelManagerLCBT();
    }
    else {
        lcbt = new LCBT(-1);
    }

    lcbt->set_loan_data(loan_data);
    lcbt->initialize();

    GATest ga_test(backtest_filters, *lcbt);
    ga_test.run();

    return 0;
}
