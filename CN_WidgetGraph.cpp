
#include "CN_WidgetGraph.h"

void WidgetNode::print(){
	cout << "WidgetNode, type " << type << " of " << originNode->getNodeID() 
	<< " local ID: " << localID << endl;
	// cout << "in size: " << edge_in.size() << endl;
	// cout << "out size: " << edge_out.size() << endl;
	for (int i = 0; i < edge_in.size(); ++i){
		cout << " From: " << edge_in[i].nodeFrom->originNode->getNodeID() 
		<< " type " << edge_in[i].nodeFrom->type << " port: " << edge_in[i].nodeFrom->localID << endl;
	}
	for (int i = 0; i < edge_out.size(); ++i){
		cout << " To: " << edge_out[i].nodeTo->originNode->getNodeID() 
		<< " type " << edge_out[i].nodeTo->type << " port: " << edge_out[i].nodeTo->localID << endl;
	}
}

void WidgetGraph::print(){
	cout << "////////////////////////// Widget Net //////////////////////////" << endl;
	for (int i = 0; i < nodeList.size(); ++i){
		nodeList[i]->print();
	}
}

WidgetGraph::WidgetGraph(){}
WidgetGraph::~WidgetGraph(){}

void WidgetGraph::constructWidget(Graph* graphT){
	this->originGraph = graphT;
	// cout << "constructWidget..." << endl;
	for (int k = 0; k < graphT->finAgent.size(); ++k){
		Node* temp = graphT->finAgent[k];
		// cout << "construct node " << temp->getNodeID() << endl;
		for (int i = 0; i < temp->edge_in.size(); ++i){
			// cout << "construct widget node at port in " << i << endl;
			WidgetNode* wNode = new WidgetNode(0, temp, i);
			temp->widgetNodeIn.push_back(wNode);

			this->nodeList.push_back(wNode);

		}
		for (int i = 0; i < temp->edge_out.size(); ++i){
			// cout << "construct widget node at port out " << i << endl;
			WidgetNode* wNode = new WidgetNode(1, temp, i);
			temp->widgetNodeOut.push_back(wNode);
			this->nodeList.push_back(wNode);
			for (int j = 0; j < temp->edge_in.size(); ++j){
				if (temp->edge_in[j].interest_rate <= temp->edge_out[i].interest_rate){
					// cout << "pushing port " << j << " to port " << i << endl;
					WidgetOutEdge wEdge;
					wEdge.nodeTo = temp->widgetNodeOut[i];
					wEdge.c_out_max = temp->edge_in[j].c_in_max;
					wEdge.d_in_current = 0;
					wEdge.interest_rate = temp->edge_in[j].interest_rate;
					temp->widgetNodeIn[j]->edge_out.push_back(wEdge);
					WidgetInEdge wEdgeIn;
					wEdgeIn.nodeFrom = temp->widgetNodeIn[j];
					wEdgeIn.c_in_max = wEdge.c_out_max;
					wEdgeIn.d_out_current = 0;
					wEdgeIn.interest_rate = wEdge.interest_rate;
					temp->widgetNodeOut[i]->edge_in.push_back(wEdgeIn);
				}
			}
		}
	}
	// cout << "finishing inside widget edges" << endl;
	for (int k = 0; k < graphT->finAgent.size(); ++k){
		Node* temp = graphT->finAgent[k];
		// cout << "from node: " << temp->getNodeID() << endl;
		for (int i = 0; i < temp->edge_out.size(); ++i){
			// find sender's port and receiver's port
			WidgetInEdge wInEdge;
			WidgetOutEdge wOutEdge;

			// cout << "from port out: " << i << endl;
			int targetIndex = 0;
			Node* tempTarget = temp->edge_out[i].nodeTo;
			for (int j = 0; j < tempTarget->edge_in.size(); ++j){
				if (temp == tempTarget->edge_in[j].nodeFrom){
					targetIndex = j;
				}
			}

			// cout << "target node: " << tempTarget->getNodeID() << " port in: " << targetIndex << endl;
			
			wInEdge.nodeFrom = temp->widgetNodeOut[i];
			wInEdge.c_in_max = temp->edge_out[i].c_out_max;
			wInEdge.d_out_current = temp->edge_out[i].d_in_current;
			wInEdge.interest_rate = temp->edge_out[i].interest_rate;
			wOutEdge.nodeTo = tempTarget->widgetNodeIn[targetIndex];
			wOutEdge.c_out_max = wInEdge.c_in_max;
			wOutEdge.d_in_current = wInEdge.d_out_current;
			wOutEdge.interest_rate = wInEdge.interest_rate;

			temp->widgetNodeOut[i]->edge_out.push_back(wOutEdge);
			tempTarget->widgetNodeIn[targetIndex]->edge_in.push_back(wInEdge);
		}
	}
	// cout << "finishing outside widget edges" << endl;
}

void WidgetGraph::copyBack(){
	for (int k = 0; k < nodeList.size(); ++k){
		WidgetNode* temp = nodeList[k];
		if (temp->type == 1)
		temp->originNode->setOutEdge(temp->edge_out[0].nodeTo->originNode, 
			temp->edge_out[0].c_out_max, temp->edge_out[0].d_in_current, 
			temp->edge_out[0].interest_rate, EQ);
	}
}
