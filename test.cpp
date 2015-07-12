#include "CN_CreditNet.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <algorithm>

using namespace std;


int defaultList[100];
ofstream fout_trans;
ofstream fout_int;

////////////////////////////////////////////////////////////////////
void singleSimulation(
                      int finNum, int conNum, int proNum,
                      double threshold, int numIR, int mechanismGenMode,
                      int window_size,
                      double* resultRate)
{
    // config the network
    CreditNet creditNet(finNum, conNum, proNum);
    creditNet.genTest0Graph(threshold, numIR);
    creditNet.setRoutePreference(mechanismGenMode);
    creditNet.printPayoff();
    
    // main loop
    // first window_size runs
    int failRate1 = 0;
    int failRate2 = 0;
    int failRateTotal = 0;
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
    cout << "//////////////////////////////////////////////////////" << endl;
    creditNet.printPayoff();
    
    *resultRate = failRateTotal / (cnt + 2.0 * window_size + 1.0);
    cout << "new thread: threshold " << threshold << " mechanism " << mechanismGenMode << *resultRate << endl;
}


// argv[1]: initialize mode
// argv[2]: number of interest rates
int main(int argc, char* argv[]){
    int finNum = 200;
    int conNum = 0;
    int proNum = 0;
    
    // config the network
    CreditNet creditNet(finNum, conNum, proNum);
    creditNet.genTest0Graph(threshold, numIR);
    creditNet.setRoutePreference(mechanismGenMode);
    
    Node* next;
    creditNet.genInterBankTransFrank(next);
    
}

