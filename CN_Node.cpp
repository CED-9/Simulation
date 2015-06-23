// Node.C

#include "Error.h"
#include "CN_Node.h"
#include <queue>
#include <vector>
#include <fstream>

Node::Node(int id){
    this->nodeID = id;
    this->nodeType = NodeType(0);
    this->transactionNum = 0.0;
}

void Node::visualize(){
	for (int i = 0; i<edge_out.size(); i++){
		cout << this->getNodeID() << ", " << edge_out[i].nodeTo->getNodeID();
    }
}

Status Node::print(){
    cout << "Node " << nodeID << endl;
    for (int i = 0; i<edge_in.size(); i++){
        cout << "in: NodeType "<< (int)edge_in[i].nodeFrom->getNodeType() << ", ID ";
        cout << edge_in[i].nodeFrom->getNodeID() << ": " << edge_in[i].d_out_current << " / "
        << edge_in[i].c_in_max << " IR: " << edge_in[i].interest_rate <<endl;
    }
    for (int i = 0; i<edge_out.size(); i++){
        cout << "out: NodeType "<< (int)edge_out[i].nodeTo->getNodeType() << ", ID ";
        cout << edge_out[i].nodeTo->getNodeID() << ": " << edge_out[i].d_in_current << " / "
        << edge_out[i].c_out_max << " IR: " << edge_out[i].interest_rate << endl;
    }
    cout << endl;
    return GOOD; 
}


void Node::setRoutePreference(RouteMechanism routeM){
    this->routePreference = routeM;
}

double Node::getCurrBanlance(){
    double temp = 0;
    for (unsigned int i = 0; i < this->edge_in.size(); i++){
        temp -= this->edge_in[i].d_out_current * this->edge_in[i].interest_rate;
    }
    for (unsigned int i = 0; i < this->edge_out.size(); i++){
        temp += this->edge_out[i].d_in_current * this->edge_out[i].interest_rate;
    }
    return temp;
}

bool Node::isInOutEdge(Node* nodeT){
    for (int i=0; i<this->edge_out.size(); i++) {
        if (nodeT == this->edge_out[i].nodeTo) {
            return true;
        }
    }
    return false;
}

///////////////////////////////////////////////////////////////
/* double change 
 * public functions
 */
///////////////////////////////////////////////////////////////
Status Node::setInEdge(
		Node* nodeT, double c_in_maxT, 
		double d_out_currentT, double interest_rateT, OpMode mode)
{
	Status status; 
	try{
		for (int i = 0; i < edge_in.size(); i++){
			if (edge_in[i].nodeFrom == nodeT){
				status = this->setDebtTo(nodeT, d_out_currentT, mode);
				if (status != GOOD){ throw status; }
				status = this->setDebIntRateTo(nodeT, interest_rateT, mode);
				if (status != GOOD){ throw status; }

				status = nodeT->setDebtFrom(this, d_out_currentT, mode);
				if (status != GOOD){ throw status; }
				status = nodeT->setDebIntRateFrom(this, interest_rateT, mode);
				if (status != GOOD){ throw status; }

				status = this->setCreditFrom(nodeT, c_in_maxT, mode); 
				if (status != GOOD){ throw status; }
				status = nodeT->setCreditTo(this, c_in_maxT, mode);
                if (status != GOOD){ throw status; }
			}
		}
	}
	catch (Status error){
		cout << "set in edge, something is wrong :(" << endl;
		return error; 
	}
	return GOOD;
}

