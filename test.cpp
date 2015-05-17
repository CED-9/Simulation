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

int main(int argc, char* argv[]){
	int finNum = 100;
	int conNum = 0;
	int proNum = 0;

	double threshold = 0.08;

	// creditNet.print();
	for (int i = 0; i < 12; ++i){
		threshold += 0.04;
		double rate = 0;

		for (int i = 0; i < 10; ++i){
			CreditNet creditNet(finNum, conNum, proNum);
			creditNet.genTest0Graph(threshold);
			WidgetGraph* widgetNet = NULL;
			// WidgetGraph* widgetNet = new WidgetGraph;
			// widgetNet->constructWidget(&creditNet);

			// main loop
			// first 2000 runs
			int failRate1 = 0;
			int failRate2 = 0;
			int failRateTotal = 0;
			vector<int> array;
			for (int i = 0; i < 1000; ++i){
				int temp;
				if (atoi(argv[1]) == 1){
					temp = creditNet.genInterBankTransGreedy(widgetNet);
				} else {
					temp = creditNet.genInterBankTransWidget(widgetNet);
				}
				array.push_back(temp);
				failRate1 += temp;
				failRateTotal += temp;
			}

			for (int i = 0; i < 1000; ++i){
				int temp;
				if (atoi(argv[1]) == 1){
					temp = creditNet.genInterBankTransGreedy(widgetNet);
				} else {
					temp = creditNet.genInterBankTransWidget(widgetNet);
				}
				array.push_back(temp);
				failRate2 += temp;
				failRateTotal += temp;
			}

			int rounds = 10000;
			int cnt = 0;
			while (1){
				if (abs(failRate1 - failRate2) <= 2){
					break;
				}
				// move on
				int temp;
				if (atoi(argv[1]) == 1){
					temp = creditNet.genInterBankTransGreedy(widgetNet);
				} else {
					temp = creditNet.genInterBankTransWidget(widgetNet);
				}
				failRate1 = failRate1 - array[0] + array[1000];
				failRate2 = failRate2 - array[1000] + temp;
				failRateTotal += temp;
				array.erase(array.begin());
				array.push_back(temp);
				cnt++;
			}
			// cout << "failRateTotal: " << failRateTotal 
			// 	<< " cnt: " << cnt << endl;
			rate += (double)failRateTotal / ((double)cnt + 2001.0);
			// widgetNet->copyBack();
			// delete widgetNet;
			// cout << (double)failRateTotal / ((double)cnt + 2001.0) << endl;
		}
		rate /= 10.0;
		// cout << "//////////////////////"s << endl;
		// cout << "fail : " << fail << endl;
		// cout << "fail rate 1: " << failRate1 << endl;
		// cout << "fail rate 2: " << failRate2 << endl;
		// cout << "count: " << count << endl;
		
		cout << (int)(threshold*99) << " " << 1 - rate << endl;
	}
}

