// Graph.C 

#define NON_DEBUG_MODE

#include "Error.h"
#include "CN_Graph.h"
#include <random>
#include <vector>
#include <list>
#include <queue>
#include <set>
#include <iostream>
#include <algorithm>
using namespace std;

 

#define CREDIT_MAX_FIN_FIN 1000000
#define CREDIT_MAX_FIN_CON 1000
#define CREDIT_MAX_PRO_FIN 1000000

#define LOW_INTEREST_RATE 3 // percent, interbank, c->f, p->f, deposit
#define HIGH_INTEREST_RATE 8 // percent

const int INTER_BANK_INTEREST = LOW_INTEREST_RATE; 
const int PRO_FIN_OUT_INTEREST = LOW_INTEREST_RATE; 
const int FIN_CON_OUT_INTEREST = HIGH_INTEREST_RATE; 

#define NODE_MAX_TOTAL 4000

/////////////////////////////////////////////////////////////////////////
///* Graph Basics */
/////////////////////////////////////////////////////////////////////////

void Graph::visualize(){
	for (int i = 0; i<finNum; i++){
		finAgent[i]->visualize();
	}
}

// print the current state of the graph
void Graph::print(){
	this->banAgent->print();
	this->labAgent->print();
	//this->divAgent->print();
	for (int i = 0; i<conNum; i++){
		conAgent[i]->print();
	}
	for (int i = 0; i<proNum; i++){
		proAgent[i]->print();
	}
	for (int i = 0; i<finNum; i++){
		finAgent[i]->print();
	}
}

// Constructor, new all the nodes
Graph::Graph(int finNumT, int conNumT, int proNumT){
	finNum = finNumT;
	conNum = conNumT;
	proNum = proNumT;

	this->banAgent = new BanNode(0);
	this->labAgent = new LabNode(1);
	//this->divAgent = new DivNode(2);
	int globalID = 2; 
	for (int i = 0; i<finNumT; i++){
		FinNode* f = new FinNode(globalID); 
		finAgent.push_back(f);
		globalID++; 
	}
	for (int i = 0; i<conNumT; i++){
		ConNode* c = new ConNode(globalID); 
		conAgent.push_back(c);
		globalID++;
	}
	for (int i = 0; i<proNumT; i++){
		ProNode* p = new ProNode(globalID); 
		proAgent.push_back(p);
		globalID++;
	}
}

