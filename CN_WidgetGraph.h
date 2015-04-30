#ifndef CN_WidgetGraph
#define CN_WidgetGraph

#include "CN_Node.h"
#include "CN_Graph.h"
#include <vector>

using namespace std;

class WidgetNode;

struct WidgetInEdge{
	WidgetNode* nodeFrom;
	double c_in_max;
	double d_out_current;
	double interest_rate;
};

struct WidgetOutEdge{
	WidgetNode* nodeTo;
	double c_out_max;
	double d_in_current;
	double interest_rate;
};

class WidgetNode{
public:
	int nodeID;
	int getNodeID(){ return nodeID; }

	int localID;
	int type;
	vector<WidgetOutEdge> edge_out;
	vector<WidgetInEdge> edge_in;
	Node* originNode;
	WidgetNode(int type, Node* o, int l) : type(type), originNode(o), localID(l) {}
	void print();
};

class WidgetGraph{
private:
	Graph* originGraph;
public:
	Node* src;
	Node* dest;
	double payment;
	vector<WidgetNode*> nodeList;
	void constructWidget(Graph* graphT);
	void copyBack();
	WidgetGraph();
	~WidgetGraph();
	void print();
	void setUpSrcAndDest(Node*, Node*, double);
	int lpSolver();

};






#endif