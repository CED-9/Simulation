// CN_ConNode.C 

#include"Error.h"
#include"CN_Node.h"
#include"CN_ConNode.h"
#include<queue>
#include<vector>
#include<math.h>
#include<iostream>
using namespace std;

ConNode::ConNode(int id){ 
	this->setNodeID(id);
	this->setNodeType(CONSUMER);
	c_perm = 0; 
	pr_perm = 0; 
	lastPrice = 0; 
}

Status ConNode::setConNode(int id){
	this->setNodeID(id);
	this->setNodeType(CONSUMER);

	c_perm = 0; 
	pr_perm = 0; 
	lastPrice = 0; 

	return GOOD; 
}

void ConNode::setLastIncome(double income){
	this->lastIncome = income;
	return;
}
double ConNode::getLastIncome(){
	return this->lastIncome;
}

double ConNode::getLastPrice(){
    return this->lastPrice;
}

double ConNode::getCurrBanlance(){
	return this->curr_banlance;
}

double ConNode::getCurrCredit(){
	return this->curr_credit;
}

ProNode* ConNode::getNextPro(){
    if (nextProOnList >= proList.size()) {
        return NULL;
    }
    ProNode* p = this->proList[nextProOnList].pro;
    nextProOnList++;
    return p;
}

void ConNode::print(){
	Utility uti;
	cout << "ConNode " << nodeID << endl;
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
			<< edge_out[i].c_out_max << " IR: " << edge_out[i].interest_rate << endl;
	}
    /*for (int i = 0; i<this->proList.size(); i++) {
        cout << "Node: " << this->proList[i].pro->getNodeID()
        << " price: " << this->proList[i].price << endl;
    }*/
    cout << this->c_perm << this->pr_perm << this->nextProOnList;
	cout << endl;
}

// 2(q)^{1/2} => q^{-1/2}
double ConNode::getUtility(double quantity){
	return 2*pow(quantity, 0.5); 
}

double ConNode::getPermMUP(){
	return pow(c_perm, -0.5)/pr_perm;
}

double ConNode::getCurrMUP(double unit_price){
	return (pow(curr_quantity, -0.5))/(unit_price);
}

double ConNode::getTempMUP(double temp, double price){
    return pow(temp, -0.5)/price;
}

double ConNode::getEqualMUP(double price){
	double temp = 0;
    double permMUP = this->getPermMUP();
	while (this->getTempMUP(temp, price) >= permMUP){
		temp += 1;
	}
	// cout<<"perm MUP "<<this->getPermMUP()<<" "<<endl;
	// cout<<"temp - 1 = "<<temp - 1<<endl;
	return temp-1;
}

void ConNode::debtCancel(Graph * creditNet){
	for (unsigned int i = 0; i < this->edge_in.size(); i++){
		Status status;
		Node* target = this->edge_in[i].nodeFrom;
		double payTo = this->getDebtTo(target, status);
		double toPay = this->getDebtFrom(target, status);
		double trueValue = min(toPay, payTo);
		// creditNet->payCase2(this, this->edge_in[i].nodeFrom, value, trueValue);
		if (this->getDebIntRateFrom(target, status) > this->getDebIntRateTo(target, status)){
			continue;
		}
		this->setInEdge(target, 0, trueValue, 0, SUB);
		this->setOutEdge(target, 0, trueValue, 0, SUB);
		// cout<<"Node "<<this->getNodeID()<<" true value: "<<trueValue<<endl;

	}
	return;
}

// one feasible 
Status ConNode::decideToBuyOpp(ProNode* p, double price, double qMax, double &quantityToBuy){
	//int perm_mup = this->getPermMUP();
    // cout<<"CON node "<<this->getNodeID()<<" PRO node "<< p->getNodeID() <<"price: "<<price<<endl;
    //cout<<"c_perm: "<<c_perm<<" pr_perm: "<<pr_perm<<endl;

    if (qMax == 0) {
        quantityToBuy = 0;
        // cout<<"nothinf to buy"<<endl;
        return GOOD;
    }
    
	double equal_q = this->getEqualMUP(price);

    // cout<<"equal_q: "<<equal_q<<endl;
    
	if (equal_q <= curr_quantity){
        
        // cout<<equal_q<<" "<<curr_quantity << " full CON" <<endl;
        
		quantityToBuy = 0;
		return CON_I_AM_FULL; 
	}

	// No money
	if (this->getCurrBanlance() + this->getCurrCredit() + this->lastIncome <= 0){
		quantityToBuy = 0;
		return CON_I_DONT_WANT_TO;
	}
    //cout<<"2"<<endl;
    
    if (equal_q > curr_quantity + qMax){
		quantityToBuy = qMax; 
	}
	else if (equal_q > this->getCurrBanlance() + this->getCurrCredit() + this->lastIncome){
    	quantityToBuy = this->getCurrBanlance() + this->lastIncome;
    }
	else {
		quantityToBuy = equal_q - curr_quantity; 
	}

    //cout<<"3"<<endl;
    
	return GOOD; 
}


Status ConNode::buy(double quantity){
    this->curr_quantity += quantity;
    return GOOD;
}


/////////////////////////////////////////////////////////////////////////////////////////
///* At the beginning at time period: init, update */
/////////////////////////////////////////////////////////////////////////////////////////

// Init ConNode: 
// ProNode list
// c_perm, 
Status ConNode::init(Graph* creditNet){
	pr_perm = 0; 
	for(int i=0; i < creditNet->proNum; i++){
		PriceOfPro p; 
		p.pro = creditNet->proAgent[i]; 
		p.price = creditNet->proAgent[i]->unit_price; 
		proList.push_back(p); 
		pr_perm += creditNet->proAgent[i]->unit_price; 
	}
	pr_perm = pr_perm/creditNet->proNum; 

	c_perm = 100;
	lastPrice = pr_perm;
	nextProOnList = 0;
	curr_banlance = 0;
    return GOOD;
}

Status ConNode::update(Graph* creditNet){
    // maybe update prolist
    this->nextProOnList = 0;
	// update all prices
	for(int i=0; i < proList.size() - 1; i++){
		proList[i].price = proList[i].pro->unit_price; 
	}
	// sort producers it is interested in
	for(int i=0; i < proList.size() - 1; i++){
		for(int j=0; j < proList.size() - 1 - i; i++){
			if (proList[j].price > proList[j+1].price){
				PriceOfPro p; 
				p.price = proList[j+1].price;
				p.pro = proList[j+1].pro; 
				proList[j+1].price = proList[j].price;
				proList[j+1].pro = proList[j].pro; 
				proList[j].price = p.price;
				proList[j].pro = p.pro; 
			}
		}
	}

	// pr_perm: check all consumers, avg their last prices
	double avg = 0; 
	for(int i=0; i<creditNet->conNum; i++){
		avg += creditNet->conAgent[i]->lastPrice; 
	}
	avg = avg / creditNet->conNum; 
	pr_perm = avg; 

    // update curr_quantity
    this->curr_quantity = 0;

    // current banlance
    double temp = 0;
    for (unsigned int i = 0; i < this->edge_in.size(); i++){
    	temp -= this->edge_in[i].d_out_current;
    }
    for (unsigned int i = 0; i < this->edge_out.size(); i++){
    	temp += this->edge_out[i].d_in_current;
    }
    this->curr_banlance = temp;

    temp = 0;
    Status status;
    for (unsigned int i = 0; i < this->edge_in.size(); i++){
    	temp += this->getCreditFrom(dynamic_cast<Node*> (this->edge_in[i].nodeFrom), status);
    }
    this->curr_credit = temp;

	return GOOD; 
}