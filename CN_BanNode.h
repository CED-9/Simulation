#ifndef CN_BanNode
#define CN_BanNode

#include"CN_Node.h"
#include"Error.h"
#include"CN_Graph.h"

// bank agent
// policy not yet defined
class BanNode : public Node{
public:
	// To financial, 
	//std::vector<OutEdge> ban_fin_out;
	//std::vector<InEdge> ban_fin_in;

	Status setBanNode(int id); 
	BanNode(int id){
        this->setNodeID(id);
        this->setNodeType(BANK);
    }

	void print();
};


#endif