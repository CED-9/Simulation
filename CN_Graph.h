#ifndef CN_Graph
#define CN_Graph

#define NODE_MAX 1000

extern int defaultList[100];  // 20 rounds

#include "CN_ConNode.h"
#include "CN_ProNode.h"
#include "CN_FinNode.h"
#include "CN_BanNode.h"
#include "CN_LabNode.h"
//#include"CN_DivNode.h"
#include <list>
#include <fstream>


class Graph{
public:
	vector<ConNode*> conAgent;
	vector<ProNode*> proAgent;
	vector<FinNode*> finAgent;
	BanNode* banAgent;
	LabNode* labAgent;
	//DivNode* divAgent;
	int finNum;
	int conNum;
	int proNum;

	/////////////////////////////////////////////////////////////////////////
	/* Graph basics */
	/////////////////////////////////////////////////////////////////////////
	Graph(int finNumT, int conNumT, int proNumT);
    Graph(Graph &graphT);
    ~Graph();
	void print();
	Node* searchID(int id);
	Graph& operator=(Graph &graphT);
    
    /////////////////////////////////////////////////////////////////////////
    /* Set all route mechanism preference */
    /////////////////////////////////////////////////////////////////////////
    void setRoutePreference(int opMode);
    

	void setZero(FinNode* f);

	void visualize();

	/////////////////////////////////////////////////////////////////////////
	/* Generate Initial Network */
	/////////////////////////////////////////////////////////////////////////
	Status addEdge(Node* node1, Node* node2);
	void genErdosRenyiGraph();
	void genTest0Graph(double, int);
	void genTestGraph();
	void genTest2Graph();

	/////////////////////////////////////////////////////////////////////////
	/* Payment */
	/////////////////////////////////////////////////////////////////////////
    // case 1: bfs + max flow
    // no discretion, no interest rate constraint
	// always choose shortest path first
    std::list <Node*> pathMixed;
	double pathCapacityMixed();
	bool bfsMixed(Node* node1, Node* node2);
	Status pathFillMixed(double cap);
	double maxFlowMixed(Node* node1, Node* node2);
    
    // case 2: bfs + max flow
    // interest rate constraint, no other discretion
    // always choose lowest ir path first
	struct NodeEdgeTypePair{
		int type;  // 0 C, 1 D
		Node* node;
	};
	std::list <NodeEdgeTypePair> pathIRBlocking;
	int maxFlowIRBlocking(Node* node1, Node* node2);
    double bfsIRBlocking(Node* node1, Node* node2);
    void pathFillIRBlocking(double cap);


    // Payment, case 1, no discretion, just bfs
    Status payCase1(Node* src, Node* dest, double value);
    // Payment, case 2, interest rate blocking
	Status payCase2(Node* src, Node* dest, double value, double& actualValue);


	/////////////////////////////////////////////////////////////////////////
	/* Debug */
	/////////////////////////////////////////////////////////////////////////
    // Scan Graph!
	void printPath();
	void printPathIRBlocking();
};


#endif