// Copy constructor, new all nodes and edges
Graph::Graph(Graph &graphT){
	// copy all nodes
	this->conNum = graphT.conNum;
	this->finNum = graphT.finNum;
	this->proNum = graphT.proNum;
	
	this->banAgent = new BanNode(graphT.banAgent->getNodeID());
	this->labAgent = new LabNode(graphT.labAgent->getNodeID());
	//this->divAgent = new DivNode(graphT.divAgent->getNodeID());
	for (int i = 0; i<finNum; i++){
		FinNode* f = new FinNode(graphT.finAgent[i]->getNodeID());
		finAgent.push_back(f);
	}
	for (int i = 0; i<conNum; i++){
		ConNode* c = new ConNode(graphT.conAgent[i]->getNodeID());
		conAgent.push_back(c);
	}
	for (int i = 0; i<proNum; i++){
		ProNode* p = new ProNode(graphT.proAgent[i]->getNodeID());
		proAgent.push_back(p);
	}
	
	// copy all edges
	Node* temp = NULL;
	InEdge e1;
	OutEdge e2;
	// bank node
	// edge in
	for (unsigned int i = 0; i<graphT.banAgent->edge_in.size(); i++){
		temp = this->searchID(graphT.banAgent->edge_in[i].nodeFrom->getNodeID());
		e1.c_in_max = graphT.banAgent->edge_in[i].c_in_max;
		e1.d_out_current = graphT.banAgent->edge_in[i].d_out_current;
		e1.interest_rate = graphT.banAgent->edge_in[i].interest_rate;
		e1.nodeFrom = temp;
		this->banAgent->edge_in.push_back(e1);
	}
	// edge out
	for (unsigned int i = 0; i<graphT.banAgent->edge_out.size(); i++){
		temp = this->searchID(graphT.banAgent->edge_out[i].nodeTo->getNodeID());
		e2.c_out_max = graphT.banAgent->edge_out[i].c_out_max;
		e2.d_in_current = graphT.banAgent->edge_out[i].d_in_current;
		e2.interest_rate = graphT.banAgent->edge_out[i].interest_rate;
		e2.nodeTo = temp;
		this->banAgent->edge_out.push_back(e2);
	}
	
	// Dividend node
	/*
	for (unsigned int i = 0; i<graphT.divAgent->edge_in.size(); i++){
		temp = this->searchID(graphT.divAgent->edge_in[i].nodeFrom->getNodeID());
		e1.c_in_max = graphT.divAgent->edge_in[i].c_in_max;
		e1.d_out_current = graphT.divAgent->edge_in[i].d_out_current;
		e1.interest_rate = graphT.divAgent->edge_in[i].interest_rate;
		e1.nodeFrom = temp;
		this->divAgent->edge_in.push_back(e1);
	}
	for (unsigned int i = 0; i<graphT.divAgent->edge_out.size(); i++){
		temp = this->searchID(graphT.divAgent->edge_out[i].nodeTo->getNodeID());
		e2.c_out_max = graphT.divAgent->edge_out[i].c_out_max;
		e2.d_in_current = graphT.divAgent->edge_out[i].d_in_current;
		e2.interest_rate = graphT.divAgent->edge_out[i].interest_rate;
		e2.nodeTo = temp;
		this->divAgent->edge_out.push_back(e2);
	}*/

	// Labor node
	for (unsigned int i = 0; i<graphT.labAgent->edge_in.size(); i++){
		temp = this->searchID(graphT.labAgent->edge_in[i].nodeFrom->getNodeID());
		e1.c_in_max = graphT.labAgent->edge_in[i].c_in_max;
		e1.d_out_current = graphT.labAgent->edge_in[i].d_out_current;
		e1.interest_rate = graphT.labAgent->edge_in[i].interest_rate;
		e1.nodeFrom = temp;
		this->labAgent->edge_in.push_back(e1);
	}
	for (unsigned int i = 0; i<graphT.labAgent->edge_out.size(); i++){
		temp = this->searchID(graphT.labAgent->edge_out[i].nodeTo->getNodeID());
		e2.c_out_max = graphT.labAgent->edge_out[i].c_out_max;
		e2.d_in_current = graphT.labAgent->edge_out[i].d_in_current;
		e2.interest_rate = graphT.labAgent->edge_out[i].interest_rate;
		e2.nodeTo = temp;
		this->labAgent->edge_out.push_back(e2);
	}
	
	// FinNode
	for(int i=0; i< finNum; i++){
		for (unsigned int j = 0; j<graphT.finAgent[i]->edge_in.size(); j++){
			temp = this->searchID(graphT.finAgent[i]->edge_in[j].nodeFrom->getNodeID());
			e1.c_in_max = graphT.finAgent[i]->edge_in[j].c_in_max;
			e1.d_out_current = graphT.finAgent[i]->edge_in[j].d_out_current;
			e1.interest_rate = graphT.finAgent[i]->edge_in[j].interest_rate;
			e1.nodeFrom = temp;
			this->finAgent[i]->edge_in.push_back(e1);
		}
		for (unsigned int j = 0; j<graphT.finAgent[i]->edge_out.size(); j++){
			temp = this->searchID(graphT.finAgent[i]->edge_out[j].nodeTo->getNodeID());
			e2.c_out_max = graphT.finAgent[i]->edge_out[j].c_out_max;
			e2.d_in_current = graphT.finAgent[i]->edge_out[j].d_in_current;
			e2.interest_rate = graphT.finAgent[i]->edge_out[j].interest_rate;
			e2.nodeTo = temp;
			this->finAgent[i]->edge_out.push_back(e2);
		}
	}
	
	// ConNode
	for(int i=0; i< conNum; i++){
		for (unsigned int j = 0; j<graphT.conAgent[i]->edge_in.size(); j++){
			temp = this->searchID(graphT.conAgent[i]->edge_in[j].nodeFrom->getNodeID());
			e1.c_in_max = graphT.conAgent[i]->edge_in[j].c_in_max;
			e1.d_out_current = graphT.conAgent[i]->edge_in[j].d_out_current;
			e1.interest_rate = graphT.conAgent[i]->edge_in[j].interest_rate;
			e1.nodeFrom = temp;
			this->conAgent[i]->edge_in.push_back(e1);
		}
		for (unsigned int j = 0; j<graphT.conAgent[i]->edge_out.size(); j++){
			temp = this->searchID(graphT.conAgent[i]->edge_out[j].nodeTo->getNodeID());
			e2.c_out_max = graphT.conAgent[i]->edge_out[j].c_out_max;
			e2.d_in_current = graphT.conAgent[i]->edge_out[j].d_in_current;
			e2.interest_rate = graphT.conAgent[i]->edge_out[j].interest_rate;
			e2.nodeTo = temp;
			this->conAgent[i]->edge_out.push_back(e2);
		}
	}
	
	// ProNode
	for(int i=0; i< proNum; i++){
		for (unsigned int j = 0; j<graphT.proAgent[i]->edge_in.size(); j++){
			temp = this->searchID(graphT.proAgent[i]->edge_in[j].nodeFrom->getNodeID());
			e1.c_in_max = graphT.proAgent[i]->edge_in[j].c_in_max;
			e1.d_out_current = graphT.proAgent[i]->edge_in[j].d_out_current;
			e1.interest_rate = graphT.proAgent[i]->edge_in[j].interest_rate; 
			e1.nodeFrom = temp; 
			this->proAgent[i]->edge_in.push_back(e1); 
		}
		for (unsigned int j = 0; j<graphT.proAgent[i]->edge_out.size(); j++){
			temp = this->searchID(graphT.proAgent[i]->edge_out[j].nodeTo->getNodeID()); 
			e2.c_out_max = graphT.proAgent[i]->edge_out[j].c_out_max; 
			e2.d_in_current = graphT.proAgent[i]->edge_out[j].d_in_current; 
			e2.interest_rate = graphT.proAgent[i]->edge_out[j].interest_rate; 
			e2.nodeTo = temp; 
			this->proAgent[i]->edge_out.push_back(e2); 
		}
	}
}

// Destructor
Graph::~Graph(){
	for (int i = 0; i<finNum; i++){
		delete finAgent[i];
	}
	for (int i = 0; i<conNum; i++){
		delete conAgent[i];
	}
	for (int i = 0; i<proNum; i++){
		delete proAgent[i];
	}
	delete banAgent;
	delete labAgent;
	//delete divAgent;
}

// Search for an ID, return Node*, not found -> NULL
Node* Graph::searchID(int id){
	Node* result = NULL; 
	if(this->banAgent->getNodeID() == id){ 
		result = banAgent; 
	}
	if(this->labAgent->getNodeID() == id){ 
		result = labAgent; 
	}
	/*
	if (this->divAgent->getNodeID() == id){
		result = divAgent;
	}*/
	for(int i=0; i< this->conNum; i++){
		if(id == this->conAgent[i]->getNodeID()){
			result = conAgent[i]; 
		}
	}
	for(int i=0; i< this->proNum; i++){
		if(id == this->proAgent[i]->getNodeID()){
			result = proAgent[i]; 
		}
	}
	for(int i=0; i< this->finNum; i++){
		if(id == this->finAgent[i]->getNodeID()){
			result = finAgent[i]; 
		}
	}
	return result; 
}

