#ifndef CN_ConNode
#define CN_ConNode

#include"CN_Node.h"
#include"Error.h"
#include"CN_Graph.h"

// pair of [producer, listed price]
struct PriceOfPro{
	ProNode* pro; 
	double price; 
};

// Consumer agent
class ConNode : public Node{
private:
    // c_perm = E(q_t), expectation of permanent consumption
    // pr_perm = unit price, expected price to accept at current time
    // proList, the list of producers a consumer can see
    double c_perm;
    double pr_perm;
    vector<PriceOfPro> proList;
    
    // curr_quantity, current consumption of this time period
    // lastPrice, the unit price of last transaction
    // nextPro, next producer to bring out some opp
    double curr_quantity;
    double lastPrice;
    double lastIncome;

    double curr_banlance;
    double curr_credit;

    int nextProOnList;
    double getUtility(double quantity);
    // Permanent/Current Marginal Utility per Price
    double getPermMUP();
    double getCurrMUP(double unit_price);
    
    // return the quantity at current price, that currMUP == permMUP
    double getEqualMUP(double price);
    
    // temp quantity -> mup
    double getTempMUP(double temp, double price);

public:
	///////////* Functions *////////////////////////////////////////
	// assumption decidion for single feasible oppertunity (ProNode, price, feasible MaxQuantity)
    double getLastPrice();
    void setLastIncome(double);
    double getLastIncome();

    double getCurrBanlance();
    double getCurrCredit();
    
    Status decideToBuyOpp(ProNode* p, double price, double qMax, double &quantityToBuy);
    void debtCancel(Graph *);
    ProNode* getNextPro();
    
    
    
	Status init(Graph* creditNet);
	Status update(Graph* creditNet); 
	
    Status buy(double quantity);
	
	
	int laborToProvide;
	int prevIncome;
	int moneyToSpend;

	// To labor, in default 1 labor market
	std::vector<OutEdge> con_lab_out;
	std::vector<InEdge> con_lab_in;

	// To financial, 
	std::vector<OutEdge> con_fin_out;
	std::vector<InEdge> con_fin_in;
    
    ConNode(int id);
	Status setConNode(int id); 

	void print();
};

#endif