#ifndef CN_WidgetGraph
#define CN_WidgetGraph

#include "CN_Node.h"
#include "CN_Graph.h"
#include <vector>

using namespace std;

class WidgetNode;

struct WidgetInEdge{
	WidgetNode* nodeFrom;
	double cap;
	double curr;
	double interest_rate;
	double interest_diff;
	int type;  // 0-5
	WidgetInEdge() : curr(0), type(-1), interest_diff(0) {}
};

struct WidgetOutEdge{
	WidgetNode* nodeTo;
	double cap;
	double curr;
	double interest_rate;
	double interest_diff;
	int type;  // 0-5
	WidgetOutEdge() : curr(0), type(-1), interest_diff(0) {}
};

class WidgetNode{
public:
	int nodeID;
	int getNodeID(){ return nodeID; }

	int localID;
	int type;  // 0 in, 1 out, 2 src, 3 dest
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