// assignment operator
Graph& Graph::operator=(Graph &graphT){
	// copy all nodes
	this->conNum = graphT.conNum; 
	this->finNum = graphT.finNum; 
	this->proNum = graphT.proNum; 
	
	this->banAgent = new BanNode(graphT.banAgent->getNodeID());
	this->labAgent = new LabNode(graphT.labAgent->getNodeID());
	//this->divAgent = new DivNode(graphT.divAgent->getNodeID());
	for (int i = 0; i<finNum; i++){
		FinNode* f = new FinNode(graphT.finAgent[i]->getNodeID()); 
		finAgent.push_back(f);
	}
	for (int i = 0; i<conNum; i++){
		ConNode* c = new ConNode(graphT.conAgent[i]->getNodeID()); 
		conAgent.push_back(c);
	}
	for (int i = 0; i<proNum; i++){
		ProNode* p = new ProNode(graphT.proAgent[i]->getNodeID()); 
		proAgent.push_back(p);
	}

	// copy all edges
	Node* temp = NULL;
	InEdge e1;
	OutEdge e2;
	// bank node
	// edge in
	for (unsigned int i = 0; i<graphT.banAgent->edge_in.size(); i++){
		temp = this->searchID(graphT.banAgent->edge_in[i].nodeFrom->getNodeID());
		e1.c_in_max = graphT.banAgent->edge_in[i].c_in_max;
		e1.d_out_current = graphT.banAgent->edge_in[i].d_out_current;
		e1.interest_rate = graphT.banAgent->edge_in[i].interest_rate;
		e1.nodeFrom = temp;
		this->banAgent->edge_in.push_back(e1);
	}
	// edge out
	for (unsigned int i = 0; i<graphT.banAgent->edge_out.size(); i++){
		temp = this->searchID(graphT.banAgent->edge_out[i].nodeTo->getNodeID());
		e2.c_out_max = graphT.banAgent->edge_out[i].c_out_max;
		e2.d_in_current = graphT.banAgent->edge_out[i].d_in_current;
		e2.interest_rate = graphT.banAgent->edge_out[i].interest_rate;
		e2.nodeTo = temp;
		this->banAgent->edge_out.push_back(e2);
	}

	// Dividend node
	/*
	for (unsigned int i = 0; i<graphT.divAgent->edge_in.size(); i++){
		temp = this->searchID(graphT.divAgent->edge_in[i].nodeFrom->getNodeID());
		e1.c_in_max = graphT.divAgent->edge_in[i].c_in_max;
		e1.d_out_current = graphT.divAgent->edge_in[i].d_out_current;
		e1.interest_rate = graphT.divAgent->edge_in[i].interest_rate;
		e1.nodeFrom = temp;
		this->divAgent->edge_in.push_back(e1);
	}
	for (unsigned int i = 0; i<graphT.divAgent->edge_out.size(); i++){
		temp = this->searchID(graphT.divAgent->edge_out[i].nodeTo->getNodeID());
		e2.c_out_max = graphT.divAgent->edge_out[i].c_out_max;
		e2.d_in_current = graphT.divAgent->edge_out[i].d_in_current;
		e2.interest_rate = graphT.divAgent->edge_out[i].interest_rate;
		e2.nodeTo = temp;
		this->divAgent->edge_out.push_back(e2);
	}*/

	// Labor node
	for (unsigned int i = 0; i<graphT.labAgent->edge_in.size(); i++){
		temp = this->searchID(graphT.labAgent->edge_in[i].nodeFrom->getNodeID());
		e1.c_in_max = graphT.labAgent->edge_in[i].c_in_max;
		e1.d_out_current = graphT.labAgent->edge_in[i].d_out_current;
		e1.interest_rate = graphT.labAgent->edge_in[i].interest_rate;
		e1.nodeFrom = temp;
		this->labAgent->edge_in.push_back(e1);
	}
	for (unsigned int i = 0; i<graphT.labAgent->edge_out.size(); i++){
		temp = this->searchID(graphT.labAgent->edge_out[i].nodeTo->getNodeID());
		e2.c_out_max = graphT.labAgent->edge_out[i].c_out_max;
		e2.d_in_current = graphT.labAgent->edge_out[i].d_in_current;
		e2.interest_rate = graphT.labAgent->edge_out[i].interest_rate;
		e2.nodeTo = temp;
		this->labAgent->edge_out.push_back(e2);
	}

	// FinNode
	for (int i = 0; i< finNum; i++){
		for (unsigned int j = 0; j<graphT.finAgent[i]->edge_in.size(); j++){
			temp = this->searchID(graphT.finAgent[i]->edge_in[j].nodeFrom->getNodeID());
			e1.c_in_max = graphT.finAgent[i]->edge_in[j].c_in_max;
			e1.d_out_current = graphT.finAgent[i]->edge_in[j].d_out_current;
			e1.interest_rate = graphT.finAgent[i]->edge_in[j].interest_rate;
			e1.nodeFrom = temp;
			this->finAgent[i]->edge_in.push_back(e1);
		}
		for (unsigned int j = 0; j<graphT.finAgent[i]->edge_out.size(); j++){
			temp = this->searchID(graphT.finAgent[i]->edge_out[j].nodeTo->getNodeID());
			e2.c_out_max = graphT.finAgent[i]->edge_out[j].c_out_max;
			e2.d_in_current = graphT.finAgent[i]->edge_out[j].d_in_current;
			e2.interest_rate = graphT.finAgent[i]->edge_out[j].interest_rate;
			e2.nodeTo = temp;
			this->finAgent[i]->edge_out.push_back(e2);
		}
	}

	// ConNode
	for (int i = 0; i< conNum; i++){
		for (unsigned int j = 0; j<graphT.conAgent[i]->edge_in.size(); j++){
			temp = this->searchID(graphT.conAgent[i]->edge_in[j].nodeFrom->getNodeID());
			e1.c_in_max = graphT.conAgent[i]->edge_in[j].c_in_max;
			e1.d_out_current = graphT.conAgent[i]->edge_in[j].d_out_current;
			e1.interest_rate = graphT.conAgent[i]->edge_in[j].interest_rate;
			e1.nodeFrom = temp;
			this->conAgent[i]->edge_in.push_back(e1);
		}
		for (unsigned int j = 0; j<graphT.conAgent[i]->edge_out.size(); j++){
			temp = this->searchID(graphT.conAgent[i]->edge_out[j].nodeTo->getNodeID());
			e2.c_out_max = graphT.conAgent[i]->edge_out[j].c_out_max;
			e2.d_in_current = graphT.conAgent[i]->edge_out[j].d_in_current;
			e2.interest_rate = graphT.conAgent[i]->edge_out[j].interest_rate;
			e2.nodeTo = temp;
			this->conAgent[i]->edge_out.push_back(e2);
		}
	}

	// ProNode
	for (int i = 0; i< proNum; i++){
		for (unsigned int j = 0; j<graphT.proAgent[i]->edge_in.size(); j++){
			temp = this->searchID(graphT.proAgent[i]->edge_in[j].nodeFrom->getNodeID());
			e1.c_in_max = graphT.proAgent[i]->edge_in[j].c_in_max;
			e1.d_out_current = graphT.proAgent[i]->edge_in[j].d_out_current;
			e1.interest_rate = graphT.proAgent[i]->edge_in[j].interest_rate;
			e1.nodeFrom = temp;
			this->proAgent[i]->edge_in.push_back(e1);
		}
		for (unsigned int j = 0; j<graphT.proAgent[i]->edge_out.size(); j++){
			temp = this->searchID(graphT.proAgent[i]->edge_out[j].nodeTo->getNodeID());
			e2.c_out_max = graphT.proAgent[i]->edge_out[j].c_out_max;
			e2.d_in_current = graphT.proAgent[i]->edge_out[j].d_in_current;
			e2.interest_rate = graphT.proAgent[i]->edge_out[j].interest_rate;
			e2.nodeTo = temp;
			this->proAgent[i]->edge_out.push_back(e2);
		}
	}
	return *this; 
}

