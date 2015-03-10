#include"CN_CreditNet.h"
#include<iostream>
using namespace std;

int defaultList[100];  // some rounds

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

ofstream fout_trans;
ofstream fout_int;

int main(int argc, char* argv[]){
	int finNum = 20;
	int conNum = 100;
	int proNum = 30;
	// cout << "finNum " << finNum << " conNum " << conNum << " proNum " << proNum << endl;

	fout_trans.open("data_trans");
	fout_int.open("data_int");

	CreditNet creditNet(finNum, conNum, proNum);
	// creditNet.print();
	creditNet.genTest2Graph();
	// creditNet.print();
	
	// cout<<"/////////////////////////////////////////////////"<<endl;
	creditNet.init();
	for (int i=0; i<30; i++) {
		cout << "round " << i << " " << endl;
		fout_trans << i << " ";
		fout_int << i << " ";

		creditNet.update();
		// cout<<"updated"<<endl;
		creditNet.genTrans();

		// print status
		// creditNet.printTranPerCon();
		// creditNet.printProPrice();
		
		//creditNet.print();
		// cout<<endl;
		creditNet.genCostAndDivPay();
		
		creditNet.debtCancel();

		creditNet.chargeIR(i);
		// print status
		// creditNet.printBanlancePerCon();

		cout<<endl;
		// creditNet.print();
	}
	// creditNet.print();
	

	//creditNet.print();
	//Graph* temp = new Graph(creditNet);
	//temp->print();
	
	for (int i = 0; i < 100; i++){
		cout << "Round: " << i << " " << defaultList[i] << endl;
	}

	//system("pause");
	fout_trans.close();
	fout_int.close();

	return 0;
}

