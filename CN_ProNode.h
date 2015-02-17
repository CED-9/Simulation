#ifndef CN_ProNode
#define CN_ProNode

#include"CN_Node.h"
#include"Error.h"
#include"CN_Graph.h"

class ProNode : public Node{
private:
    double capital;
    double laborCost;
    double productivity;
	double currProfit;
public:
	double unit_price;
    double last_sale;
    
    
	// To financial, 
	std::vector<OutEdge> pro_fin_out;
	std::vector<InEdge> pro_fin_in;

	ProNode(int id); 
	Status setProNode(int id);
    double getUnitLaborCost();
    double getProductivity();
	void setCurrProfit(double);
	double getCurrProfit();
    Status sell(double value);
    void debtCancel(Graph *);
        
    // init/update proNode
    Status init(Graph* creditNet);
    Status update(Graph* creditNet);
    
	void print();
};


#endif