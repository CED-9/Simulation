
#include "CN_GreedyGraph.h"


void GreedyNode::setInEdge(
		GreedyNode* nodeT, double c_in_maxT, 
		double d_out_currentT, double interest_rateT)
{
	GreedyInEdge* temp = edge_in.find(nodeT)->second;
	temp->c_in_max = c_in_maxT;
	temp->d_out_current = d_out_currentT;
	temp->interest_rate = interest_rateT;
}

void GreedyNode::setOutEdge(
		GreedyNode* nodeT, double c_out_maxT, 
		double d_in_currentT, double interest_rateT)
{
	GreedyOutEdge* temp = edge_out.find(nodeT)->second;
	temp->c_out_max = c_out_maxT;
	temp->d_in_current = d_in_currentT;
	temp->interest_rate = interest_rateT;
}

double GreedyNode::getDebt(GreedyNode* nodeT){
	GreedyOutEdge* tempEdge = edge_out.find(nodeT)->second;
	double debt = tempEdge->d_in_current;
	for (int i = 0; i < tempEdge->tempDAdd.size(); ++i){
		debt += tempEdge->tempDAdd[i].d_temp;
	}
	for (int i = 0; i < tempEdge->tempDSub.size(); ++i){
		debt -= tempEdge->tempDSub[i].d_temp;
	}
	return debt;
}

double GreedyNode::getCredit(GreedyNode* nodeT){
	GreedyInEdge* tempEdge = edge_in.find(nodeT)->second;
	double credit_max = tempEdge->c_in_max;
	double debt = nodeT->getDebt(this);
	return credit_max - debt;
}

void GreedyNode::print(){
	cout << "GreedyNode " << nodeID << endl;
	for (auto it = edge_in.begin(); it != edge_in.end(); it++){
		GreedyInEdge* tempEdge = it->second;
		cout << "From node " << it->first->getNodeID() << endl;
		cout << tempEdge->c_in_max << " " << tempEdge->d_out_current << " "
			<< tempEdge->interest_rate << endl;
	}
	for (auto it = edge_out.begin(); it != edge_out.end(); it++){
		GreedyOutEdge* tempEdge = it->second;
		cout << "To node " << it->first->getNodeID() << endl;
		cout << tempEdge->c_out_max << " " << tempEdge->d_in_current << " "
			<< tempEdge->interest_rate << endl;
	}
}

void GreedyGraph::constructGreedy(Graph* graphT){
	originGraph = graphT;
	for (int i = 0; i < graphT->finAgent.size(); ++i){
		Node* temp = graphT->finAgent[i];
		temp->greedyNode = new GreedyNode;
		temp->greedyNode->nodeID = temp->getNodeID();
		temp->greedyNode->originNode = temp;
		this->nodeList.push_back(temp->greedyNode);
	}
	for (int i = 0; i < graphT->finAgent.size(); ++i){
		Node* temp = graphT->finAgent[i];
		for (int j = 0; j < temp->edge_in.size(); ++j){
			GreedyInEdge* gInEdge = new GreedyInEdge;
			gInEdge->c_in_max = temp->edge_in[j].c_in_max;
			gInEdge->d_out_current = temp->edge_in[j].d_out_current;
			gInEdge->interest_rate = temp->edge_in[j].interest_rate;
			temp->greedyNode->edge_in.insert(make_pair(temp->edge_in[j].nodeFrom->greedyNode, gInEdge));
		}
		for (int j = 0; j < temp->edge_out.size(); ++j){
			GreedyOutEdge* gOutEdge = new GreedyOutEdge;
			gOutEdge->c_out_max = temp->edge_out[j].c_out_max;
			gOutEdge->d_in_current = temp->edge_out[j].d_in_current;
			gOutEdge->interest_rate = temp->edge_out[j].interest_rate;
			temp->greedyNode->edge_out.insert(make_pair(temp->edge_out[j].nodeTo->greedyNode, gOutEdge));
		}
	}
}

