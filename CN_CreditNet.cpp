// CN_CreditNet.C 
#define NON_DEBUG_MODE

#include  "Error.h"
// #include "CN_DistributionGenerator.h"
#include "CN_CreditNet.h"
#include <queue>
#include <vector>
#include <iostream>
using namespace std;

CreditNet::CreditNet(int finNumT, int conNumT, int proNumT)
        : Graph(finNumT, conNumT, proNumT){}

void CreditNet::proPayLab(ProNode* p){
	Status status; 
	LabNode* l = this->labAgent;
	Node* node1 = dynamic_cast<Node*>(p);
	Node* node2 = dynamic_cast<Node*>(l);

	double value = p->last_sale * p->getUnitLaborCost();
	p->setCurrProfit(p->last_sale - value);
	//cout<<"cost: "<<value<<endl;

	double trueValue;
	status = payCase2(node1, node2, value, trueValue);
	if (status != GOOD){
		Utility utility; 
		utility.print(status);
		cout << "Producer default on service: ProNode " << p->getNodeID() << endl;
	}
	l->paymentIn(trueValue);
	return;
}

void CreditNet::labPayCon(){
	LabNode* l = this->labAgent;
	vector<double> partition;
	DisGenerator gen;

	gen.normalPartition(l->getCurrPayment(), this->conNum, partition);

	cout << "Total Labor Payment: " << l->getCurrPayment() << " conNum: " << this->conNum << endl;
	
	for (int i = 0; i < partition.size(); i++) {
		double trueValue;
		payCase2(dynamic_cast<Node*>(l), dynamic_cast<Node*>(this->conAgent[i]), partition[i], trueValue);
		l->paymentOut(trueValue);
		this->conAgent[i]->setLastIncome(trueValue);
	}

	return;
}


/////////////////////////////////////////////////////////////////////////
/* Credit Network functions */
/////////////////////////////////////////////////////////////////////////
void CreditNet::init(){
	// init all con, pro, fin, ...
	for (int i = 0; i < this->proNum; i++) {
		this->proAgent[i]->init(this);
	}
	for (int i = 0; i < this->conNum; i++) {
		this->conAgent[i]->init(this);
	}
	for (int i = 0; i < this->finNum; i++) {
		this->finAgent[i]->init(this);
	}
}
void CreditNet::update(){
	// update con, pro, fin
	ProNode::updateGlobal(this);
	for (int i = 0; i < this->proNum; i++) {
		//cout<<"update Pro "<<i<<endl;
		this->proAgent[i]->update(this);
	}
	for (int i = 0; i < this->conNum; i++) {
		//cout<<"update Con "<<i<<endl;
		this->conAgent[i]->update(this);
	}
	for (int i = 0; i < this->finNum; i++) {
		//cout<<"update Fin "<<i<<endl;
		this->finAgent[i]->update(this);
	}
}

// for liquid stuff
int CreditNet::genInterBankTrans(){
	FinNode* f1 = NULL;
	FinNode* f2 = NULL;

	int fid1 = rand()%finNum;
	f1 = finAgent[fid1];
	int fid2 = rand()%finNum;
	while (fid1 == fid2){
		fid2 = rand()%finNum;
	}
	f2 = finAgent[fid2];

	double trueValue = 0;

	// this->print();
	// cout << "fid1: " << fid1 << " fid2: " << fid2 << endl;
	WidgetGraph* widgetNet = new WidgetGraph;
	widgetNet->constructWidget(this);
	widgetNet->setUpSrcAndDest(
		this->finAgent[fid1], this->finAgent[fid2], 1.0);
	// widgetNet->print();
	int status = widgetNet->lpSolver();
	if (status != 0){
		// cout << "no solution!" << endl;
		delete widgetNet;
		return 1;
	}
	widgetNet->copyBack();

	delete widgetNet;
	// CreditNet* tempNet = new CreditNet(*this);
	// FinNode* f3 = tempNet->finAgent[fid1];
	// FinNode* f4 = tempNet->finAgent[fid2];

	// payCase2(dynamic_cast<Node*>(f3), dynamic_cast<Node*>(f4), 2.0, trueValue);
	// delete tempNet;
	// // fail
	// if (trueValue < 2.0){
	// 	return 1;
	// }
	// payCase2(dynamic_cast<Node*>(f1), dynamic_cast<Node*>(f2), 2.0, trueValue);

	return 0;
}

