#include"CN_Graph.h"
#include<iostream>
using namespace std;


int defaultList[100];  // 20 rounds

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
    
    /*
     if (argc<3){ cout << "Not enough input" << endl; return 0; }
     int finNum = atoi(argv[1]);
     int conNum = atoi(argv[2]);
     int proNum = atoi(argv[3]);
     */
    
	int finNum = 10;
	int conNum = 20;
	int proNum = 10;
    //Status status;
    
    cout << "finNum " << finNum << " conNum "
    << conNum << " proNum " << proNum << endl;
    Graph creditNet(finNum, conNum, proNum);
    // creditNet.print();
    creditNet.genTest2Graph();
	// creditNet.print();
	//creditNet.bfsIRBlocking(creditNet.conAgent[0], creditNet.proAgent[0]);
	//creditNet.print();

	
    cout<<"/////////////////////////////////////////////////"<<endl;
    cout<<"init "<<endl;
    creditNet.init();
    for (int i=0; i<15; i++) {
        cout<<endl<<"round: "<<i<<endl;
        //cout<<"updating..."<<endl;
        creditNet.update();
        
        creditNet.genTrans();
        
        //creditNet.print();
        cout<<endl;
        creditNet.genCostAndDivPay();
        
        creditNet.debtCancel();

        creditNet.chargeIR(i);
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

