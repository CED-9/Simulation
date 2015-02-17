// CN_LabNode.C 

#include"Error.h"
#include"CN_LabNode.h"
#include<queue>
#include<vector>
#include<iostream>
using namespace std;


Status LabNode::setLabNode(int id){
	nodeID = id; 
	nodeType = LABOR; 
	return GOOD; 
}

Status LabNode::init(Graph *creditNet){
    return GOOD;
}


Status LabNode::paymentIn(double value){
    this->curr_in_payment += value;
    return GOOD;
}

double LabNode::getCurrPayment(){
    return this->curr_in_payment;
}

Status LabNode::paymentOut(double value){
    this->curr_in_payment -= value;
    return GOOD;
}

void LabNode::print(){
	Utility uti;
	cout << "LabNode " << nodeID << endl;
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
	cout << endl;
}