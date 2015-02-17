// CN_CreditNet.C 

#include"Error.h"
#include"CN_CreditNet.h"
#include<queue>
#include<vector>
#include<iostream>
using namespace std;

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

void CreditNet::proPayDiv(ProNode* p){
	Status status;
	LabNode* l = this->labAgent;
	Node* node1 = dynamic_cast<Node*>(p);
	Node* node2 = dynamic_cast<Node*>(l);

	double value = p->getCurrProfit();

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
	double unit_payment = l->getCurrPayment() / (this->conNum);
	cout << "Total Payment: " << l->getCurrPayment() << " unit: " << unit_payment << " conNum: " << this->conNum << endl;
	for (int i = 0; i<this->conNum; i++) {
		double trueValue;
		payCase2(dynamic_cast<Node*>(l), dynamic_cast<Node*>(this->conAgent[i]), unit_payment, trueValue);
		l->paymentOut(unit_payment);
	}

	return;
}


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
void CreditNet::genTrans(){
	double total_trans = 0;
	while (1) {
		ConNode* c = NULL;
		ProNode* p = NULL;
		bool flag = 0;
		for (int i = 0; i < conNum; i++){
			//cout<<"conAgent ID: "<<this->conAgent[i]->getNodeID()<<endl;

			Graph* temp = new Graph(*this);

			c = conAgent[i];
			p = conAgent[i]->getNextPro();
			if (p != NULL) {
				flag = 1;
			}
			else {
				//cout<<"end of list, do nothing"<<endl;
				continue;
			}
			//cout<<"entering temp"<<endl;
			int cid = c->getNodeID();
			int pid = p->getNodeID();
			//temp->print();
			double cap = maxFlowMixed(
				dynamic_cast<Node*>(temp->searchID(cid)),
				dynamic_cast<Node*>(temp->searchID(pid))
				);
			delete temp;
			//cout<<"exit temp"<<endl;

			//cout<<"capacity: "<<cap<<endl;
			double quant;
			c->decideToBuyOpp(p, p->unit_price, min(cap, p->getProductivity()), quant);
			//cout<<"quant: "<<quant<<endl;
			if (quant <= 0.000001) {
				//cout<<"buy zero"<<endl;
				continue;
			}
			double trueValue;
			payCase2(dynamic_cast<Node*>(c), dynamic_cast<Node*>(p), quant, trueValue);
			//cout<<"Quantity: "<<quant<< " from node "<< c->getNodeID()<<" to node "<<p->getNodeID()<<endl;
			total_trans += quant;
			c->buy(quant);
			p->last_sale += quant;
			p->sell(quant);
		}
		if (!flag) {
			break;
		}
	}
	cout << "total trans: " << total_trans << endl;
	return;
}

void CreditNet::payCosts(){
	for (int i = 0; i<this->proNum; i++) {
		//cout<<"proAgent ID: "<<this->proAgent[i]->getNodeID()<<endl;
		this->proPayLab(this->proAgent[i]);
	}
	this->labPayCon();
}

void CreditNet::payDividends(){
	for (int i = 0; i<this->proNum; i++) {
		//cout<<"proAgent ID: "<<this->proAgent[i]->getNodeID()<<endl;
		this->proPayDiv(this->proAgent[i]);
	}
	this->labPayCon();
}

void CreditNet::chargeIR(){
	Graph temp = *this;
	Status status;
	double amount = 0;
	for (int i = 0; i<temp.conNum; i++) {
		for (int j = 0; j<temp.conAgent[i]->edge_in.size(); j++) {
			Node* node1 = dynamic_cast<Node*>(this->conAgent[i]);
			Node* node2 = this->conAgent[i]->edge_in[j].nodeFrom;

			Node* node3 = dynamic_cast<Node*>(temp.conAgent[i]);
			Node* node4 = temp.conAgent[i]->edge_in[j].nodeFrom;
			double ir = node3->getDebtFrom(node4, status)
				* node3->getDebIntRateFrom(node4, status);
			amount += ir;
			double trueValue;
			status = this->payCase2(node1, node2, ir, trueValue);
			if (status != GOOD) {
				cout << "in suff flow" << endl;
			}
		}
	}
	for (int i = 0; i<temp.proNum; i++) {
		for (int j = 0; j<temp.proAgent[i]->edge_in.size(); j++) {
			Node* node1 = dynamic_cast<Node*>(this->proAgent[i]);
			Node* node2 = this->proAgent[i]->edge_in[j].nodeFrom;

			Node* node3 = dynamic_cast<Node*>(temp.proAgent[i]);
			Node* node4 = temp.proAgent[i]->edge_in[j].nodeFrom;
			double ir = node3->getDebtFrom(node4, status)
				* node3->getDebIntRateFrom(node4, status);
			//amount += ir;
			double trueValue;
			this->payCase2(node1, node2, ir, trueValue);
		}
	}
	for (int i = 0; i<temp.finNum; i++) {
		for (int j = 0; j<temp.finAgent[i]->edge_in.size(); j++) {
			Node* node1 = dynamic_cast<Node*>(this->finAgent[i]);
			Node* node2 = this->finAgent[i]->edge_in[j].nodeFrom;

			Node* node3 = dynamic_cast<Node*>(temp.finAgent[i]);
			Node* node4 = temp.finAgent[i]->edge_in[j].nodeFrom;
			double ir = node3->getDebtFrom(node4, status)
				* node3->getDebIntRateFrom(node4, status);
			//amount += ir;
			double trueValue;
			this->payCase2(node1, node2, ir, trueValue);
		}

	}
	// bank and labor
	for (int j = 0; j<temp.banAgent->edge_in.size(); j++) {
		Node* node1 = dynamic_cast<Node*>(this->banAgent);
		Node* node2 = this->banAgent->edge_in[j].nodeFrom;

		Node* node3 = dynamic_cast<Node*>(temp.banAgent);
		Node* node4 = temp.banAgent->edge_in[j].nodeFrom;
		double ir = node3->getDebtFrom(node4, status)
			* node3->getDebIntRateFrom(node4, status);
		//amount += ir;
		double trueValue;
		this->payCase2(node1, node2, ir, trueValue);
	}
	for (int j = 0; j<temp.labAgent->edge_in.size(); j++) {
		Node* node1 = dynamic_cast<Node*>(this->labAgent);
		Node* node2 = this->labAgent->edge_in[j].nodeFrom;

		Node* node3 = dynamic_cast<Node*>(temp.labAgent);
		Node* node4 = temp.labAgent->edge_in[j].nodeFrom;
		double ir = node3->getDebtFrom(node4, status)
			* node3->getDebIntRateFrom(node4, status);
		//amount += ir;
		double trueValue;
		this->payCase2(node1, node2, ir, trueValue);
	}
	cout << "total ir: " << amount << endl;
}


