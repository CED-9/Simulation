//
//  CN_DevNode.h
//  CN_ON_MAC
//
//  Created by 刘俊明 on 1/26/15.
//  Copyright (c) 2015 刘俊明. All rights reserved.
//

#ifndef CN_DivNode
#define CN_DivNode

#include"CN_Node.h"
#include"Error.h"
#include"CN_Graph.h"

class DivNode : public Node{
private:
    double curr_div;
public:
    Status paymentIn(double value);
    double getCurrPayment();
    Status paymentOut(double value);
    
    DivNode(int id){
        this->setNodeID(id);
        this->setNodeType(DIVIDEND);
    }
    
    Status setDivNode(int id);
    Status init(Graph* credit);
    
    void print();
};

#endif
