#include "CN_CreditNet.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <thread>
#include <algorithm>
#include <thread>
#include <mutex>
#include <fstream>

using namespace std;


int defaultList[100];
ofstream fout_trans;
ofstream fout_int;

mutex lock_rates;
mutex lock_cout;

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
    // main loop
    // first window_size runs

    int failRateTotal = 0;
    vector<int> array;
    for (int i = 0; i < window_size; ++i){
        int temp;
        
        temp = creditNet.genInterBankTrans();
        
        array.push_back(temp);
        failRateTotal += temp;
    }
    
  
    /*std::string reps = std::to_string(cnt + 2.0 * window_size + 1.0);*/
    lock_cout.lock();
    cout << threshold << endl;
    creditNet.printPayoff();
/*    ofstream myfile;
    myfile.open(std::to_string(mechanismGenMode) + "_" + std::to_string(threshold) ".txt",ios::app);
    myfile << creditNet.printPayoff() << "," << failRateTotal << "\n";
    myfile.close();
    /*cout << "new thread: threshold " << threshold << " mechanism " << mechanismGenMode << " "<< *resultRate << " " << (cnt + 2.0 * window_size + 1.0) << endl;*/
    lock_cout.unlock();
}


// argv[1]: initialize mode
// argv[2]: number of interest rates
int main(int argc, char* argv[]){
	int finNum = 50;
	int conNum = 0;
	int proNum = 0;
	double threshold;
	int numIR = atoi(argv[2]);
    int mechanismGenMode = atoi(argv[1]);
    int window_size = 5000;
    int iter = 2;
	double degrees [3] = {0.20,0.40,0.80};
        /*,0.02,0.04,0.06,0.09,0.12,0.15,0.20,0.25,0.35};*/

	// 10 rounds
	for (int i = 0; i < 3; ++i){
		threshold = degrees[i];
		double* rates = new double [iter];
        vector<std::thread*> threadPool;
        
        // smooth the result
        for (int j = 0; j < iter; ++j){
            std::thread* singleRoundThread
            = new std::thread(singleSimulation,
                              finNum, conNum, proNum,
                              threshold, numIR, mechanismGenMode,
                              window_size, rates + j);
            threadPool.push_back(singleRoundThread);
		}
        
        // wait for all threads to finish
        for (int j = 0; j < iter; ++j){
            threadPool[j]->join();
            delete threadPool[j];
        }
        
       
        /*cout << endl << (double)(threshold*199) << " " << 1 - rateFinal << endl;*/
	}
}

