// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include "options_parser.h"
#include "methods.h"
#include "integration_structures.h"
#include <algorithm>

#define PRINT_STEPS

using std::vector;

double deDjongFunc(double x1, double x2) {
    double sum = 0.002;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            sum += 1 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6) + pow((x2 - 16 * i), 6));
        }
    }
    return pow(sum, -1);
}

void
calculatePartIntegral(PartIntegrationParameters partIntegrationParameters, bool calcFirstTime, double &result) {
    double beginX = partIntegrationParameters.beginX,
    endX = partIntegrationParameters.endX,
    beginY = partIntegrationParameters.beginY,
    endY = partIntegrationParameters.endY;
    int splitsNumX = partIntegrationParameters.splitsNumX,
    splitsNumY = partIntegrationParameters.splitsNumY;

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

    result = integralValue;
}


double calculateOnceIntegral(IntegrationParameters integrationParameters, bool calcFirstTime, int threadsNum, vector<int>& threadsSplitsNum) {
    // threads
    std::vector<std::thread> threads(threadsNum);
    std::vector<double> partIntegralResults(threadsNum);

    double partX = (integrationParameters.endX - integrationParameters.beginX) / threadsNum;

    // initial state of structure
    PartIntegrationParameters partIntegrationParameters{
            integrationParameters.beginX,
            integrationParameters.endX,
            integrationParameters.beginY,
            integrationParameters.endY,
            threadsSplitsNum[0],
            integrationParameters.splitsNumY
    };

    for (int i = 0; i < threadsNum; i++) {
        PartIntegrationParameters integrationParametersThread = partIntegrationParameters;
        integrationParametersThread.beginX = partIntegrationParameters.beginX + partX * i;
        integrationParametersThread.endX = partIntegrationParameters.beginX + partX * (i + 1);
        integrationParametersThread.splitsNumX = threadsSplitsNum[i];
        std::cout << "beginX | endX -> " << integrationParametersThread.beginX <<  " | " << integrationParametersThread.endX << std::endl;
        std::cout << "splitsNumX for this part -> " << integrationParametersThread.splitsNumX << std::endl;
        threads[i] = std::thread(calculatePartIntegral, integrationParametersThread, calcFirstTime, std::ref(partIntegralResults[i]));
    }

    for (std::thread & th : threads)
    {
        // If thread Object is Joinable then Join that thread.
        if (th.joinable())
            th.join();
    }

    // Sum up part integrals
    double integralVal = 0;
    for (const auto &n : partIntegralResults)
        integralVal += n;

    return integralVal;
}

vector<int> calculateThreadsSplitsNum(int splitsNum, int threadsNum) {
    vector<int> threadsSplitsNum(threadsNum);
    int eachValue = splitsNum / threadsNum;
    int remain = splitsNum % threadsNum;
    std::fill(threadsSplitsNum.begin(), threadsSplitsNum.begin() + remain, eachValue + 1);
    std::fill(threadsSplitsNum.begin() + remain, threadsSplitsNum.end(), eachValue);
    return threadsSplitsNum;
}

IntegrationResult calculatePreciseIntegral(IntegrationParameters integrationParameters, int threadsNum) {
    vector<int> threadsSplitsNum = calculateThreadsSplitsNum(integrationParameters.splitsNumX, threadsNum);

    bool calcFirstTime = true;
    double previousIntegralVal = calculateOnceIntegral(integrationParameters, calcFirstTime, threadsNum, threadsSplitsNum),
            newIntegralVal = 0;
    calcFirstTime = false;

    double absError = 99999, relError = 99999;
    while (absError > integrationParameters.absEps and relError > integrationParameters.relEps) {
        for (auto& threadSplitsNum: threadsSplitsNum) {
            threadSplitsNum *= 2;
        }
        integrationParameters.splitsNumX *= 2; // for cout only?

        newIntegralVal = previousIntegralVal / 4 +
                calculateOnceIntegral(integrationParameters, calcFirstTime, threadsNum, threadsSplitsNum);

        absError = std::abs(newIntegralVal - previousIntegralVal);
        relError = std::abs(newIntegralVal - previousIntegralVal) / newIntegralVal;
#ifdef PRINT_STEPS
        std::cout << "splits num: " << integrationParameters.splitsNumX << " | " << integrationParameters.splitsNumY << '\n'
                  << "abs error: " << absError << '\n'
                  << "rel error: " << relError << '\n'
                  << "------------------------" << std::endl;
#endif
        previousIntegralVal = newIntegralVal;
    }

    return IntegrationResult{newIntegralVal, absError, relError};
}

void fn1(int threadNumber) {
    for (int i = 0; i < 100000; ++i) {
        std::cout << "fn-" << threadNumber << " :"<< i <<  std::endl;
    }
}

int main(int argc, char *argv[]) {
    double beginX = -50, endX = 50, beginY = -50, endY = 50;
    int splitsNumX = 2, splitsNumY = 100;
    double absEps = 0.05, relEps = 0.0000002;
    int threadsNum = 1;

    IntegrationParameters integrationParameters{beginX, endX, beginY, endY, splitsNumX, splitsNumY, absEps, relEps};

    auto time_start = get_current_time_fenced();

    IntegrationResult integrationResult = calculatePreciseIntegral(integrationParameters, threadsNum);

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