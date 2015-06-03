#include "CN_CreditNet.h"
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <thread>

using namespace std;


int defaultList[100];
ofstream fout_trans;
ofstream fout_int;

////////////////////////////////////////////////////////////////////



int main(int argc, char* argv[]){
	int finNum = 200;
	int conNum = 0;
	int proNum = 0;
	double threshold;
	int numIR = atoi(argv[2]);
	int iter = 10;
	double degrees [10] = {0.01,0.02,0.04,0.06,0.09,0.12,0.15,0.20,0.25,0.35};


	// creditNet.print();
	for (int i = 0; i < 10; ++i){
		threshold = degrees[i];
		double rate;
		double fails = 0;
		double total = 0;
		for (int i = 0; i < iter; ++i){
			CreditNet creditNet(finNum, conNum, proNum);
			creditNet.genTest0Graph(threshold, numIR);
			WidgetGraph* widgetNet = NULL;
			// WidgetGraph* widgetNet = new WidgetGraph;
			// widgetNet->constructWidget(&creditNet);

			// main loop
			// first 2000 runs
			int failRate1 = 0;
			int failRate2 = 0;
			int failRateTotal = 0;
			vector<int> array;
			for (int i = 0; i < 4500; ++i){
				int temp;
                
                temp = creditNet.genInterBankTrans();
				
                array.push_back(temp);
				failRate1 += temp;
				failRateTotal += temp;
			}

			for (int i = 0; i < 4500; ++i){
				int temp;
                
				temp = creditNet.genInterBankTrans();
				
                array.push_back(temp);
				failRate2 += temp;
				failRateTotal += temp;
			}

			int cnt = 0;
			while (1){
				if (abs(failRate1 - failRate2) <= 4500.0*2.0/1000.0){
					break;
				}
				// move on
				int temp;
                
				temp = creditNet.genInterBankTrans();
                
				failRate1 = failRate1 - array[0] + array[4500];
				failRate2 = failRate2 - array[4500] + temp;
				failRateTotal += temp;
				array.erase(array.begin());
				array.push_back(temp);
				cnt++;
			}
			// cout << "failRateTotal: " << failRateTotal 
			// 	<< " cnt: " << cnt << endl;
			fails += (double)failRateTotal;
			total += (double)cnt + 9001.0;

			// widgetNet->copyBack();
			// delete widgetNet;
			// cout << (double)failRateTotal / ((double)cnt + 2001.0) << endl;
		}
		rate = fails/total;
		// cout << "//////////////////////"s << endl;
		// cout << "fail : " << fail << endl;
		// cout << "fail rate 1: " << failRate1 << endl;
		// cout << "fail rate 2: " << failRate2 << endl;
		// cout << "count: " << count << endl;
		cout << (double)(threshold*199) << " " << 1 - rate << " " << total/(double)iter << endl;
	}
}

