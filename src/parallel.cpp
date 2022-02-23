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
#include <memory>

#define PRINT_STEPS

using std::vector;
using std::cout;
using std::endl;

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


double calculateOnceIntegral(IntegrationParameters integrationParameters, bool calcFirstTime, int threadsNum, const vector<int>& threadsSplitsNum) {
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
#ifdef PRINT_STEPS
        std::cout << "beginX | endX -> " << integrationParametersThread.beginX <<  " | " << integrationParametersThread.endX << std::endl;
        std::cout << "splitsNumX for this part -> " << integrationParametersThread.splitsNumX << std::endl;

#endif
        threads[i] = std::thread(calculatePartIntegral, integrationParametersThread, calcFirstTime, std::ref(partIntegralResults[i]));
    }
#ifdef PRINT_STEPS
    cout << "------------------------" << std::endl;
#endif

    for (std::thread & th : threads)
    {
        if (th.joinable())
            th.join();
    }

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
        integrationParameters.splitsNumY *= 2;

        newIntegralVal = previousIntegralVal / 4 +
                calculateOnceIntegral(integrationParameters, calcFirstTime, threadsNum, threadsSplitsNum);

        absError = std::abs(newIntegralVal - previousIntegralVal);
        relError = std::abs(newIntegralVal - previousIntegralVal) / newIntegralVal;
#ifdef PRINT_STEPS
        std::cout << "splits num: " << (integrationParameters.splitsNumX *= 2) << " | " << integrationParameters.splitsNumY << '\n'
                  << "abs error: " << absError << '\n'
                  << "rel error: " << relError << '\n'
                  << "------------------------" << std::endl;
#endif
        previousIntegralVal = newIntegralVal;
    }

    return IntegrationResult{newIntegralVal, absError, relError};
}

int main(int argc, char *argv[]) {
    std::unique_ptr<command_line_options_t> command_line_options;
    try {
        command_line_options = std::make_unique<command_line_options_t>(argc, argv);
    }
    catch (std::exception &ex) {
        cout << ex.what() << endl;
        return 1;
    }

    IntegrationParameters integrationParameters{
        command_line_options->x_start,
        command_line_options->x_end,
        command_line_options->y_start,
        command_line_options->y_end,
        command_line_options->init_steps_x,
        command_line_options->init_steps_y,
        command_line_options->abs_err,
        command_line_options->rel_err
    };
    int threadsNum = command_line_options->n_threads;

    cout << threadsNum << endl;

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