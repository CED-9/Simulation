//
//  CN_DivNode.cpp
//  CN_ON_MAC
//
//  Created by 刘俊明 on 1/27/15.
//  Copyright (c) 2015 刘俊明. All rights reserved.
//

#include"Error.h"
#include"CN_DivNode.h"
#include<queue>
#include<vector>
#include<iostream>
using namespace std;



Status DivNode::setDivNode(int id){
    nodeID = id;
    nodeType = LABOR;
    return GOOD;
}

Status DivNode::init(Graph *creditNet){
    return GOOD;
}


Status DivNode::paymentIn(double value){
    this->curr_div += value;
    return GOOD;
}

double DivNode::getCurrPayment(){
    return this->curr_div;
}

Status DivNode::paymentOut(double value){
    this->curr_div -= value;
    return GOOD;
}

void DivNode::print(){
    Utility uti;
    cout << "DivNode " << nodeID << endl;
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