void CreditNet::genTrans(){
	double total_trans = 0;
	bool flag = 1;
	while (1) {
		ConNode* c = NULL;
		ProNode* p = NULL;
		flag = 1;
		for(int i = 0; i < conNum; i++){
			// cout<<"conAgent ID: "<<this->conAgent[i]->getNodeID()<<endl;
			c = conAgent[i];
			p = conAgent[i]->getNextPro();
			if (p == NULL) {
				// cout<<"end of list, do nothing"<<endl;
				continue;
			}
			int cid = c->getNodeID();
			int pid = p->getNodeID();
			double proOffer = p->getProductivity();
			double conDemand; 
			c->decideToBuyOpp(p, p->unit_price, proOffer, conDemand);
			double trueValue;

			if (conDemand <= 0.000001) {
				// cout<<"buy zero"<<endl;
				i--;
				continue;
			}

			payCase2(dynamic_cast<Node*>(c), dynamic_cast<Node*>(p), conDemand, trueValue);
			flag = 0;  // some one bought somthing 

			total_trans += trueValue;
			c->buy(trueValue);
			p->last_sale += trueValue;
			p->sell(trueValue);
		}
		if (flag) {
			break;
		}
	}
	cout<<"total trans: "<<total_trans<<endl;
	fout_trans << total_trans <<endl;

	return;
}

void CreditNet::genCostAndDivPay(){
	for (int i=0; i<this->proNum; i++) {
		//cout<<"proAgent ID: "<<this->proAgent[i]->getNodeID()<<endl;
		this->proPayLab(this->proAgent[i]);
	}
	this->labPayCon();
	return;
}

void CreditNet::debtCancel(){
	// Consumer
	for (int i = 0; i < this->conNum; i++){
		this->conAgent[i]->debtCancel(this);
	}
	// Producer
	for (int i = 0; i < this->proNum; i++){
		this->proAgent[i]->debtCancel(this);
	}
	return;
}