/*  Add an edge node1 -> node2
*  input: Node* node1, node2
*  return: Status status
*  modify: node1.outedge, node2.indege
*  all quantities = 0
*/
Status Graph::addEdge(Node* node1, Node* node2){
	if (node1->isInOutEdge(node2)) {
		return INVALID_OP;
	}
	InEdge e2;
	OutEdge e3;
	e3.nodeTo = node2;
	e2.nodeFrom = node1;
	e3.c_out_max = 0;
	e3.d_in_current = 0;
	e3.interest_rate = 0;
	e2.c_in_max = 0;
	e2.d_out_current = 0;
	e2.interest_rate = 0;

	node1->edge_out.push_back(e3);
	node2->edge_in.push_back(e2);

	return GOOD;
}

/////////////////////////////////////////////////////////////////////////
/* Generate Initial Network */
/////////////////////////////////////////////////////////////////////////

// genErdosRenyi Graph
// Any possible linkable pair: generate a link of Pr = 0.5
void Graph::genErdosRenyiGraph(){
	default_random_engine generator;
	uniform_real_distribution<double> distribution1(0.0, 1.0);
	uniform_int_distribution<int> distribution2(1, CREDIT_MAX_FIN_FIN);
	uniform_int_distribution<int> distribution3(1, CREDIT_MAX_FIN_CON);
	uniform_int_distribution<int> distribution4(1, CREDIT_MAX_PRO_FIN);

	// f<=> f, c
	for (int i = 0; i<finNum; i++){
		// inter bank, f <=> f
		for (int j = i + 1; j<finNum; j++){
			double num = distribution1(generator);
			if (num > 0.5){
				this->addEdge(dynamic_cast<Node*>(this->finAgent[i]), 
					dynamic_cast<Node*>(this->finAgent[j]));
			}
		}
		// financial to consumer, f <=> c
		for (int j = 0; j<conNum; j++){
			double num = distribution1(generator);
			if (num > 0.5){
				this->addEdge(dynamic_cast<Node*>(this->finAgent[i]), 
					dynamic_cast<Node*>(this->conAgent[j]));
			}
		}
	}

	// producer to financial, p <=> f
	for (int i = 0; i<proNum; i++){
		for (int j = 0; j<finNum; j++){
			double num = distribution1(generator);
			if (num > 0.5){
				this->addEdge(dynamic_cast<Node*>(this->proAgent[i]),
					dynamic_cast<Node*>(this->finAgent[j]));
			}
		}
	}
	// lab to fin
	for (int i=0; i < this->finNum; i++){
		double num = distribution1(generator);
		if (num > 0.5){
			this->addEdge(dynamic_cast<Node*>(this->finAgent[i]),
				dynamic_cast<Node*>(this->labAgent));
		}
	}
	// div to fin
	/*
	for (int i = 0; i < this->finNum; i++){
		double num = distribution1(generator);
		if (num > 0.5){
			this->addEdge(dynamic_cast<Node*>(this->finAgent[i]),
				dynamic_cast<Node*>(this->divAgent));
		}
	}*/
	// ban to fin
	for (int i = 0; i < this->finNum; i++){
		double num = distribution1(generator);
		if (num > 0.5){
			this->addEdge(dynamic_cast<Node*>(this->finAgent[i]),
				dynamic_cast<Node*>(this->banAgent));
		}
	}
}

// Generate a test graph
// finNum = 2, conNum = 1, proNum = 1
void Graph::genTest0Graph(double threshold, int numIR){
	default_random_engine generator;
	uniform_real_distribution<double> distribution1(0.0, 1.0);
	for (int i = 0; i < finNum; i++){
		for (int j = i+1; j < finNum; j++){
			double num = distribution1(generator);
			double ir = (rand() % numIR + 1)/100.0;
			// cout << ir << endl;
			// double ir = 0.1;
			if (num > 1.0 - threshold){

				this->addEdge(finAgent[j], finAgent[i]);
				finAgent[j]->setOutEdge(finAgent[i], 1, rand()%2, ir, EQ);
				
				// if (rand()%2 == 1){
				// 	this->addEdge(finAgent[i], finAgent[j]);
				// 	finAgent[i]->setOutEdge(finAgent[j], 1, 0, ir, EQ);	
				// } else {
				// 	this->addEdge(finAgent[j], finAgent[i]);
				// 	finAgent[j]->setOutEdge(finAgent[i], 1, 0, ir, EQ);
				// }
				// finAgent[i]->setInEdge(finAgent[j], 0.5, 0, ir, EQ);
			}
		}
	}
	// temp
	// finAgent[1]->setOutEdge(finAgent[2], 1, 0, 0.2, EQ);
}

