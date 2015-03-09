// CN_ProNode.C 

#include"Error.h"
#include"CN_ProNode.h"
#include<queue>
#include<vector>
#include<iostream>
using namespace std;

double ProNode::msrp;

ProNode::ProNode(int id){ 
	nodeID = id; 
	nodeType = PRODUCER; 
	unit_price = 0; 
}

Status ProNode::setProNode(int id){
	nodeID = id; 
	nodeType = PRODUCER; 
	unit_price = 0; 

	return GOOD; 
}

double ProNode::getUnitLaborCost(){
    return laborCost;
}

double ProNode::getProductivity(){
    return this->productivity;
}

void ProNode::setCurrProfit(double profit){
	this->currProfit = profit;
}
double ProNode::getCurrProfit(){
	return this->currProfit;
}

Status ProNode::sell(double value){
    this->productivity -= value;
    return GOOD;
}


void ProNode::debtCancel(Graph * creditNet){
	double int_payed = 0;
	double trueValue = 0;
	double income = 0;
	double value = 0;

	for (unsigned int i = 0; i < this->edge_in.size(); ++i){
		Status status;
		Node* target = this->edge_in[i].nodeFrom;
		double payTo = this->getDebtTo(target, status);
		trueValue = 0;

		for (int j = 0; j < this->edge_out.size(); ++j){
			if (this->edge_out[j].d_in_current == 0){
				continue;
			}
			Node* in = this->edge_out[j].nodeTo;
			income = this->edge_out[j].d_in_current;
			value = min(income, payTo);
			creditNet->payCase2(this->edge_out[j].nodeTo, 
				this->edge_in[i].nodeFrom, value, trueValue);
			// cout << "pro trueValue: " << trueValue << endl;
			this->setInEdge(target, 0, trueValue, 0, SUB);
			this->setOutEdge(in, 0, trueValue, 0, SUB);

			payTo -= trueValue;
		}
		int_payed += trueValue;
	}
	// cout << "int_payed: " << int_payed << endl;
	return;
}


// init producer
Status ProNode::init(Graph* creditNet){
    this->unit_price = 1.0;
    this->last_sale = 0;
    this->laborCost = 1;
    this->productivity = 80;
	this->currProfit = 0;
    return GOOD;
}

// update global info
void ProNode::updateGlobal(Graph* cn){
	double sum = 0;
	for (int i = 0; i < cn->proNum; ++i){
		sum += cn->proAgent[i]->unit_price;
	}
	sum = sum/cn->proNum;
	ProNode::msrp = sum;
}

// update certain producer
Status ProNode::update(Graph* creditNet){
    this->unit_price = this->msrp - 0.5 + (rand()%100) / 100.0;
    this->last_sale = 0;
    this->productivity = unit_price * 200;  // marginal cost = 0.01 pr
    return GOOD;
}

void ProNode::print(){
	Utility uti;
	cout << "ProNode " << nodeID << endl;
	for (int i = 0; i<edge_in.size(); i++){
		cout << "in: NodeType ";
		uti.print(edge_in[i].nodeFrom->getNodeType());
		cout << ", ID ";
		cout << edge_in[i].nodeFrom->getNodeID() << ": " << edge_in[i].d_out_current << " / "
			<< edge_in[i].c_in_max << " IR: " << edge_in[i].interest_rate <<endl;
	}
	for (int i = 0; i<edge_out.size(); i++){
		cout << "out: NodeType "; 
		uti.print(edge_out[i].nodeTo->getNodeType());
		cout << ", ID ";
		cout << edge_out[i].nodeTo->getNodeID() << ": " << edge_out[i].d_in_current << " / "
			<< edge_out[i].c_out_max << " IR: " << edge_out[i].interest_rate <<endl;
	}
    cout<<"unit price: "<<this->unit_price<<endl;
	cout << endl;
}