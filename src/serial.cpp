// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <cmath>
#include "options_parser.h"
#include "methods.h"
#include "integration_structures.h"

//#define PRINT_STEPS

double deDjongFunc(double x1, double x2) {
    double sum = 0.002;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            sum += 1 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6) + pow((x2 - 16 * i), 6));
        }
    }
    return pow(sum, -1);
}

double
calculatePartIntegral(double beginX, double endX, double beginY, double endY, int splitsNumX, int splitsNumY, bool calcFirstTime) {
    int jStartValue = 0, jIncrement = 1;
    if (!calcFirstTime) {
        jStartValue = 1;
        jIncrement = 2;
    }
    double deltaX = (endX - beginX) / splitsNumX,
            deltaY = (endY - beginY) / splitsNumY;
    double deltaS = deltaX * deltaY;
    double funcValuesSum = 0;
    for (int i = 0; i < splitsNumX; i += 2) {
        double curX = beginX + deltaX * i;
        for (int j = jStartValue; j < splitsNumY; j += jIncrement) {
            double curY = beginY + deltaY * j;
            funcValuesSum += deDjongFunc(curX, curY);
        }
    }
    for (int i = 1; i < splitsNumX; i += 2) {
        double curX = beginX + deltaX * i;
        for (int j = 0; j < splitsNumY; ++j) {
            double curY = beginY + deltaY * j;
            funcValuesSum += deDjongFunc(curX, curY);
        }
    }
    double integralValue = funcValuesSum * deltaS;
    return integralValue;
}

IntegrationResult calculateIntegral(double beginX, double endX, double beginY, double endY, int splitsNumX, int splitsNumY) {
    double absEps = 0.05, relEps = 0.0000002;
    double absError = 99999, relError = 99999;

    double previousIntegralVal = calculatePartIntegral(beginX, endX, beginY, endY, splitsNumX, splitsNumY, true),
            newIntegralVal = 0;

    while (absError > absEps and relError > relEps) {
        splitsNumX *= 2, splitsNumY *= 2;

        newIntegralVal = previousIntegralVal / 4 +
                         calculatePartIntegral(beginX, endX, beginY, endY, splitsNumX, splitsNumY, false);

        absError = std::abs(newIntegralVal - previousIntegralVal);
        relError = std::abs(newIntegralVal - previousIntegralVal) / newIntegralVal;
#ifdef PRINT_STEPS
        std::cout << "splits num: " << splitsNumX << " | " << splitsNumY << '\n'
                  << "abs error: " << absError << '\n'
                  << "rel error: " << relError << '\n'
                  << "------------------------" << std::endl;
#endif
        previousIntegralVal = newIntegralVal;
    }

    return IntegrationResult{newIntegralVal, absError, relError};
}

int main(int argc, char *argv[]) {
    double beginX = -50, endX = 50, beginY = -50, endY = 50;
    int splitsNumX = 2, splitsNumY = 100;

    auto time_start = get_current_time_fenced();

    IntegrationResult integrationResult = calculateIntegral(beginX, endX, beginY, endY, splitsNumX, splitsNumY);

    auto time_finish = get_current_time_fenced();

    std::cout.precision(9);
#ifdef PRINT_STEPS
    std::cout << "variant: ";
#endif
    std::cout << 1 << std::endl;
#ifdef PRINT_STEPS
    std::cout << "integral: ";
#endif
    std::cout << integrationResult.integralValue << std::endl;
#ifdef PRINT_STEPS
    std::cout << "abs error: ";
#endif
    std::cout << integrationResult.absError << std::endl;
#ifdef PRINT_STEPS
    std::cout << "rel error: ";
#endif
    std::cout << integrationResult.relError << std::endl;
#ifdef PRINT_STEPS
    std::cout << "time: ";
#endif
    std::cout << to_us(time_finish - time_start) << std::endl;


    return 0;
}
