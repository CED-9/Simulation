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

	double threshold = 0.16;

	// creditNet.print();
	for (int i = 0; i < 1; ++i){
		threshold += 0.02;
		double fail = 0;
		int count = 0;

		for (int i = 0; i < 1; ++i){
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
				int temp = creditNet.genInterBankTrans(widgetNet);
				array.push_back(temp);
				failRate1 += temp;
				failRateTotal += temp;
			}

			for (int i = 0; i < 1000; ++i){
				int temp = creditNet.genInterBankTrans(widgetNet);
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
				int result = creditNet.genInterBankTrans(widgetNet);
				failRate1 = failRate1 - array[0] + array[1000];
				failRate2 = failRate2 - array[1000] + result;
				failRateTotal += result;
				array.erase(array.begin());
				array.push_back(result);
				cnt++;
			}
			fail += failRateTotal;
			count += cnt;
			// widgetNet->copyBack();
			// delete widgetNet;
		}
		fail = fail/1.0;
		count /= 1;
		cout << "//////////////////////" << endl;
		cout << "fail : " << fail << endl;
		// cout << "fail rate 1: " << failRate1 << endl;
		// cout << "fail rate 2: " << failRate2 << endl;
		cout << "count: " << count << endl;
		
		cout << (int)(threshold*99) << " " << 1 - fail/(double)(count + 2001) << endl;
	}
 //    /*
 //     if (argc<3){ cout << "Not enough input" << endl; return 0; }
 //     int finNum = atoi(argv[1]);
 //     int conNum = atoi(argv[2]);
 //     int proNum = atoi(argv[3]);
 //     */
	
	// int finNum = 10;
	// int conNum = 20;
	// int proNum = 10;
 //    //Status status;
	
 //    cout << "finNum " << finNum << " conNum "
 //    << conNum << " proNum " << proNum << endl;
 //    Graph creditNet(finNum, conNum, proNum);
 //    // creditNet.print();
 //    creditNet.genTest2Graph();
	// // creditNet.print();
	// //creditNet.bfsIRBlocking(creditNet.conAgent[0], creditNet.proAgent[0]);
	// //creditNet.print();

	
 //    cout<<"/////////////////////////////////////////////////"<<endl;
 //    cout<<"init "<<endl;
 //    creditNet.init();
 //    for (int i=0; i<15; i++) {
 //        cout<<endl<<"round: "<<i<<endl;
 //        //cout<<"updating..."<<endl;
 //        creditNet.update();
		
 //        creditNet.genTrans();
		
 //        //creditNet.print();
 //        cout<<endl;
 //        creditNet.genCostAndDivPay();
		
 //        creditNet.debtCancel();

 //        creditNet.chargeIR(i);
 //        cout<<endl;
 //        // creditNet.print();
 //    }
 //    creditNet.print();
	

 //    //creditNet.print();
 //    //Graph* temp = new Graph(creditNet);
 //    //temp->print();
	
	// for (int i = 0; i < 20; i++){
	// 	cout << "Round: " << i << " " << defaultList[i] << endl;
	// }

 //    //system("pause");
 //    return 0;
}

