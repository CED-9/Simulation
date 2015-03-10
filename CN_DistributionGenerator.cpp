#include "CN_DistributionGenerator.h"
#include <random>
#include <algorithm>
#include <vector>
#include <iostream>

using namespace std;

void DisGenerator::normalPartition(double total, int cnt, vector<double> &result){
    default_random_engine generator;
    normal_distribution<double> distribution(total/cnt,1.0);
    for (int i = 0; i < cnt; ++i){
        result.push_back(distribution(generator));
    }
}