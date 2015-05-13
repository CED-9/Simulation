#ifndef CN_FinNode
#define CN_FinNode

#include"CN_Node.h"
#include"Error.h"
#include"CN_Graph.h"

class FinNode : public Node{
private:
    int spread;
    double totalCredit;

public:
	FinNode(int id){
        this->setNodeID(id);
        this->setNodeType(FINANCIAL);
    }
    // ~FinNode();
    
    
    
    /*
	// To consumer, 
	std::vector<OutEdge> fin_con_out;
	std::vector<InEdge> fin_con_in;

	// To producer, 
	std::vector<OutEdge> fin_pro_out;
	std::vector<InEdge> fin_pro_in;

	// To financial, 
	std::vector<OutEdge> fin_fin_out;
	std::vector<InEdge> fin_fin_in;

	// To labor, 
	std::vector<OutEdge> fin_lab_out;
	std::vector<InEdge> fin_lab_in;

	// To bank, 
	std::vector<OutEdge> fin_ban_out;
	std::vector<InEdge> fin_ban_in;
    */
    
	Status setFinNode(int id);
    
    
    // init Financial
    Status init(Graph* creditNet);
    Status update(Graph* creditNet);
    

	void print();
};

#endif