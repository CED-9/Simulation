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

int main(int argc, char* argv[]){
    
	int finNum = 10;
	int conNum = 20;
	int proNum = 10;
    //Status status;
    
    cout << "finNum " << finNum << " conNum "
    << conNum << " proNum " << proNum << endl;
    CreditNet creditNet(finNum, conNum, proNum);
    // creditNet.print();
    creditNet.genTest2Graph();
    // creditNet.print();
	
    cout<<"/////////////////////////////////////////////////"<<endl;
    cout<<"init "<<endl;
    creditNet.init();
    for (int i=0; i<15; i++) {
        cout<<endl<<"round: "<<i<<endl;

        creditNet.update();
        cout<<"updated"<<endl;

        creditNet.genTrans();

        // print status
        creditNet.printTranPerCon();
        creditNet.printProPrice();
        
        //creditNet.print();
        cout<<endl;
        creditNet.genCostAndDivPay();
        
        creditNet.debtCancel();

        creditNet.chargeIR(i);
        // print status
        creditNet.printBanlancePerCon();

        cout<<endl;
        // creditNet.print();
    }
    creditNet.print();
    

    //creditNet.print();
    //Graph* temp = new Graph(creditNet);
    //temp->print();
    
	for (int i = 0; i < 20; i++){
		cout << "Round: " << i << " " << defaultList[i] << endl;
	}

    //system("pause");
    return 0;
}

