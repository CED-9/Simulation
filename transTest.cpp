#include "CN_CreditNet.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <algorithm>
#include <thread>
#include <mutex>

using namespace std;


int defaultList[100];
ofstream fout_trans;
ofstream fout_int;

mutex lock_rates;
mutex lock_cout;

////////////////////////////////////////////////////////////////////
void singleSimulation(
    int finNum, int conNum, int proNum,
    double threshold, int numIR, 
    int window_size, const int numTest, int burn,
    double* resultRate)
{
    // config the network
    CreditNet creditNet(finNum, conNum, proNum);
    creditNet.genTest0Graph(threshold, numIR);
    creditNet.setRoutePreference(1);
    
    // main loop
    // first window_size runs
    int failRate1 = 0;
    int failRate2 = 0;
    int failRateTotal = 0;
    int test= 0;
    vector<int> array;
    for (int i = 0; i < window_size; ++i){
        int temp;
        
        temp = creditNet.genInterBankTrans();
        
        array.push_back(temp);
        failRate1 += temp;
        failRateTotal += temp;
    }
    
    for (int i = 0; i < window_size; ++i){
        int temp;
        
        temp = creditNet.genInterBankTrans();
        
        array.push_back(temp);
        failRate2 += temp;
        failRateTotal += temp;
    }
    
    int cnt = 0;
    while (1){
        if (abs(failRate1 - failRate2) <= window_size * 2.0 / 1000.0){
            break;
        }
        else{
            int temp;
            for (int k = 0; k<numTest;++k){
                for (int h = 0; h<burn;++h){
                    temp = creditNet.genInterBankTrans;
                }
                for (int i = 0; i<finNum; ++i){
                    for (int j=0; j<finNum;++j){
                        if(i != j){
                            test += creditNet.testInterBankTransWidget(i,j);
                        }                        
                    }             
                }
            }    
        }
    }    
}  
                
        // move on
        int temp;
        
        temp = creditNet.genInterBankTrans();
        
        failRate1 = failRate1 - array[0] + array[window_size];
        failRate2 = failRate2 - array[window_size] + temp;
        failRateTotal += temp;
        array.erase(array.begin());
        array.push_back(temp);
        cnt++;
    }
    lock_rates.lock();
    *resultRate = failRateTotal / (cnt + 2.0 * window_size + 1.0);
    double transRate = (double) test/((double) numTest * (double) finNum * ((double) finNum-1))
    lock_rates.unlock();
    lock_cout.lock();
    cout << "threshold   " << threshold << "   SSrate" << *resultRate << "    Transrate" << transRate<<endl;

    lock_cout.unlock();
}


// argv[1]: initialize mode
// argv[2]: number of interest rates
int main(int argc, char* argv[]){
    int finNum = 4;
    int conNum = 0;
    int proNum = 0;
    double threshold;
    int numIR = atoi(argv[2]);
    int mechanismGenMode = atoi(argv[1]);
    int window_size = 100;
    int iter = 10;
    int numTest = 5;
    int burn = 5;    
    // double degrees [10] = {0.01,0.02,0.04,0.06,0.09,0.12,0.15,0.20,0.25,0.35};
    double degrees[1] = {1};

    // 10 rounds
    for (int i = 0; i < 10; ++i){
        threshold = degrees[i];
        double* rates = new double [iter];
        vector<std::thread*> threadPool;
        double rateFinal = 0;
        
        // smooth the result
        for (int j = 0; j < iter; ++j){
            std::thread* singleRoundThread
            = new std::thread(singleSimulation,
                              finNum, conNum, proNum,
                              threshold, numIR, numTest, burn,
                              window_size, rates + j);
            threadPool.push_back(singleRoundThread);
        }
        
        // wait for all threads to finish
        for (int j = 0; j < iter; ++j){
            threadPool[j]->join();
            delete threadPool[j];
        }
        
        for (int j = 0; j < iter; j++) {
            cout << rates[j] << " ";
            rateFinal += rates[j];
        }
        
        delete [] rates;
        rateFinal /= iter;
        
        // cout << endl << (double)(threshold*199) << " " << 1 - rateFinal << endl;
    }
}