// Generate a test graph
// finNum = 4, conNum = 3, proNum = 3
void Graph::genTestGraph(){
	// e1 = e3, e2 = e4
	// e1:	i->j, i_j_out
	// e2:	j->i, j_i_out
	// e3:	i->j, j_i_in 
	// e4:	j->i, i_j_in
	finNum = 4;
	conNum = 3;
	proNum = 3;
	OutEdge e1, e2;
	InEdge e3, e4;

	e1.nodeTo = finAgent[1];
	e1.interest_rate = double(LOW_INTEREST_RATE)/100.0; 
	e1.d_in_current = 0; 
	e1.c_out_max = 100;

	e2.nodeTo = finAgent[0]; 
	e2.interest_rate = double(LOW_INTEREST_RATE) / 100.0;
	e2.d_in_current = 0; 
	e2.c_out_max = 100;

	e3.nodeFrom = finAgent[0]; 
	e3.interest_rate = double(LOW_INTEREST_RATE) / 100.0;
	e3.d_out_current = 0; 
	e3.c_in_max = e1.c_out_max; 

	e4.nodeFrom = finAgent[1];
	e4.interest_rate = double(LOW_INTEREST_RATE) / 100.0;
	e4.d_out_current = 0; 
	e4.c_in_max = e2.c_out_max;

	// F0, F1
	finAgent[0]->edge_out.push_back(e1);
	finAgent[1]->edge_out.push_back(e2);
	finAgent[1]->edge_in.push_back(e3);
	finAgent[0]->edge_in.push_back(e4);
	// F0, F3
	e1.nodeTo = finAgent[3]; 
	e2.nodeTo = finAgent[0]; 
	e3.nodeFrom  = finAgent[0]; 
	e4.nodeFrom = finAgent[3]; 
	finAgent[0]->edge_out.push_back(e1);
	finAgent[3]->edge_out.push_back(e2);
	finAgent[3]->edge_in.push_back(e3);
	finAgent[0]->edge_in.push_back(e4);
	// F2, F1
	e1.nodeTo = finAgent[1]; 
	e2.nodeTo = finAgent[2]; 
	e3.nodeFrom  = finAgent[2]; 
	e4.nodeFrom = finAgent[1]; 
	finAgent[2]->edge_out.push_back(e1);
	finAgent[1]->edge_out.push_back(e2);
	finAgent[1]->edge_in.push_back(e3);
	finAgent[2]->edge_in.push_back(e4);
	// F2, F3
	e1.nodeTo = finAgent[3]; 
	e2.nodeTo = finAgent[2]; 
	e3.nodeFrom  = finAgent[2]; 
	e4.nodeFrom = finAgent[3]; 
	finAgent[2]->edge_out.push_back(e1);
	finAgent[3]->edge_out.push_back(e2);
	finAgent[3]->edge_in.push_back(e3);
	finAgent[2]->edge_in.push_back(e4);

	for (int i = 0; i < 4; i++){
		// L1, Fi
		e1.nodeTo = finAgent[i]; 
		e2.nodeTo = labAgent; 
		e3.nodeFrom  = labAgent; 
		e4.nodeFrom = finAgent[i]; 
		labAgent->edge_out.push_back(e1);
		finAgent[i]->edge_out.push_back(e2);
		finAgent[i]->edge_in.push_back(e3);
		labAgent->edge_in.push_back(e4);
	}
/*
	for (int i = 0; i < 4; i++){
		// L1, Fi
		e1.nodeTo = divAgent[i];
		e2.nodeTo = divAgent;
		e3.nodeFrom  = divAgent;
		e4.nodeFrom = divAgent[i];
		labAgent->edge_out.push_back(e1);
		finAgent[i]->edge_out.push_back(e2);
		finAgent[i]->edge_in.push_back(e3);
		labAgent->edge_in.push_back(e4);
	}
*/    
	// P0, F0
	e1.nodeTo = finAgent[0];
	e1.interest_rate = double(LOW_INTEREST_RATE)/100.0; 
	e1.d_in_current = 0; 
	e1.c_out_max = 100;

	e2.nodeTo = proAgent[0]; 
	e2.interest_rate = double(HIGH_INTEREST_RATE) / 100.0;
	e2.d_in_current = 0; 
	e2.c_out_max = 100;

	e3.nodeFrom = proAgent[0]; 
	e3.interest_rate = e1.interest_rate;
	e3.d_out_current = 0; 
	e3.c_in_max = e1.c_out_max; 

	e4.nodeFrom = finAgent[0];
	e4.interest_rate = e2.interest_rate;
	e4.d_out_current = 0; 
	e4.c_in_max = e2.c_out_max;
	
	proAgent[0]->edge_out.push_back(e1);
	finAgent[0]->edge_out.push_back(e2);
	finAgent[0]->edge_in.push_back(e3);
	proAgent[0]->edge_in.push_back(e4);

	// P1, F3
	e1.nodeTo = finAgent[3]; 
	e2.nodeTo = proAgent[1]; 
	e3.nodeFrom = proAgent[1]; 
	e4.nodeFrom = finAgent[3]; 
	proAgent[1]->edge_out.push_back(e1);
	finAgent[3]->edge_out.push_back(e2);
	finAgent[3]->edge_in.push_back(e3);
	proAgent[1]->edge_in.push_back(e4);
	// P2, F2
	e1.nodeTo = finAgent[2]; 
	e2.nodeTo = proAgent[2]; 
	e3.nodeFrom = proAgent[2]; 
	e4.nodeFrom = finAgent[2]; 
	proAgent[2]->edge_out.push_back(e1);
	finAgent[2]->edge_out.push_back(e2);
	finAgent[2]->edge_in.push_back(e3);
	proAgent[2]->edge_in.push_back(e4);

	for (int i = 0; i < 3; i++){
		// Ci, Fi
		e1.nodeTo = finAgent[i];
		e1.interest_rate = double(LOW_INTEREST_RATE)/100.0; 
		e1.d_in_current = 0; 
		e1.c_out_max = 100;

		e2.nodeTo = conAgent[i]; 
		e2.interest_rate = double(HIGH_INTEREST_RATE) / 100.0;
		e2.d_in_current = 0; 
		e2.c_out_max = 10;

		e3.nodeFrom = conAgent[i]; 
		e3.interest_rate = e1.interest_rate;
		e3.d_out_current = 0; 
		e3.c_in_max = e1.c_out_max; 

		e4.nodeFrom = finAgent[i];
		e4.interest_rate = e2.interest_rate;
		e4.d_out_current = 0; 
		e4.c_in_max = e2.c_out_max;

		conAgent[i]->edge_out.push_back(e1);
		finAgent[i]->edge_out.push_back(e2);
		finAgent[i]->edge_in.push_back(e3);
		conAgent[i]->edge_in.push_back(e4);
	}

}