void CreditNet::chargeIR(int time){
	// if (time == 10){
	//  this->executeDefault(this->finAgent[0], time);
	// }
	Graph temp = *this;
	Status status;
	double amount = 0;
	// charge the consumers
	for (int i=0; i<temp.conNum; i++) {
		for (int j=0; j<temp.conAgent[i]->edge_in.size(); j++) {
			Node* node1 = dynamic_cast<Node*>(this->conAgent[i]);
			Node* node2 = this->conAgent[i]->edge_in[j].nodeFrom;
			
			Node* node3 = dynamic_cast<Node*>(temp.conAgent[i]);
			Node* node4 = temp.conAgent[i]->edge_in[j].nodeFrom;
			double ir = node3->getDebtTo(node4, status)
				* node3->getDebIntRateTo(node4, status);
			amount += ir;
			double trueValue;
			status = this->payCase2(node1, node2, ir, trueValue);
			if (status!=GOOD) {
				cout<<"in suff flow con node "<< node1->getNodeID() << " to " 
				<< node2->getNodeID() <<endl;
			}
		}
	}
	// charge the producers
	for (int i=0; i<temp.proNum; i++) {
		for (int j=0; j<temp.proAgent[i]->edge_in.size(); j++) {
			Node* node1 = dynamic_cast<Node*>(this->proAgent[i]);
			Node* node2 = this->proAgent[i]->edge_in[j].nodeFrom;
			
			Node* node3 = dynamic_cast<Node*>(temp.proAgent[i]);
			Node* node4 = temp.proAgent[i]->edge_in[j].nodeFrom;
			double ir = node3->getDebtTo(node4, status)
			* node3->getDebIntRateTo(node4, status);
			amount += ir;
			double trueValue;
			status = this->payCase2(node1, node2, ir, trueValue);
			if (status!=GOOD) {
				cout<<"in suff flow pro node "<< node1->getNodeID() << " to " 
				<< node2->getNodeID() <<endl;
			}
		}
	}
	bool flag = 0;
	for (int i=0; i<temp.finNum; i++) {
		for (int j=0; j<temp.finAgent[i]->edge_in.size(); j++) {
			Node* node1 = dynamic_cast<Node*>(this->finAgent[i]);
			Node* node2 = this->finAgent[i]->edge_in[j].nodeFrom;
			
			Node* node3 = dynamic_cast<Node*>(temp.finAgent[i]);
			Node* node4 = temp.finAgent[i]->edge_in[j].nodeFrom;
			double ir = node3->getDebtTo(node4, status)
			* node3->getDebIntRateTo(node4, status);
			amount += ir;
			if (ir <= 0){ continue; }

			double trueValue;
			this->payCase2(node1, node2, ir, trueValue);
#ifndef NON_DEBUG_MODE
			cout << "ir: "<< ir << " true value: " << trueValue << endl;
			cout << "paycase2: node " << node1->getNodeID() << " node "
				<< node2->getNodeID() << " " << node1->getDebtTo(node2, status)
				<<" "<< node1->getDebIntRateTo(node2, status) << endl;
#endif
			if (trueValue < ir){
				cout << "node " << node1->getNodeID() << " is defaulting to node "<< node2->getNodeID() <<" at time " << time << endl;
				this->executeDefault(this->finAgent[i], time);
				temp.setZero(temp.finAgent[i]);
				flag = 1;
				break;
			}   
		}
	}
	// bank and labor
	for (int j=0; j<temp.banAgent->edge_in.size(); j++) {
		Node* node1 = dynamic_cast<Node*>(this->banAgent);
		Node* node2 = this->banAgent->edge_in[j].nodeFrom;
		
		Node* node3 = dynamic_cast<Node*>(temp.banAgent);
		Node* node4 = temp.banAgent->edge_in[j].nodeFrom;
		double ir = node3->getDebtTo(node4, status)
		* node3->getDebIntRateTo(node4, status);
		amount += ir;
		double trueValue;
		status = this->payCase2(node1, node2, ir, trueValue);
		if (status!=GOOD) {
			cout<<"in suff flow node "<< node1->getNodeID() << " to " 
			<< node2->getNodeID() <<endl;
		}
	}
	for (int j=0; j<temp.labAgent->edge_in.size(); j++) {
		Node* node1 = dynamic_cast<Node*>(this->labAgent);
		Node* node2 = this->labAgent->edge_in[j].nodeFrom;
		
		Node* node3 = dynamic_cast<Node*>(temp.labAgent);
		Node* node4 = temp.labAgent->edge_in[j].nodeFrom;
		double ir = node3->getDebtTo(node4, status)
		* node3->getDebIntRateTo(node4, status);
		amount += ir;
		double trueValue;
		status = this->payCase2(node1, node2, ir, trueValue);
		if (status!=GOOD) {
			cout<<"in suff flow node "<< node1->getNodeID() << " to " 
			<< node2->getNodeID() <<endl;
		}
	}
	cout<<"total ir: "<<amount<<endl;

	fout_int << amount <<endl;
	return;
}

void CreditNet::executeDefault(FinNode* f, int time){
	cout <<"execute default node "<<f->getNodeID()<<endl;;
	for (int i = 0; i < f->edge_in.size(); i++){
		f->setInEdge(f->edge_in[i].nodeFrom, 0, 0, 0, EQ);
	}
	for (int i = 0; i < f->edge_out.size(); i++){
		f->setOutEdge(f->edge_out[i].nodeTo, 0, 0, 0, EQ);
	}
	defaultList[time]++;
}



/////////////////////////////////////////////////////////////////////////
/* Print anything */
/////////////////////////////////////////////////////////////////////////
void CreditNet::printTranPerCon(){
	for (int i = 0; i < conNum; ++i){
		cout << "Con Node " << this->conAgent[i]->getNodeID() << " bought: "
			<< this->conAgent[i]->getCurrQuantity() 
			<< " lastIncome " << this->conAgent[i]->getLastIncome() << endl;
	}
}

void CreditNet::printProPrice(){
	for (int i = 0; i < proNum; ++i){
		cout << "Pro Node " << this->proAgent[i]->getNodeID() << " listed price: " 
			<< this->proAgent[i]->unit_price << endl;
	}
}

void CreditNet::printBanlancePerCon(){
	for (int i = 0; i < conNum; ++i){
		cout << "Con Node " << this->conAgent[i]->getNodeID() << " banlance: "
			<< this->conAgent[i]->getCurrBanlance() << endl;
	}
}