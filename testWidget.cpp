#include "CN_CreditNet.h"
#include "CN_WidgetGraph.h"
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
	CreditNet creditNet(finNum, conNum, proNum);
	creditNet.genTest0Graph(1);
	// creditNet.print();
	cout << "////////////////////////////////////////////////////////////////" << endl;

	// cout << "////////////////////////////////////////////////////////////////" << endl;
	// creditNet.print();
	// GreedyGraph greedy;
	// greedy.constructGreedy(&creditNet);
	// cout << "////////////////////////////////////////////////////////////////" << endl;
	// greedy.print();
	// greedy.copyBack();
	WidgetGraph* widgetNet = new WidgetGraph;
	widgetNet->constructWidget(&creditNet);
	// widgetNet->print();
	for (int i = 0; i < 10; ++i){
		int temp = creditNet.genInterBankTrans(widgetNet);
		cout << "temp: " << temp << endl;
	}
	// delete widgetNet;
	// widgetNet->setUpSrcAndDest(
	// 	creditNet.finAgent[2], creditNet.finAgent[0], 1.0);
	// // widgetNet->print();
	// int status = widgetNet->lpSolver();
	// widgetNet->copyBack();
	// creditNet.print();
	// widgetNet->print();

	return 0;
}