// Generate a test graph
void Graph::genTest2Graph(){
	default_random_engine generator;
	uniform_real_distribution<double> distribution1(0.0, 1.0);
	// con <=> fin
	for (int i = 0; i < conNum; i++){
		for (int j = 0; j < finNum; j++){
			double num = distribution1(generator);
			if (num > 0.1){
				this->addEdge(conAgent[i], finAgent[j]);
				this->addEdge(finAgent[j], conAgent[i]);
				conAgent[i]->setInEdge(finAgent[j], 100, 0, 0.08, EQ);
				conAgent[i]->setOutEdge(finAgent[j], 1000, 0, 0.03, EQ);
			}
		}
	}
	// pro <=> fin
	for (int i= 0; i < proNum; i++){
		for (int j = 0; j < finNum; j++){
			double num = distribution1(generator);
			if (num > 0.1){
				this->addEdge(proAgent[i], finAgent[j]);
				this->addEdge(finAgent[j], proAgent[i]);
				proAgent[i]->setInEdge(finAgent[j], 1000, 0, 0.08, EQ);
				proAgent[i]->setOutEdge(finAgent[j], 1000, 0, 0.03, EQ);
			}
		}
	}
	for (int i = 0; i < finNum; i++){
		for (int j = i + 1; j < finNum; j++){
			double num = distribution1(generator);
			if (num > 0.1){
				this->addEdge(finAgent[i], finAgent[j]);
				this->addEdge(finAgent[j], finAgent[i]);
				finAgent[i]->setInEdge(finAgent[j], 700, 0, 0.03, EQ);
				finAgent[i]->setOutEdge(finAgent[j], 700, 0, 0.03, EQ);
			}
		}
	}
	for (int i = 0; i < finNum; i++){
		this->addEdge(this->labAgent, finAgent[i]);
		this->addEdge(finAgent[i], this->labAgent);
		this->labAgent->setInEdge(finAgent[i], 5000, 0, 0.03, EQ);
		this->labAgent->setOutEdge(finAgent[i], 5000, 0, 0.03, EQ);
	}

}

/////////////////////////////////////////////////////////////////////////////
/* Search/Maxflow among all agents */
/* pay case 1, no discretion */
/////////////////////////////////////////////////////////////////////////////

// Case 1 no discretion,
// keep finding the shortest path
// until value is reached
Status Graph::payCase1(Node* src, Node* dest, double value){
	Status status;
	double current = 0;
	double tempCap = 0;

	//cout<<"pay: "<<value<<endl;
	try {
		while (this->bfsMixed(src, dest)) {
			// sense the bfs path capacity
			tempCap = this->pathCapacityMixed();
			if (tempCap<0.000001) {
				break;
			}
			if (tempCap + current > value) {
				// fill in part of current path
				status = this->pathFillMixed(value - current);
				if (status != GOOD) { throw status; }
				current = value;
				break;
			}
			else {
				// fill in all
				this->pathFillMixed(tempCap);
				current += tempCap;
			}
		}

		if (current != value) {
			throw INSUFFICIENT_FLOW;
		}
	}
	catch (Status error) {
		return error;
	}
	return GOOD;
}

double Graph::maxFlowMixed(Node* node1, Node* node2){
	double currentFlow = 0;
	double flow = 0;
	while (bfsMixed(node1, node2)){
		currentFlow = pathCapacityMixed();
		//cout<<"curr flow: "<<currentFlow<<endl;
		if (currentFlow < 0.0001) {
			break;
		}
		flow += currentFlow;
		pathFillMixed(currentFlow);
	}
	return flow;
}

// Fill the current path, up to the input current flow
// no discretion (eg. no interest rate constraints)
// first use credit, then debt
Status Graph::pathFillMixed(double currentFlow){
	//cout<<"pathfill: "<<currentFlow<<endl;
	Node* node1 = pathMixed.front();
	pathMixed.pop_front();
	Node* node2;
	double credit = 0;
	Status status; 
	try{
		while (!pathMixed.empty()){
			node2 = pathMixed.front();
			pathMixed.pop_front();

			credit = node1->getCreditFrom(node2, status);
			//cout<<"credit: "<<node2->getNodeID()<<" "<<credit<<endl;
			// Use credit first, then debt
			if (currentFlow <= credit){
				status = node1->setDebtTo(node2, currentFlow, ADD);
				if (status != GOOD){ throw status; }
				status = node2->setDebtFrom(node1, currentFlow, ADD);
				if (status != GOOD){ throw status; }
			}
			else {
				status = node1->setDebtTo(node2, credit, ADD);
				if (status != GOOD){ throw status; }
				status = node2->setDebtFrom(node1, credit, ADD);
				if (status != GOOD){ throw status; }
				
				//cout<<"curr - cre: "<<currentFlow <<" "<< credit<<endl;
				status = node1->setDebtFrom(node2, currentFlow - credit, SUB);
				if (status != GOOD){
					//cout<<"ccccc"<<endl;
					throw status;
				}
				status = node2->setDebtTo(node1, currentFlow - credit, SUB);
				if (status != GOOD){ throw status; }
			}
			node1 = node2;
		}
	}
	catch (Status error){
		return error; 
	}
	return GOOD; 
}

// Get the max flow for current path
// max flow = credit + debt IOUs
// no interest rate constraint, no discretion
double Graph::pathCapacityMixed(){
	std::list <Node*> pathTemp = pathMixed;
	Status status; 
	Node* node1 = pathMixed.front();
	pathMixed.pop_front();
	Node* node2;

	double cap = (double)(2 * CREDIT_MAX_FIN_FIN);

	while (!pathMixed.empty()){
		node2 = pathMixed.front();
		pathMixed.pop_front();
		cap = min(cap,
			node1->getDebtFrom(node2, status)
			+ node1->getCreditFrom(node2, status));
		node1 = node2;
	}

	while (!pathTemp.empty()){
		pathMixed.push_back(pathTemp.front());
		pathTemp.pop_front();
	}
	return cap;
}