Status Node::setOutEdge(
		Node* nodeT, double c_out_maxT, 
		double d_in_currentT, double interest_rateT, OpMode mode)
{
	Status status; 
	try{
		for (int i = 0; i < edge_out.size(); i++){
			if (edge_out[i].nodeTo == nodeT){
                
				status = this->setDebtFrom(nodeT, d_in_currentT, mode); 
				if (status != GOOD){ throw status; }
				status = this->setDebIntRateFrom(nodeT, interest_rateT, mode); 
				if (status != GOOD){ throw status; }
                
				status = nodeT->setDebtTo(this, d_in_currentT, mode); 
				if (status != GOOD){ throw status; }
				status = nodeT->setDebIntRateTo(this, interest_rateT, mode); 
				if (status != GOOD){ throw status; }

				status = this->setCreditTo(nodeT, c_out_maxT, mode);
				if (status != GOOD){ throw status; }
				status = nodeT->setCreditFrom(this, c_out_maxT, mode);
				if (status != GOOD){ throw status; }
			}
		}
	}
	catch (Status error){
		cout << "set out edge, something is wrong :(" << endl;
		return error; 
	}
	return GOOD;
}

///////////////////////////////////////////////////////////////
/* Basic operations, no change, get
 * public functions
 */
///////////////////////////////////////////////////////////////
/* Find (current debt) from other node
* not found = 0
*/
double Node::getDebtFrom(Node* nodeT, Status &status){
	for (int i = 0; i < edge_out.size(); i++){
		if (edge_out[i].nodeTo == nodeT){
			return edge_out[i].d_in_current;
		}
	}
	// if not found return 0
	status = GET_NODE_NOT_REACHABLE; 
	return (double)0;
}

/* Find (current debt interest rate) from other node
* not found = 0
*/
double Node::getDebIntRateFrom(Node* nodeT, Status status){
	for (int i = 0; i < edge_out.size(); i++){
		if (edge_out[i].nodeTo == nodeT){
			return edge_out[i].interest_rate;
		}
	}
	// if not found return 0
	status = GET_NODE_NOT_REACHABLE; 
	return 0;
}

/* Find (credit remaining) from other node
* not found = 0
*/
double Node::getCreditFrom(Node* nodeT, Status &status){
	for (int i = 0; i < edge_in.size(); i++){
		if (edge_in[i].nodeFrom == nodeT){
            //cout<<"                        get credit: "<< edge_in[i].c_in_max <<" "<<edge_in[i].d_out_current<< " "<<edge_in[i].c_in_max - edge_in[i].d_out_current<<endl;
			return edge_in[i].c_in_max - edge_in[i].d_out_current;
		}
	}
	// if not found return 0
	status = GET_NODE_NOT_REACHABLE; 
	return double(0);
}

/* Find (credit interest rate) from other node
* not found = 0
*/
double Node::getCreIntRateFrom(Node* nodeT, Status status){
	for (int i = 0; i < edge_in.size(); i++){
		if (edge_in[i].nodeFrom == nodeT){
			return edge_in[i].interest_rate;
		}
	}
	// if not found return 0
	status = GET_NODE_NOT_REACHABLE; 
	return double(0);
}

/* Find (current debt) to other node
* not found = 0
*/
double Node::getDebtTo(Node* nodeT, Status &status){
	for (int i = 0; i < edge_in.size(); i++){
		if (edge_in[i].nodeFrom == nodeT){
			return edge_in[i].d_out_current;
		}
	}
	status = GET_NODE_NOT_REACHABLE; 
	return double(0);
}

/* Find (current debt interest rate) to other node
* not found = 0
*/
double Node::getDebIntRateTo(Node* nodeT, Status status){
	for (int i = 0; i < edge_in.size(); i++){
		if (edge_in[i].nodeFrom == nodeT){
			return edge_in[i].interest_rate;
		}
	}
	status = GET_NODE_NOT_REACHABLE; 
	return double(0);
}

///////////////////////////////////////////////////////////////
/* Basic operations, single change 
 * public functions
 */
///////////////////////////////////////////////////////////////
/* Set (credit remained) to other node
* not found = 0
*/
double Node::getCreditTo(Node* nodeT, Status &status){
	for (int i = 0; i < edge_out.size(); i++){
		if (edge_out[i].nodeTo == nodeT){
			return edge_out[i].c_out_max - edge_out[i].d_in_current;
		}
	}
	status = GET_NODE_NOT_REACHABLE; 
	return double(0);
}

