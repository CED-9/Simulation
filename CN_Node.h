#ifndef CN_Node
#define CN_Node

#include "Error.h"
#include <vector>
#include <list>
#include <fstream>
#include <vector>

#include <iostream>
using namespace std;

extern ofstream fout_trans;
extern ofstream fout_int;

enum OpMode{
	ADD, SUB, EQ
};

enum RouteMechanism{
    FF, LP_SOURCE, LP_OVERALL
};

// forward declaration
class Node;
class Graph; 
class WidgetNode;
class GreedyNode;

struct InEdge{
	Node* nodeFrom;
	double c_in_max;
	double d_out_current;
	double interest_rate;
	// vector< pair<double, double> > temp;
};

struct OutEdge{
	Node* nodeTo;
	double c_out_max;
	double d_in_current;
	double interest_rate;
	// vector< pair<double, double> > temp;
};

class Node{
protected:
	NodeType nodeType;
	int nodeID;
	
public:
	int transNum;
	
    // edges
	std::vector<OutEdge> edge_out;
	std::vector<InEdge> edge_in;
	
    // widget info
	vector<WidgetNode*> widgetNode_In_In;
	vector<WidgetNode*> widgetNode_In_Out;
	vector<WidgetNode*> widgetNode_Out_In;
	vector<WidgetNode*> widgetNode_Out_Out;
	WidgetNode* superWidgetNodeSrc;
	WidgetNode* superWidgetNodeDest;

	GreedyNode* greedyNode;

    // Preference
    RouteMechanism routePreference;
    
    // Transaction
    double transactionNum;
    
	Node(){}
	Node(int id);
	~Node(){}
	
    double getCurrBanlance();
    
    //////////////////////////////////////////////////////////////
    void setRoutePreference(RouteMechanism routeM);
	bool isInOutEdge(Node* nodeT);

	void visualize();
	
	void setNodeType(NodeType nodeTypeT){ nodeType = nodeTypeT; }
	NodeType getNodeType(){ return nodeType; }
	void setNodeID(int id){ nodeID = id; }
	int getNodeID(){ return nodeID; }

	// double change
	Status setInEdge(
		Node* nodeT, double c_in_maxT, 
		double d_out_currentT, double interest_rateT, OpMode mode
		); 
	Status setOutEdge(
		Node* nodeT, double c_out_maxT, 
		double d_in_currentT, double interest_rateT, OpMode mode
		); 

	// basic operation, single change
	// return "remained" credit or current debt 
	// not found = 0
	double getDebtFrom(Node* nodeT, Status &status);
	double getDebtTo(Node* nodeT, Status &status);
	double getCreditFrom(Node* nodeT, Status &status);
	double getCreditTo(Node* nodeT, Status &status);
	// get interest rate
	double getDebIntRateFrom(Node* nodeT, Status status);
	double getDebIntRateTo(Node* nodeT, Status status);
	double getCreIntRateFrom(Node* nodeT, Status status);
	double getCreIntRateTo(Node* nodeT, Status status);
	// basic operation, no change
	// set total credit or current debt
	// no invalid change 
	Status setDebtFrom(Node* nodeT, double value, OpMode mode);
	Status setDebtTo(Node* nodeT, double value, OpMode mode);
	Status setCreditFrom(Node* nodeT, double value, OpMode mode);
	Status setCreditTo(Node* nodeT, double value, OpMode mode);
	// set interest rate
	Status setDebIntRateFrom(Node* nodeT, double value, OpMode mode);
	Status setDebIntRateTo(Node* nodeT, double value, OpMode mode);
	Status setCreIntRateFrom(Node* nodeT, double value, OpMode mode);
	Status setCreIntRateTo(Node* nodeT, double value, OpMode mode);

	Status print(); 
};

class ConNode; 
class ProNode; 
class FinNode; 
class LabNode; 
class BanNode; 
////////////////////////////////////////////////////////////////////////////////

#endif