// bfs, no discretion
// path exists when there is credit or debt relation
// input: two node*
// return: true if path (capacity > 0) exists
bool Graph::bfsMixed(Node* node1, Node* node2){
	this->pathMixed.clear(); 
	Status status; 
	queue <Node*> tempQueue; 
	set <Node*> tempVisited; 
	Node* front = NULL;
	Node* prev[NODE_MAX_TOTAL];
	tempVisited.insert(node1); 
	tempQueue.push(node1); 	
	for(int i=0; i<NODE_MAX_TOTAL; i++){
		prev[i] = NULL;
	}
	while(tempQueue.size() != 0){
		front = tempQueue.front(); 
		tempQueue.pop(); 
		if(node2 == front){
			break; 
		}

		// someone issued c 
		for(int i=0; i < front->edge_in.size(); i++){ 
			if(tempVisited.end() != tempVisited.find(front->edge_in[i].nodeFrom)
				|| front->getCreditFrom(front->edge_in[i].nodeFrom, status) <= 0.000001)
			{ 
				continue; 
			}
			prev[front->edge_in[i].nodeFrom->getNodeID()] = front; 
			tempVisited.insert(front->edge_in[i].nodeFrom); 
			tempQueue.push(front->edge_in[i].nodeFrom); 
		}
		// someone owed d, but issued no c
		for(int i=0; i < front->edge_out.size(); i++){
			if(tempVisited.end() != tempVisited.find(front->edge_out[i].nodeTo)
				|| front->getDebtFrom(front->edge_out[i].nodeTo, status) <= 0.000001)
			{ 
				continue; 
			}
			prev[front->edge_out[i].nodeTo->getNodeID()] = front; 
			tempVisited.insert(front->edge_out[i].nodeTo); 
			tempQueue.push(front->edge_out[i].nodeTo); 
		}
	}
	if(front != node2){
		//cout<<"No path exists! "<<endl;
		this->pathMixed.clear(); 
		return false; 
	}
	while(front){
		this->pathMixed.push_front(front); 
		front = prev[front->getNodeID()];
	}
	return true; 
}

///////////////////////////////////////////////////////////////
/* Search Max flow among all nodes, IR blocking */
/* pay case 2, IR blocking */
///////////////////////////////////////////////////////////////

Status Graph::payCase2(Node* src, Node* dest, double value, double &actualValue){
	Status status;
	double current = 0;
	double tempCap = 0;

	//cout<<"pay: "<<value<<endl;
	try {
		while (1){
			tempCap = this->bfsIRBlocking(src, dest);
			if (tempCap < 0.000001) {
				break;
			}
			if (tempCap + current > value) {
				// fill in part of current path
				//cout << "value - currrent: " << value - current << endl;
				this->pathFillIRBlocking(value - current);
				current = value;
				actualValue = value;
				break;
			}
			else {
				// fill in all
				this->pathFillIRBlocking(tempCap);
				current += tempCap;
			}
		}
		if (current != value) {
			actualValue = current;
			throw INSUFFICIENT_FLOW;
		}
	}
	catch (Status error) {
		return error;
	}

	return GOOD;
}

int Graph::maxFlowIRBlocking(Node* node1, Node* node2){
	double currentFlow = 0;
	double flow = 0;
	while (1){
		currentFlow = this->bfsIRBlocking(node1, node2);
		if (currentFlow < 0.000001) {
			break;
		}
		flow += currentFlow;
#ifndef NON_DEBUG_MODE
	this->printPathIRBlocking();
	cout << "current flow: " << currentFlow << endl;
#endif
		pathFillIRBlocking(currentFlow);
	}
	return flow;
}