/* Set (credit interest rate) to other node
* not found = 0
*/
double Node::getCreIntRateTo(Node* nodeT, Status status){
	for (int i = 0; i < edge_out.size(); i++){
		if (edge_out[i].nodeTo == nodeT){
			return edge_out[i].interest_rate;
		}
	}
	status = GET_NODE_NOT_REACHABLE; 
	return double(0);
}

/* Set (current debt) from other node
* Input : value, operation
* Modify : d_in_current =/+=/-= value
* must be valid
*/
Status Node::setDebtFrom(Node* nodeT, double value, OpMode mode){
	try{
		for (int i = 0; i < edge_out.size(); i++){
			if (edge_out[i].nodeTo == nodeT){
				switch (mode){
				case ADD:
					if (edge_out[i].d_in_current + value > edge_out[i].c_out_max)
						throw INVALID_OP; 
					edge_out[i].d_in_current += value; break;
				case SUB:
                        //cout<<"diff: "<<edge_out[i].d_in_current <<" "<< value<<endl;
                    if (edge_out[i].d_in_current - value < -0.000001)
						throw INVALID_OP;
					edge_out[i].d_in_current -= value; break;
				case EQ:
					edge_out[i].d_in_current = value; break;
				default:
					throw OP_NOT_DEFINE;
				}
			}
		}
	}
	catch (Status error){
		//cout << "something is wrong :(" << endl;
		return error; 
	} 
	return GOOD;
}

/* Set (current debt interest rate) from other node
* Input : value, operation
* Modify : interest_rate =/+=/-= value
* must be valid
*/
Status Node::setDebIntRateFrom(Node* nodeT, double value, OpMode mode){
	try{
		for (int i = 0; i < edge_out.size(); i++){
			if (edge_out[i].nodeTo == nodeT){
				switch (mode){
				case ADD:
					edge_out[i].interest_rate += value; break;
				case SUB:
					if (edge_out[i].interest_rate - value < -0.000001)
						throw INVALID_OP;
					edge_out[i].interest_rate -= value; break;
				case EQ:
					edge_out[i].interest_rate = value; break;
				default:
					throw OP_NOT_DEFINE;
				}
			}
		}
	}
	catch (Status error){
		//cout << "something is wrong :(" << endl;
		return error; 
	} 
	return GOOD;
}


/* Set (credit remained) from other node
* Input : value, operation
* Modify : c_in_max =/+=/-= value
* must be valid
*/
Status Node::setCreditFrom(Node* nodeT, double value, OpMode mode){
	try{
		for (int i = 0; i < edge_in.size(); i++){
			if (edge_in[i].nodeFrom == nodeT){
				switch (mode){
				case ADD:
					edge_in[i].c_in_max += value; break;
				case SUB:
					if (edge_in[i].c_in_max - value < edge_in[i].d_out_current)
						throw INVALID_OP; 
					edge_in[i].c_in_max -= value; break;
				case EQ:
					edge_in[i].c_in_max = value; break;
				default:
					throw OP_NOT_DEFINE;
				}
			}
		}
	}
	catch (Status error){
		//cout << "something is wrong :(" << endl;
		return error; 
	}
	return GOOD;
}

/* Set (credit remained interest rate) from other node
* Input : value, operation
* Modify : interest_rate =/+=/-= value
* must be valid
*/
Status Node::setCreIntRateFrom(Node* nodeT, double value, OpMode mode){
	try{
		for (int i = 0; i < edge_in.size(); i++){
			if (edge_in[i].nodeFrom == nodeT){
				switch (mode){
				case ADD:
					edge_in[i].interest_rate += value; break;
				case SUB:
					if (edge_in[i].interest_rate - value < -0.000001)
						throw INVALID_OP; 
					edge_in[i].interest_rate -= value; break;
				case EQ:
					edge_in[i].interest_rate = value; break;
				default:
					throw OP_NOT_DEFINE;
				}
			}
		}
	}
	catch (Status error){
		//cout << "something is wrong :(" << endl;
		return error; 
	}
	return GOOD;
}


