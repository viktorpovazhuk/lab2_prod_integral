//
// Created by vityha on 20.02.22.
//

#ifndef INTEGRATE_TASK_1_INTEGRATION_STRUCTURES_H
#define INTEGRATE_TASK_1_INTEGRATION_STRUCTURES_H

struct IntegrationResult {
    double integralValue;
    double absError;
    double relError;
};

struct IntegrationParameters {
    double beginX, endX, beginY, endY;
    int splitsNumX, splitsNumY;
    double absEps, relEps;
};

struct PartIntegrationParameters {
    double beginX, endX, beginY, endY;
    int splitsNumX, splitsNumY;
};

#endif //INTEGRATE_TASK_1_INTEGRATION_STRUCTURES_H