struct IRPair {
	Node* curr;
	int type;  // 0 C; 1 D
	double ir;
	double cap;	
};
void help_fill_single_hop(Node* node1, Node* node2, int type, double cap){
#ifndef NON_DEBUG_MODE
	cout << "filling: node " << node1->getNodeID() << " node " << node2->getNodeID() <<" cap " << cap << endl;
#endif
	if (type == 0){
		node1->setDebtTo(node2, cap, ADD);
		node2->setDebtFrom(node1, cap, ADD);
	}
	else if (type == 1){
		node1->setDebtFrom(node2, cap, SUB);
		node2->setDebtTo(node1, cap, SUB);
	}
	return;
}
void Graph::pathFillIRBlocking(double cap){
	NodeEdgeTypePair curr = pathIRBlocking.front();
	pathIRBlocking.pop_front();
	NodeEdgeTypePair next;

	while (!pathIRBlocking.empty()){
		next = pathIRBlocking.front();
		pathIRBlocking.pop_front();
		help_fill_single_hop(curr.node, next.node, next.type, cap);
		curr = next;
	}
	pathIRBlocking.clear();
	return;
}
// bfs, interest rate blocking
// node will use the lowest link out
// return the path capacity
// do not fill the path
double Graph::bfsIRBlocking(Node* node1, Node* node2){
	this->pathIRBlocking.clear();
	Status status;

	vector <IRPair> tempQueue;
	set <Node*> tempVisited;
	IRPair front;
	Node* prev[NODE_MAX_TOTAL][2];  // 0 Prev->C->Front ; 1 Prev->D->Front
	for (int i = 0; i<NODE_MAX_TOTAL; i++){
		prev[i][0] = NULL;
		prev[i][1] = NULL;
	}

	tempVisited.insert(node1);
	IRPair tempPair;
	tempPair.curr = node1;
	tempPair.ir = 9;  // > any ir
	tempPair.cap = CREDIT_MAX_FIN_FIN + 1.0;
	tempPair.type = 0;  // credit
	tempQueue.push_back(tempPair);
	tempPair.type = 1;  // debt
	tempQueue.push_back(tempPair);

	
	while (tempQueue.size() != 0){

#ifndef NON_DEBUG_MODE
		cout << "queue:";
		for (unsigned int i = 0; i < tempQueue.size(); i++){
			cout <<" "<< tempQueue[i].curr->getNodeID()<<" ";
		}
		cout << endl;
#endif

		// pop out the smallest ir edge on the frontier
		double temp_max_ir = 10;
		int position = 0;
		for (int i = tempQueue.size()-1; i >= 0; i--){
			if (temp_max_ir > tempQueue[i].ir){
				position = i;
				temp_max_ir = tempQueue[i].ir;
			}
		}
		front = tempQueue[position];
		tempQueue.erase(tempQueue.begin() + position);
		
		// jump out if reached
		if (node2 == front.curr){
			break;
		}

#ifndef NON_DEBUG_MODE
		cout << "node " << front.curr->getNodeID() << " is dequed" << endl;
#endif

		// push all credit / debt out link into the frontier
		double tempIR = 10;
		int next_id = 0;
		Node* next = NULL;
		int typeToNext = 0;  // credit
		for (int i = 0; i < front.curr->edge_in.size(); i++){
			if (tempVisited.end() != tempVisited.find(front.curr->edge_in[i].nodeFrom)){
				continue;
			}
			double tempCap = front.curr->getCreditFrom(front.curr->edge_in[i].nodeFrom, status);
			if (tempCap > 0 && front.ir >= front.curr->edge_in[i].interest_rate){
				tempIR = front.curr->edge_in[i].interest_rate;
				next = front.curr->edge_in[i].nodeFrom;
				next_id = next->getNodeID();

				prev[next_id][0] = front.curr;
				tempVisited.insert(next);
				tempPair.curr = next;
				tempPair.ir = tempIR;
				tempPair.type = typeToNext;
				tempPair.cap = min(front.cap, tempCap);
				tempQueue.push_back(tempPair);
#ifndef NON_DEBUG_MODE
	cout << "node " << next_id << " is pushed" << endl;
#endif
			}
		}
		for (int i = 0; i < front.curr->edge_out.size(); i++){
			if (tempVisited.end() != tempVisited.find(front.curr->edge_out[i].nodeTo)){
				continue;
			}
			double tempCap = front.curr->edge_out[i].d_in_current;
			if (front.ir >= front.curr->edge_out[i].interest_rate
				&& tempCap > 0){
				tempIR = front.curr->edge_out[i].interest_rate;
				typeToNext = 1;
				next = front.curr->edge_out[i].nodeTo;
				next_id = next->getNodeID();

				prev[next_id][1] = front.curr;
				tempVisited.insert(next);
				tempPair.curr = next;
				tempPair.ir = tempIR;
				tempPair.cap = min(tempCap, front.cap);
				tempPair.type = typeToNext;
				tempQueue.push_back(tempPair);
#ifndef NON_DEBUG_MODE
	cout << "node " << next_id << " is pushed" << endl;
#endif
			}
		}
	}  

	if(front.curr != node2){
#ifndef NON_DEBUG_MODE
		cout << "No path exists! " << endl;
#endif
		this->pathMixed.clear(); 
		return 0; 
	}
	double cap = front.cap;
	Node* tempFront = front.curr;

	while (1){
#ifndef NON_DEBUG_MODE
		cout << "node " << tempFront->getNodeID() << endl;
#endif
		Node* tempPrev;
		if (prev[tempFront->getNodeID()][0] != NULL){
			tempPrev = prev[tempFront->getNodeID()][0];
			// push into the path
			NodeEdgeTypePair nodeEdgeTypePair;
			nodeEdgeTypePair.node = tempFront;
			nodeEdgeTypePair.type = 0;
			pathIRBlocking.push_front(nodeEdgeTypePair);
		}
		else if (prev[tempFront->getNodeID()][1] != NULL){
			tempPrev = prev[tempFront->getNodeID()][1];
			// push into the path
			NodeEdgeTypePair nodeEdgeTypePair;
			nodeEdgeTypePair.node = tempFront;
			nodeEdgeTypePair.type = 1;
			pathIRBlocking.push_front(nodeEdgeTypePair);
		}
		else {
			NodeEdgeTypePair nodeEdgeTypePair;
			nodeEdgeTypePair.node = tempFront;
			nodeEdgeTypePair.type = -1;  // no use
			pathIRBlocking.push_front(nodeEdgeTypePair);
			break;
		}
		tempFront = tempPrev;
	}
	return cap; 
}

// TO DO set preference for all
void Graph::setRoutePreference(int opMode){
    switch (opMode) {
        case 1:
            for (int i = 0; i < finNum; i++) {
                finAgent[i]->routePreference = FF;
            }
            break;
        case 2:
            for (int i = 0; i < finNum; i++) {
                finAgent[i]->routePreference = LP_SOURCE;
            }
            break;
        case 3:
            for (int i = 0; i < finNum; i++) {
                finAgent[i]->routePreference = LP_OVERALL;
            }
            break;
        case 4:
            for (int i = 0; i < finNum; i++) {
                finAgent[i]->routePreference = (RouteMechanism)(rand()%3);
            }
            break;
        default:
            break;
    }
    return;
}

//////////////////////////////////////////////////////////////////////////////
/* Debug */
//////////////////////////////////////////////////////////////////////////////
void Graph::printPath(){
	if (pathMixed.empty()){ cout<<"empty"<<endl; return; }
	std::list <Node*> pathTemp = pathMixed;
	Node* node1 = pathMixed.front();
	pathMixed.pop_front();
	cout<<"path node: "<<node1->getNodeID(); 
	while (!pathMixed.empty()){
		node1 = pathMixed.front();
		pathMixed.pop_front();
		cout<<" "<<node1->getNodeID(); 
	}
	cout<<endl; 
	while (!pathTemp.empty()){
		pathMixed.push_back(pathTemp.front());
		pathTemp.pop_front();
	}
	return;
}

void Graph::printPathIRBlocking(){
	if (pathIRBlocking.empty()){ cout << "empty" << endl; return; }
	std::list <NodeEdgeTypePair> pathTemp = pathIRBlocking;
	NodeEdgeTypePair node1 = pathIRBlocking.front();
	pathIRBlocking.pop_front();
	cout << "path node: " << node1.node->getNodeID();
	while (!pathIRBlocking.empty()){
		node1 = pathIRBlocking.front();
		pathIRBlocking.pop_front();
		cout << " " << node1.node->getNodeID();
	}
	cout << endl;
	while (!pathTemp.empty()){
		pathIRBlocking.push_back(pathTemp.front());
		pathTemp.pop_front();
	}
	return;
}

void Graph::setZero(FinNode* f){
	for (int i = 0; i < f->edge_in.size(); i++){
		f->setInEdge(f->edge_in[i].nodeFrom, 0, 0, 0, EQ);
	}
	for (int i = 0; i < f->edge_out.size(); i++){
		f->setOutEdge(f->edge_out[i].nodeTo, 0, 0, 0, EQ);
	}
}

