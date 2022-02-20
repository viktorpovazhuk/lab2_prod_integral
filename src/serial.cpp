// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include "options_parser.h"

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
calculatePartIntegral(double beginX, double endX, double beginY, double endY, int splitsNum, bool calcFirstTime) {
    int jStartValue = 0, jIncrement = 1;
    if (!calcFirstTime) {
        jStartValue = 1;
        jIncrement = 2;
    }
    double deltaX = (endX - beginX) / splitsNum,
            deltaY = (endY - beginY) / splitsNum;
    double deltaS = deltaX * deltaY;
    double funcValuesSum = 0;
    for (int i = 0; i < splitsNum; i += 2) {
        double curX = beginX + deltaX * i;
        for (int j = jStartValue; j < splitsNum; j += jIncrement) {
            double curY = beginY + deltaY * j;
            funcValuesSum += deDjongFunc(curX, curY);
        }
    }
    for (int i = 1; i < splitsNum; i += 2) {
        double curX = beginX + deltaX * i;
        for (int j = 0; j < splitsNum; ++j) {
            double curY = beginY + deltaY * j;
            funcValuesSum += deDjongFunc(curX, curY);
        }
    }
    double integralValue = funcValuesSum * deltaS;
    return integralValue;
}

double calculateIntegral(double beginX, double endX, double beginY, double endY) {
    int splitsNum = 2000;
    double absEps = 0.000005, relEps = 0.02;
    double previousIntegralVal = calculatePartIntegral(beginX, endX, beginY, endY, splitsNum, true),
            newIntegralVal = 0;
    double absError = 99999, relError = 99999;
    while (absError > absEps or relError > relEps) {
        splitsNum *= 2;
        newIntegralVal = previousIntegralVal / 4 +
                         calculatePartIntegral(beginX, endX, beginY, endY, splitsNum, false);
        absError = std::abs(newIntegralVal - previousIntegralVal);
        relError = std::abs(newIntegralVal - previousIntegralVal) / newIntegralVal;

        std::cout << "splits num: " << splitsNum << '\n'
                  << "abs error: " << absError << '\n'
                  << "rel error: " << relError << std::endl;
        previousIntegralVal = newIntegralVal;
    }
    return newIntegralVal;
}

int main(int argc, char *argv[]) {
    double beginX = -50, endX = 50, beginY = -50, endY = 50;
//    int threadsNum = 6;
//    double partX = (endX - beginX) / threadsNum;
//    std::vector<std::thread> threads;
    double integralValue = calculateIntegral(beginX, endX, beginY, endY);
    std::cout.precision(9);
    std::cout << std::fixed << integralValue;
    return 0;
}
