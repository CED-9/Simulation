#include "Error.h"
#include <random>
#include <algorithm>

void normalPartition(double total, int cnt, vector<double> & result){
    default_random_engine generator;
    normal_distribution<double> distribution(total/cnt,1.0);
    for (int i = 0; i < cnt; ++i){
        result.push_back(distribution(generator));
    }
}