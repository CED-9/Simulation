#ifndef CN_LabNode
#define CN_LabNode

#include"CN_Node.h"
#include"Error.h"
#include"CN_Graph.h"

class LabNode : public Node{
private:
    double curr_in_payment;
    
public:

    Status paymentIn(double value);
    double getCurrPayment();
    Status paymentOut(double value);
    
	// To consumer, 
	std::vector<OutEdge> lab_con_out;
	std::vector<InEdge> lab_con_in;

	// To financial, 
	std::vector<OutEdge> lab_fin_out;
	std::vector<InEdge> lab_fin_in;

	LabNode(int id){
        this->setNodeID(id);
        this->setNodeType(LABOR);
    }
    
	Status setLabNode(int id);
    Status init(Graph* creditNet);


	void print();
};


#endif