void GreedyGraph::copyBack(){
	for (int i = 0; i < nodeList.size(); ++i){
		GreedyNode* curr = nodeList[i];
		Node* target = nodeList[i]->originNode;
		for (auto it = curr->edge_in.begin(); it != curr->edge_in.end(); it++){
			double d_add_temp = 0;
			double d_sub_temp = 0;
			for (int k = 0; k < it->second->tempDAdd.size(); ++k){
				d_add_temp += it->second->tempDAdd[k].d_temp;
			}
			for (int k = 0; k < it->second->tempDSub.size(); ++k){
				d_sub_temp += it->second->tempDSub[k].d_temp;
			}
			target->setInEdge(it->first->originNode, 0, d_add_temp - d_sub_temp, 0, ADD);
		}
	}
}

GreedyGraph::GreedyGraph(){}
GreedyGraph::~GreedyGraph(){}

void GreedyGraph::print(){
	cout << "////////////////////////// Greedy Net //////////////////////////" << endl;
	for (int i = 0; i < nodeList.size(); ++i){
		nodeList[i]->print();
	}
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
double GreedyGraph::bfsIRBlocking(GreedyNode* node1, GreedyNode* node2){
	this->pathIRBlocking.clear();
	Status status;

	vector <IRInfo> tempQueue;
	set <GreedyNode*> tempVisited;
	IRInfo front;
	unordered_map<GreedyNode*, PathInfo > prev;


	tempVisited.insert(node1);
	IRInfo tempIRInfo;
	tempIRInfo.curr = node1;
	tempIRInfo.ir = 10;
	tempIRInfo.cap = CREDIT_MAX_FIN_FIN + 1.0;

	tempIRInfo.type = 0;  // credit
	tempQueue.push_back(tempIRInfo);
	tempIRInfo.type = 1;  // debt
	tempQueue.push_back(tempIRInfo);

	while (tempQueue.size() != 0){
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

		// push all credit / debt out link into the frontier
		double tempIR = 10;
		int next_id = 0;
		GreedyNode* next = NULL;



		// Credit Line, InEdge
		for (auto it = front.curr->edge_in.begin(); it != front.curr->edge_in.end(); it++){
			if (tempVisited.end() != tempVisited.find(it->first)){
				continue;
			}
			double tempCap = front.curr->getCreditFrom(it->first);
			if (tempCap > 0 && front.ir >= it->second->interest_rate){
				tempIR = it->second->interest_rate;
				next = it->first;
				next_id = next->getNodeID();

				// mark path
				PathInfo tempPathInfo;
				tempPathInfo.type = 0;
				tempPathInfo.next = next;
				if (prev.find(front.curr) == prev.end()){
					prev.insert(front.prev, tempPathInfo);
				} else {
					cout << "something wrong" << endl;
					continue;
				}

				// insert to Visited
				tempVisited.insert(next);

				// push into queue
				tempIRInfo.curr = next;
				tempIRInfo.ir = tempIR;
				tempIRInfo.type = 0;
				tempIRInfo.cap = min(front.cap, tempCap);
				tempQueue.push_back(tempIRInfo);
			}
		}
		// Debt Line, OutEdge
		for (auto it = front.curr->edge_out.begin(); it != front.curr->edge_out.end(); it++){
			if (tempVisited.end() != tempVisited.find(it->first)){
				continue;
			}
			double tempCap = front.curr->getDebt(it->first);
			if (front.ir >= it->second->interest_rate
				&& tempCap > 0){
				tempIR = it->second->interest_rate;
				next = it->first;
				next_id = next->getNodeID();

				// mark path
				PathInfo tempPathInfo;
				tempPathInfo.type = 1;
				tempPathInfo.next = next;
				if (prev.find(front.curr) == prev.end()){
					prev.insert(front.prev, tempPathInfo);
				} else {
					cout << "something wrong" << endl;
					continue;
				}

				tempVisited.insert(next);

				// push into queue
				tempIRInfo.curr = next;
				tempIRInfo.ir = tempIR;
				tempIRInfo.type = 1;
				tempIRInfo.cap = min(front.cap, tempCap);
				tempQueue.push_back(tempIRInfo);
			}
		}

	}  

	if(front.curr != node2){
		this->pathMixed.clear(); 
		return 0; 
	}
	double cap = front.cap;
	Node* tempFront = front.curr;

	while (1){
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