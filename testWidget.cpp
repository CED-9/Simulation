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
	int temp = creditNet.genInterBankTrans();
	cout << "temp: " << temp << endl;
	// creditNet.print();

	return 0;
}

