#ifndef CN_GreedyGraph
#define CN_GreedyGraph

#include "CN_Node.h"
#include "CN_Graph.h"
#include <vector>
#include <unordered_map>

using namespace std;

class GreedyNode;

struct tempDebt{
	double d_temp;
	double ir;
};

struct GreedyInEdge{
	double c_in_max;
	double d_out_current;
	double interest_rate;
	vector< tempDebt > tempDAdd;
	vector< tempDebt > tempDSub;
};

struct GreedyOutEdge{
	double c_out_max;
	double d_in_current;
	double interest_rate;
	vector< tempDebt > tempDAdd;
	vector< tempDebt > tempDSub;
};

class GreedyNode{
public:
	Node* originNode;
	int nodeID;
	unordered_map<GreedyNode*, GreedyOutEdge*> edge_out;
	unordered_map<GreedyNode*, GreedyInEdge*> edge_in;
	// double change
	void setInEdge(
		GreedyNode* nodeT, double c_in_maxT, 
		double d_out_currentT, double interest_rateT
		); 
	void setOutEdge(
		GreedyNode* nodeT, double c_out_maxT, 
		double d_in_currentT, double interest_rateT
		); 
	double getDebt(GreedyNode*);
	double getCredit(GreedyNode*);
	int getNodeID(){ return nodeID; }
	void print();
};

class GreedyGraph{
	Graph* originGraph;
public:
	vector<GreedyNode*> nodeList;
	void constructGreedy(Graph* graphT);
	void copyBack();
	GreedyGraph();
	~GreedyGraph();
	void print();
	
	struct PathInfo{
		int type;  // 0 credit, 1 debt, 2 d+, 3 d-
		GreedyNode* next;
	};
	struct IRInfo{
		double ir;  
		double cap;
		int type;  // 0 credit, 1 debt, 2 d+, 3 d-
		GreedyNode* curr;
	};
	list <PathInfo> pathIRBlocking;
	int maxFlowIRBlocking(GreedyNode* node1, GreedyNode* node2);
	void pathFillIRBlocking(double cap);
	double bfsIRBlocking(GreedyNode*, GreedyNode*);

};















#endif