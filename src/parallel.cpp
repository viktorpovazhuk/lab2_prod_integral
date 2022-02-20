// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
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

void
calculatePartIntegral(IntegrationParameters integrationParameters, bool calcFirstTime, double &result) {
    double& beginX = integrationParameters.beginX,
    endX = integrationParameters.endX,
    beginY = integrationParameters.beginY,
    endY = integrationParameters.endY;
    int& splitsNumX = integrationParameters.splitsNumX,
    splitsNumY = integrationParameters.splitsNumY;

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

double calculateOnceIntegral(IntegrationParameters integrationParameters, bool calcFirstTime, int threadsNum) {
    // threads
    //    double partX = (endX - beginX) / threadsNum;
//    std::vector<std::thread> threads;
//    double integralValue = calculate_integral(beginX, endX, beginY, endY, splitsNum);
//    std::cout << integralValue;

    std::vector<std::thread> threads(threadsNum);
    std::vector<double> partialVolume(threadsNum);

    double volumePart1 = 0, volumePart2 = 0;
    IntegrationParameters integrationParameters1 = integrationParameters;
    integrationParameters1.beginX = -50;
    integrationParameters1.endX = 0;
    IntegrationParameters integrationParameters2 = integrationParameters;
    integrationParameters2.beginX = 0;
    integrationParameters2.endX = 50;

    std::thread thread1 = std::thread(calculatePartIntegral, integrationParameters1, calcFirstTime, std::ref(volumePart1));
    std::thread thread2 = std::thread(calculatePartIntegral, integrationParameters2, calcFirstTime, std::ref(volumePart2));

    thread1.join();
    thread2.join();



//    for (int i = 0; i < threadsNum; i++) {
//        threads[i] = std::thread(calculate_integral, beginX, endX, beginY, endY, splitsNum, std::ref(partialVolume[i]));
//    }

//    for (std::thread & th : threads)
//    {
//        // If thread Object is Joinable then Join that thread.
//        if (th.joinable())
//            th.join();
//    }

//    std::cout << volumePart1 << std::endl;
//    std::cout << volumePart2 << std::endl;
    // end threads

    double integralVal = volumePart1 + volumePart2;

    return integralVal;
}

IntegrationResult calculatePreciseIntegral(IntegrationParameters integrationParameters, int threadsNum) {
    bool calcFirstTime = true;
    double previousIntegralVal = calculateOnceIntegral(integrationParameters, calcFirstTime, threadsNum),
            newIntegralVal = 0;
    calcFirstTime = false;

    double absError = 99999, relError = 99999;
    while (absError > integrationParameters.absEps and relError > integrationParameters.relEps) {
        integrationParameters.splitsNumX *= 2, integrationParameters.splitsNumY *= 2;

        newIntegralVal = previousIntegralVal / 4 +
                calculateOnceIntegral(integrationParameters, calcFirstTime, threadsNum);

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
    int splitsNumX = 200, splitsNumY = 200;
    double absEps = 0.000005, relEps = 0.02;
    int threadsNum = 2;

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