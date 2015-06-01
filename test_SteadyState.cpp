#include"CN_CreditNet.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
using namespace std;

int defaultList[100];  // 20 rounds

ofstream fout_trans;
ofstream fout_int;

/**
 *  check status
 *  if not GOOD, print it;
 */
void checkStatus(Status& status){
    if (status != GOOD) {
        Utility utility;
        utility.print(status);
    }
}


// Steady State Log
#define NSS_RATE_SIZE 100
double nssRate[NSS_RATE_SIZE];

int main(int argc, char* argv[]){
    // Basic settings
    int finNum = 200;
    int conNum = 0;
    int proNum = 0;
    int iter = 100000; // smooth the result
    
    int testLimit = 200;
    double threshold = 0.09;
    int numIR = atoi(argv[2]);
    int algoMode = atoi(argv[1]); // 1 Greedy, else Widget

    // init
    for (int i = 0; i < NSS_RATE_SIZE; ++i){
        nssRate[i] = 0;
    }

    for (int i = 0; i < iter; ++i){
        CreditNet creditNet(finNum, conNum, proNum);
        creditNet.genTest0Graph(threshold, numIR);
        WidgetGraph* widgetNet = NULL; // no use
        // WidgetGraph* widgetNet = new WidgetGraph;
        // widgetNet->constructWidget(&creditNet);
        
        // main loop
        // first 200 runs
        for (int i = 0; i < testLimit; ++i){
            int temp;
            if (algoMode == 1){
                temp = creditNet.genInterBankTransGreedy(widgetNet);
            } else {
                temp = creditNet.genInterBankTransWidget(widgetNet);
            }
            nssRate[i] += temp;
        }
    }

    // calculate rate
    for (int i = 0; i < NSS_RATE_SIZE; ++i){
        nssRate[i] /= 100000.0;
        cout << i << ": \t" << nssRate[i] << endl;
    }

    return 0;
}