/* Set (current debt) to other node
* Input : value, operatino
* Modify : d_out_current =/+=/-= value
* must be valid
*/
Status Node::setDebtTo(Node* nodeT, double value, OpMode mode){
	try{
		for (int i = 0; i < edge_in.size(); i++){
			if (edge_in[i].nodeFrom == nodeT){
				switch (mode){
				case ADD:
					if (edge_in[i].d_out_current + value > edge_in[i].c_in_max)
						throw INVALID_OP; 
					edge_in[i].d_out_current += value; break;
				case SUB:
					if (edge_in[i].d_out_current - value < -0.000001)
						throw INVALID_OP; 
					edge_in[i].d_out_current -= value; break;
				case EQ:
					edge_in[i].d_out_current = value; break;
				default:
					throw OP_NOT_DEFINE;
				}
			}
		}
	}
	catch (Status error){
		//cout << "something is wrong :(" << endl;
		return error; 
	}
	return GOOD;
}

/* Set (current debt interest rate) to other node
* Input : value, operatino
* Modify : interest_rate =/+=/-= value
* must be valid
*/
Status Node::setDebIntRateTo(Node* nodeT, double value, OpMode mode){
	try{
		for (int i = 0; i < edge_in.size(); i++){
			if (edge_in[i].nodeFrom == nodeT){
				switch (mode){
				case ADD:
					edge_in[i].interest_rate += value; break;
				case SUB:
					if (edge_in[i].interest_rate - value < -0.000001)
						throw INVALID_OP; 
					edge_in[i].interest_rate -= value; break;
				case EQ:
					edge_in[i].interest_rate = value; break;
				default:
					throw OP_NOT_DEFINE;
				}
			}
		}
	}
	catch (Status error){
		//cout << "something is wrong :(" << endl;
		return error; 
	}
	return GOOD;
}

/* Set (credit remained) to other node
* Input : value, op
* Modify : c_out_max =/+=/-= value
* must be valid
*/
Status Node::setCreditTo(Node* nodeT, double value, OpMode mode){
	try{
		for (int i = 0; i < edge_out.size(); i++){
			if (edge_out[i].nodeTo == nodeT){
				switch (mode){
				case ADD:
					edge_out[i].c_out_max += value; break;
				case SUB:
					if (edge_out[i].c_out_max - value < edge_out[i].d_in_current)
						throw INVALID_OP; 
					edge_out[i].c_out_max -= value; break;
				case EQ:
					edge_out[i].c_out_max = value; break;
				default:
					throw OP_NOT_DEFINE;
				}
			}
		}
	}
	catch (Status error){
		//cout << "something is wrong :(" << endl;
	}
	return GOOD;
}

/* Set (credit remained interest rate) to other node
* Input : value, op
* Modify : interest_rate =/+=/-= value
* must be valid
*/
Status Node::setCreIntRateTo(Node* nodeT, double value, OpMode mode){
	try{
		for (int i = 0; i < edge_out.size(); i++){
			if (edge_out[i].nodeTo == nodeT){
				switch (mode){
				case ADD:
					edge_out[i].interest_rate += value; break;
				case SUB:
					if (edge_out[i].interest_rate - value < -0.000001)
						throw INVALID_OP; 
					edge_out[i].interest_rate -= value; break;
				case EQ:
					edge_out[i].interest_rate = value; break;
				default:
					throw OP_NOT_DEFINE;
				}
			}
		}
	}
	catch (Status error){
		//cout << "something is wrong :(" << endl;
	}
	return GOOD;
}