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
	int finNum = 2;
	int conNum = 0;
	int proNum = 0;
	CreditNet creditNet(finNum, conNum, proNum);
	// creditNet.addEdge(creditNet.finAgent[0],creditNet.finAgent[1]);
	// creditNet.addEdge(creditNet.finAgent[0],creditNet.finAgent[2]);
	// creditNet.addEdge(creditNet.finAgent[1],creditNet.finAgent[2]);
	// creditNet.finAgent[0]->setOutEdge(creditNet.finAgent[1],1,0,1,EQ);
	// creditNet.finAgent[1]->setOutEdge(creditNet.finAgent[2],1,0,1,EQ);
	// creditNet.finAgent[0]->setOutEdge(creditNet.finAgent[2],1,0,1,EQ);

	creditNet.genTest0Graph(1);
	creditNet.print();
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
	// for (int i = 0; i < 10; ++i){
	// 	int temp = creditNet.genInterBankTrans(widgetNet);
	// 	cout << "temp: " << temp << endl;
	// }
	// delete widgetNet;
	widgetNet->setUpSrcAndDest(
		creditNet.finAgent[1], creditNet.finAgent[0], 1.0);
	// // widgetNet->print();
	int status = widgetNet->lpSolver();
	if (status == 0){
		widgetNet->copyBack();
	}
	// creditNet.print();
	delete widgetNet;
	cout << "status: " << status << endl;
	// widgetNet->print();

	// 2nd
	widgetNet = new WidgetGraph;
	widgetNet->constructWidget(&creditNet);
	widgetNet->setUpSrcAndDest(
		creditNet.finAgent[1], creditNet.finAgent[0], 1.0);
	status = widgetNet->lpSolver();
	if (status == 0){
		widgetNet->copyBack();
	}
	delete widgetNet;
	cout << "status: " << status << endl;

	// 3rd
	widgetNet = new WidgetGraph;
	widgetNet->constructWidget(&creditNet);
	widgetNet->setUpSrcAndDest(
		creditNet.finAgent[0], creditNet.finAgent[1], 1.0);
	status = widgetNet->lpSolver();
	if (status == 0){
		widgetNet->copyBack();
	}
	delete widgetNet;
	cout << "status: " << status << endl;

	// 4th
	widgetNet = new WidgetGraph;
	widgetNet->constructWidget(&creditNet);
	widgetNet->setUpSrcAndDest(
		creditNet.finAgent[0], creditNet.finAgent[1], 1.0);
	status = widgetNet->lpSolver();
	if (status == 0){
		widgetNet->copyBack();
	}
	delete widgetNet;
	cout << "status: " << status << endl;

	return 0;
}


