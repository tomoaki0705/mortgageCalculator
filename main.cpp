#include <iostream>
#include "getopt.h"

const double kEps = 1./100000.;

int64_t computeRemainder(const uint32_t totalIteration, double interest, uint64_t totalDebt, uint64_t monthlyPayment)
{
    int64_t currentDebt = totalDebt;
    for ( uint32_t i = 0;i < totalIteration;i++ )
    {
        int64_t currentCharge = (int64_t)(currentDebt * interest / 12.);
        currentDebt = currentDebt + currentCharge - monthlyPayment;
    }
    return currentDebt;
}

uint64_t bisectAmount(const uint32_t totalIteration, double interest, uint64_t totalDebt, uint64_t upperBound, uint64_t lowerBound)
{
    if (upperBound - lowerBound <= 2)
    {
        return upperBound;
    }
    uint64_t currentPayment = (upperBound + lowerBound) >> 1;
    int64_t remainder = computeRemainder(totalIteration, interest, totalDebt, currentPayment);
    if ( remainder < 0 )
    {
        return bisectAmount(totalIteration, interest, totalDebt, currentPayment, lowerBound);
    }
    else if (remainder <= (int64_t)(totalDebt * kEps))
    {
        return currentPayment;
    }
    else
    {
        return bisectAmount(totalIteration, interest, totalDebt, upperBound, currentPayment);
    }
}

int main(int argc, char**argv)
{
    uint64_t totalDebt = 10000000;     // ten-million JPY
    double interest = 0.01;            // this stands for 1%
    uint32_t totalIteration = 35 * 12; // this stands for 35 years
    try
    {
        auto results = GetOpt::getopt(argc, argv
            , "d|debt"   ,  "total amount of debt",          &totalDebt
            , "i|interest", "actual interest (0.01 for 1%)", &interest
            , "c|count",    "total iteration count",         &totalIteration
            );
        auto begin = results.args.begin();
        auto end = results.args.end();
        if ( begin == end || ++begin == end)
        {
            results.result.helpWanted = true;
        }
        if(results.result.helpWanted)
        {
            defaultGetoptPrinter("Usage: ", results.result.options);
            return -1;
        }
    }
    catch(GetOpt::GetOptException e)
    {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    auto amount = bisectAmount(totalIteration, interest, totalDebt, totalDebt, 0);
    std::cout << "Total debt  :" << totalDebt << std::endl;
    std::cout << "Total count :" << totalIteration << " monthly payment " << std::endl;
    std::cout << "Monthly fee :" << amount << std::endl;
	return 0;
}
