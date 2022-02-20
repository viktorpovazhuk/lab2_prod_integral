// This is a personal academic project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++, C#, and Java: http://www.viva64.com

#include <iostream>
#include <thread>
#include <vector>
#include <cmath>
#include "options_parser.h"

double de_djong_func(double x1, double x2) {
    double sum = 0.002;
    for (int i = -2; i <= 2; ++i) {
        for (int j = -2; j <= 2; ++j) {
            sum += 1 / (5 * (i + 2) + j + 3 + pow((x1 - 16 * j), 6) + pow((x2 - 16 * i), 6));
        }
    }
    return pow(sum, -1);
}

void calculate_integral(double beginX, double endX, double beginY, double endY, int splitsNum, double &result) {
    double deltaX = (endX - beginX) / splitsNum,
            deltaY = (endY - beginY) / splitsNum;
    double deltaS = deltaX * deltaY;
    double funcValuesSum = 0;

    for (int i = 0; i < splitsNum; ++i) {
        double curX = beginX + deltaX * i;
        for (int j = 0; j < splitsNum; ++j) {
            double curY = beginY + deltaY * j;
            funcValuesSum += de_djong_func(curX, curY);
        }
    }

    double integralValue = funcValuesSum * deltaS;
    result = integralValue;
}


void fn1(int threadNumber) {
    for (int i = 0; i < 100000; ++i) {
        std::cout << "fn-" << threadNumber << " :"<< i <<  std::endl;
    }
}

int main(int argc, char *argv[]) {    // Create a vector of threads
    double beginX = -50, endX = 50, beginY = -50, endY = 50;
    int splitsNum = 1000;
//    int threadsNum = 6;
//    double partX = (endX - beginX) / threadsNum;
//    std::vector<std::thread> threads;
//    double integralValue = calculate_integral(beginX, endX, beginY, endY, splitsNum);
//    std::cout << integralValue;


    int threadsNum = 2;
    std::vector<std::thread> threads(threadsNum);
    std::vector<double> partialVolume(threadsNum);

    double volumePart1;
    double volumePart2;



    std::thread thread1 = std::thread(calculate_integral, -50, 0, -50, 50, splitsNum, std::ref(volumePart1));
    std::thread thread2 = std::thread(calculate_integral, 0, 50, -50, 50, splitsNum, std::ref(volumePart2));

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

    std::cout << volumePart1 << std::endl;
    std::cout << volumePart2 << std::endl;

    return 0;
}
