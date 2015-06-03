#include "CN_WidgetGraph.h"
#define CREDIT_MAX 100;

void WidgetNode::print(){
	cout << "/////////////////////////////////////////////" << endl;
	cout << "WidgetNode, type " << type << " of " << originNode->getNodeID() 
	<< " local ID: " << localID << " global ID: " << nodeID << endl;
	cout << "in size: " << edge_in.size() << endl;
	cout << "out size: " << edge_out.size() << endl;
	for (int i = 0; i < edge_in.size(); ++i){
		cout << " From: " << edge_in[i].nodeFrom->nodeID 
		<< " type " << edge_in[i].nodeFrom->type 
		<< " port: " << edge_in[i].nodeFrom->localID 
		<< " cap: " << edge_in[i].cap 
		<< " curr: " << edge_in[i].curr << endl;
	}
	for (int i = 0; i < edge_out.size(); ++i){
		cout << " To: " << edge_out[i].nodeTo->nodeID
		<< " type " << edge_out[i].nodeTo->type 
		<< " port: " << edge_out[i].nodeTo->localID 
		<< " cap: " << edge_out[i].cap 
		<< " curr: " << edge_out[i].curr << endl;
	}
}

void WidgetGraph::print(){
	cout << "////////////////////////// Widget Net //////////////////////////" << endl;
	cout << "num of nodes in widget net: " << nodeList.size() << endl;
	int cnt = 0;
	for (int i = 0; i < nodeList.size(); ++i){
		cnt += nodeList[i]->edge_out.size();
	}
	cout << "num of edges: " << cnt << endl;
	for (int i = 0; i < nodeList.size(); ++i){
		nodeList[i]->print();
	}
}

WidgetGraph::WidgetGraph(){}
WidgetGraph::~WidgetGraph(){
	// clear info in the original graph
	for (int k = 0; k < originGraph->finNum; ++k){
		Node* temp = originGraph->finAgent[k];
		temp->widgetNode_In_In.clear();
		temp->widgetNode_In_Out.clear();
		temp->widgetNode_Out_In.clear();
		temp->widgetNode_Out_Out.clear();
	}
	for (int i = 0; i < nodeList.size(); ++i){
		nodeList[i]->edge_out.clear();
		nodeList[i]->edge_in.clear();
		delete nodeList[i];
	}
	nodeList.clear();
}

void WidgetGraph::setUpSrcAndDest(Node* srcT, Node* destT, double pay){
	this->src = srcT;
	this->dest = destT;
	this->payment = pay;
}

void WidgetGraph::constructWidget(Graph* graphT){
	this->originGraph = graphT;
	// cout << "constructWidget..." << endl;
	// cout << "num of fin agents " << graphT->finAgent.size() << endl;
	// WidgetNode* superSrc = new WidgetNode(1, nullptr, -1);
	// WidgetNode* superDest = new WidgetNode(0, nullptr, -1);
	// this->nodeList.push_back(superSrc);  
	// this->nodeList.push_back(superDest);

	// Okay letsconstruct Widget nodes
	for (int k = 0; k < graphT->finAgent.size(); ++k){
		Node* temp = graphT->finAgent[k];
		// cout << "fin agent: " << k << " " << temp->edge_in.size() << " " 
			// << temp->edge_out.size() << endl;
		// cout << "construct node " << temp->getNodeID() << endl;

		WidgetNode* superWidgetNodeSrc = new WidgetNode(2, temp, -1);
		WidgetNode* superWidgetNodeDest = new WidgetNode(3, temp, -1);
		this->nodeList.push_back(superWidgetNodeSrc);
		this->nodeList.push_back(superWidgetNodeDest);

		for (int i = 0; i < temp->edge_in.size(); ++i){
			// cout << "construct widget node at port in " << i << endl;

			WidgetNode* wNode1 = new WidgetNode(0, temp, i);  // In_In
			WidgetNode* wNode2 = new WidgetNode(1, temp, i);  // In_Out
			temp->widgetNode_In_In.push_back(wNode1);
			temp->widgetNode_In_Out.push_back(wNode2);
			this->nodeList.push_back(wNode1);
			this->nodeList.push_back(wNode2);
		}
		for (int i = 0; i < temp->edge_out.size(); ++i){
			// cout << "construct widget node at port out " << i << endl;
			WidgetNode* wNode1 = new WidgetNode(0, temp, i);  // In_In
			WidgetNode* wNode2 = new WidgetNode(1, temp, i);  // In_Out
			temp->widgetNode_Out_In.push_back(wNode1);
			temp->widgetNode_Out_Out.push_back(wNode2);
			this->nodeList.push_back(wNode1);
			this->nodeList.push_back(wNode2);
		}

		// cout << "begin super nodes" << endl;
		// superSrc <- _In, superDest -> _Out
		for (int i = 0; i < temp->widgetNode_In_In.size(); ++i){
			WidgetOutEdge wEdgeOut;
			wEdgeOut.nodeTo = superWidgetNodeSrc;
			wEdgeOut.cap = CREDIT_MAX;
			wEdgeOut.curr = 0;
			wEdgeOut.interest_diff = -temp->edge_in[i].interest_rate;
			temp->widgetNode_In_In[i]->edge_out.push_back(wEdgeOut);

			WidgetInEdge wEdgeIn;
			wEdgeIn.nodeFrom = temp->widgetNode_In_In[i];
			wEdgeIn.cap = wEdgeOut.cap;
			wEdgeIn.curr = 0;
			wEdgeIn.interest_diff = -temp->edge_in[i].interest_rate;
			superWidgetNodeSrc->edge_in.push_back(wEdgeIn);
		}
		for (int i = 0; i < temp->widgetNode_Out_In.size(); ++i){
			WidgetOutEdge wEdgeOut;
			wEdgeOut.nodeTo = superWidgetNodeSrc;
			wEdgeOut.cap = CREDIT_MAX;
			wEdgeOut.curr = 0;
			wEdgeOut.interest_diff = -temp->edge_out[i].interest_rate;
			temp->widgetNode_Out_In[i]->edge_out.push_back(wEdgeOut);

			WidgetInEdge wEdgeIn;
			wEdgeIn.nodeFrom = temp->widgetNode_Out_In[i];
			wEdgeIn.cap = wEdgeOut.cap;
			wEdgeIn.curr = 0;
			wEdgeIn.interest_diff = -temp->edge_out[i].interest_rate;
			superWidgetNodeSrc->edge_in.push_back(wEdgeIn);
		}
		// superDest -> _Out
		for (int i = 0; i < temp->widgetNode_In_Out.size(); ++i){
			WidgetOutEdge wEdgeOut;
			wEdgeOut.nodeTo = temp->widgetNode_In_Out[i];
			wEdgeOut.cap = CREDIT_MAX;
			wEdgeOut.curr = 0;
			wEdgeOut.interest_diff = temp->edge_in[i].interest_rate;
			superWidgetNodeDest->edge_out.push_back(wEdgeOut);

			WidgetInEdge wEdgeIn;
			wEdgeIn.nodeFrom = superWidgetNodeDest;
			wEdgeIn.cap = wEdgeOut.cap;
			wEdgeIn.curr = 0;
			wEdgeIn.interest_diff = temp->edge_in[i].interest_rate;
			temp->widgetNode_In_Out[i]->edge_in.push_back(wEdgeIn);
		}
		for (int i = 0; i < temp->widgetNode_Out_Out.size(); ++i){
			WidgetOutEdge wEdgeOut;
			wEdgeOut.nodeTo = temp->widgetNode_Out_Out[i];
			wEdgeOut.cap = CREDIT_MAX;
			wEdgeOut.curr = 0;
			wEdgeOut.interest_diff = temp->edge_out[i].interest_rate;
			superWidgetNodeDest->edge_out.push_back(wEdgeOut);

			WidgetInEdge wEdgeIn;
			wEdgeIn.nodeFrom = superWidgetNodeDest;
			wEdgeIn.cap = wEdgeOut.cap;
			wEdgeIn.curr = 0;
			wEdgeIn.interest_diff = temp->edge_out[i].interest_rate;
			temp->widgetNode_Out_Out[i]->edge_in.push_back(wEdgeIn);
		}


		// cout << "begin in_in to out_out" << endl;
		// In_In -> Out_Out, Out_In -> In_Out
		for (int i = 0; i < temp->edge_out.size(); ++i){
			for (int j = 0; j < temp->edge_in.size(); ++j){
				if (temp->edge_out[i].interest_rate >= temp->edge_in[j].interest_rate){
					// cout << "pushing port " << j << " to port " << i << endl;
					// In_In -> Out_Out, [j]->[i]
					WidgetOutEdge wEdgeOut;
					wEdgeOut.nodeTo = temp->widgetNode_Out_Out[i];
					wEdgeOut.cap = CREDIT_MAX;
					wEdgeOut.curr = 0;
					wEdgeOut.interest_diff = 
						temp->edge_out[i].interest_rate - temp->edge_in[j].interest_rate;
					temp->widgetNode_In_In[j]->edge_out.push_back(wEdgeOut);

					WidgetInEdge wEdgeIn;
					wEdgeIn.nodeFrom = temp->widgetNode_In_In[j];
					wEdgeIn.cap = wEdgeOut.cap;
					wEdgeIn.curr = 0;
					wEdgeIn.interest_diff = wEdgeOut.interest_diff;
					temp->widgetNode_Out_Out[i]->edge_in.push_back(wEdgeIn);
				}
				if (temp->edge_out[i].interest_rate <= temp->edge_in[j].interest_rate){
					// Out_In -> In_Out, [i]->[j]
					WidgetOutEdge wEdgeOut;
					wEdgeOut.nodeTo = temp->widgetNode_In_Out[j];
					wEdgeOut.cap = CREDIT_MAX;
					wEdgeOut.curr = 0;
					wEdgeOut.interest_diff = 
						- temp->edge_out[i].interest_rate + temp->edge_in[j].interest_rate;
					temp->widgetNode_Out_In[i]->edge_out.push_back(wEdgeOut);

					WidgetInEdge wEdgeIn;
					wEdgeIn.nodeFrom = temp->widgetNode_Out_In[i];
					wEdgeIn.cap = wEdgeOut.cap;
					wEdgeIn.curr = 0;
					wEdgeIn.interest_diff = wEdgeOut.interest_diff;
					temp->widgetNode_In_Out[j]->edge_in.push_back(wEdgeIn);
				}
			}
		}
		// cout << "begin in_in to in_out" << endl;
		// In_In -> In_Out
		for (int i = 0; i < temp->edge_in.size(); ++i){
			for (int j = i+1; j < temp->edge_in.size(); ++j){
				// if (i == j){
				//  continue;
				// }
				if (temp->edge_in[i].interest_rate >= temp->edge_in[j].interest_rate){
					// [i].ir >= [j].ir, [j]->[i]
					WidgetOutEdge wEdgeOut;
					wEdgeOut.nodeTo = temp->widgetNode_In_Out[i];
					wEdgeOut.cap = CREDIT_MAX;
					wEdgeOut.curr = 0;
					wEdgeOut.interest_diff = 
						temp->edge_in[i].interest_rate - temp->edge_in[j].interest_rate;
					temp->widgetNode_In_In[j]->edge_out.push_back(wEdgeOut);

					WidgetInEdge wEdgeIn;
					wEdgeIn.nodeFrom = temp->widgetNode_In_In[j];
					wEdgeIn.cap = wEdgeOut.cap;
					wEdgeIn.curr = 0;
					wEdgeIn.interest_diff = wEdgeOut.interest_diff;
					temp->widgetNode_In_Out[i]->edge_in.push_back(wEdgeIn);
				}
				if (temp->edge_in[i].interest_rate <= temp->edge_in[j].interest_rate){
					// [i].ir <= [j].ir, [i]->[j]
					WidgetOutEdge wEdgeOut;
					wEdgeOut.nodeTo = temp->widgetNode_In_Out[j];
					wEdgeOut.cap = CREDIT_MAX;
					wEdgeOut.curr = 0;
					wEdgeOut.interest_diff = 
						- temp->edge_in[i].interest_rate + temp->edge_in[j].interest_rate;
					temp->widgetNode_In_In[i]->edge_out.push_back(wEdgeOut);

					WidgetInEdge wEdgeIn;
					wEdgeIn.nodeFrom = temp->widgetNode_In_In[i];
					wEdgeIn.cap = wEdgeOut.cap;
					wEdgeIn.curr = 0;
					wEdgeIn.interest_diff = wEdgeOut.interest_diff;
					temp->widgetNode_In_Out[j]->edge_in.push_back(wEdgeIn);
				}
			}
		}
		// cout << "begin out_in to out_out" << endl;
		// Out_In -> Out_Out 
		for (int i = 0; i < temp->edge_out.size(); ++i){
			for (int j = i+1; j < temp->edge_out.size(); ++j){
				// if (i == j){
				//  continue;
				// }
				if (temp->edge_out[i].interest_rate >= temp->edge_out[j].interest_rate){
					// [i].ir >= [j].ir, [j]->[i]
					WidgetOutEdge wEdgeOut;
					wEdgeOut.nodeTo = temp->widgetNode_Out_Out[i];
					wEdgeOut.cap = CREDIT_MAX;
					wEdgeOut.curr = 0;
					wEdgeOut.interest_diff = 
						temp->edge_out[i].interest_rate - temp->edge_out[j].interest_rate;
					temp->widgetNode_Out_In[j]->edge_out.push_back(wEdgeOut);

					WidgetInEdge wEdgeIn;
					wEdgeIn.nodeFrom = temp->widgetNode_Out_In[j];
					wEdgeIn.cap = wEdgeOut.cap;
					wEdgeIn.curr = 0;
					wEdgeIn.interest_diff = wEdgeOut.interest_diff;
					temp->widgetNode_Out_Out[i]->edge_in.push_back(wEdgeIn);
				}
				if (temp->edge_out[i].interest_rate <= temp->edge_out[j].interest_rate){
					// [i].ir <= [j].ir, [i]->[j]
					WidgetOutEdge wEdgeOut;
					wEdgeOut.nodeTo = temp->widgetNode_Out_Out[j];
					wEdgeOut.cap = CREDIT_MAX;
					wEdgeOut.curr = 0;
					wEdgeOut.interest_diff = 
						- temp->edge_out[i].interest_rate + temp->edge_out[j].interest_rate;
					temp->widgetNode_Out_In[i]->edge_out.push_back(wEdgeOut);

					WidgetInEdge wEdgeIn;
					wEdgeIn.nodeFrom = temp->widgetNode_Out_In[i];
					wEdgeIn.cap = wEdgeOut.cap;
					wEdgeIn.curr = 0;
					wEdgeIn.interest_diff = wEdgeOut.interest_diff;
					temp->widgetNode_Out_Out[j]->edge_in.push_back(wEdgeIn);
				}
			}
		}
		// cout << "end" << endl;
	}

	// cout << "finishing inside widget edges" << endl;

	for (int k = 0; k < graphT->finAgent.size(); ++k){
		Node* temp = graphT->finAgent[k];
		// cout << "from node: " << temp->getNodeID() << endl;
		for (int i = 0; i < temp->edge_out.size(); ++i){
			// find sender's port and receiver's port
			WidgetInEdge wInEdge1;
			WidgetInEdge wInEdge2;
			WidgetOutEdge wOutEdge1;
			WidgetOutEdge wOutEdge2;

			wOutEdge1.type = 1;

			// cout << "from port out: " << i << endl;
			Node* tempTarget = temp->edge_out[i].nodeTo;
			int j;
			for (j = 0; j < tempTarget->edge_in.size(); ++j){
				if (temp == tempTarget->edge_in[j].nodeFrom){
					break;
				}
			}

			// cout << "target node: " << tempTarget->getNodeID() << " port in: " << j << endl;
			
			wInEdge1.nodeFrom = temp->widgetNode_Out_Out[i];
			wInEdge1.cap = temp->edge_out[i].c_out_max - temp->edge_out[i].d_in_current;
			wInEdge1.interest_rate = temp->edge_out[i].interest_rate;

			// cout << "1" << endl;

			wInEdge2.nodeFrom = tempTarget->widgetNode_In_Out[j];
			wInEdge2.cap = temp->edge_out[i].d_in_current;
			wInEdge2.interest_rate = wInEdge1.interest_rate;

			// cout << "2" << endl;

			wOutEdge1.nodeTo = tempTarget->widgetNode_In_In[j];
			wOutEdge1.cap = wInEdge1.cap;
			wOutEdge1.interest_rate = wInEdge1.interest_rate;

			// cout << "3" << endl;

			wOutEdge2.nodeTo = temp->widgetNode_Out_In[i];
			wOutEdge2.cap = wInEdge2.cap;
			wOutEdge2.interest_rate = wInEdge2.interest_rate;

			// cout << "4" << endl;

			temp->widgetNode_Out_Out[i]->edge_out.push_back(wOutEdge1);
			tempTarget->widgetNode_In_Out[j]->edge_out.push_back(wOutEdge2);
			tempTarget->widgetNode_In_In[j]->edge_in.push_back(wInEdge1);
			temp->widgetNode_Out_In[i]->edge_in.push_back(wInEdge2);
		}
	}
	// cout << "finishing outside widget edges" << endl;
	// set node ID
	for (int i = 0; i < this->nodeList.size(); ++i){
		nodeList[i]->nodeID = i;
	}
}

void WidgetGraph::copyBack(){

	for (int i = 0; i < originGraph->finNum; ++i){
		Node* temp = originGraph->finAgent[i];

		for (int j = 0; j < temp->widgetNode_Out_Out.size(); ++j){
			// cout << "curr: " << temp->widgetNode_Out_Out[j]->edge_out[0].curr 
			// << " " << temp->widgetNode_Out_In[j]->edge_in[0].curr << endl;
			temp->setOutEdge(temp->widgetNode_Out_Out[j]->edge_out[0].nodeTo->originNode, 
				0, temp->widgetNode_Out_Out[j]->edge_out[0].curr - 
				temp->widgetNode_Out_In[j]->edge_in[0].curr, 0, ADD);
		}
	}

	// cout << "before" << endl;
	// for (int i = 0; i < originGraph->finNum; ++i){
	//  Node* temp = originGraph->finAgent[i];
	//  for (int j = 0; j < temp->edge_in.size(); ++j){
	//      double curr1 = temp->widgetNode_In_In[j]->edge_in[0].curr;
	//      double curr2 = temp->widgetNode_In_Out[j]->edge_out[0].curr;
	//      temp->widgetNode_In_In[j]->edge_in[0].curr = 0;
	//      temp->widgetNode_In_Out[j]->edge_out[0].curr = 0;
	//      temp->widgetNode_In_In[j]->edge_in[0].cap 
	//          = temp->widgetNode_In_In[j]->edge_in[0].cap - curr1 + curr2;
	//      temp->widgetNode_In_Out[j]->edge_out[0].cap 
	//          = temp->widgetNode_In_Out[j]->edge_out[0].cap - curr2 + curr1;
	//  }
	//  for (int j = 0; j < temp->edge_out.size(); ++j){
	//      double curr1 = temp->widgetNode_Out_In[j]->edge_in[0].curr;
	//      double curr2 = temp->widgetNode_Out_Out[j]->edge_out[0].curr;
	//      temp->widgetNode_Out_In[j]->edge_in[0].curr = 0;
	//      temp->widgetNode_Out_Out[j]->edge_out[0].curr = 0;
	//      temp->widgetNode_Out_In[j]->edge_in[0].cap
	//          = temp->widgetNode_Out_In[j]->edge_in[0].cap - curr1 + curr2;
	//      temp->widgetNode_Out_Out[j]->edge_out[0].cap
	//          = temp->widgetNode_Out_Out[j]->edge_out[0].cap - curr2 + curr1;
	//  }
	// }
	// cout << "after" << endl;
	// clear info in the original graph
	// for (int k = 0; k < originGraph->finNum; ++k){
	//  Node* temp = originGraph->finAgent[k];
	//  temp->widgetNodeIn.clear();
	//  temp->widgetNodeOut.clear();
	// }
}

/////////////////////////////////////////////////////////////////
// lp-solver
/////////////////////////////////////////////////////////////////
#include <ilcplex/cplex.h>
#include <stdlib.h>
#include <string.h>

/* Forward declaration for function at end of program */

static int
	buildNetwork (CPXENVptr env, CPXNETptr net, WidgetGraph* widgetNet, 
	double* &supply, int* &head, int* &tail, double* &obj, double* &ub, double* &lb, int opMode);

static void
	free_and_null (char **ptr);

int WidgetGraph::lpSolver(int opMode)
{
	/* Declare variables and arrays for retrieving problem data and
	  solution information later on. */
	// cout << "lp solver" << endl;

	int      narcs;
	int      nnodes;
	int      solstat;
	double   objval;
	double   *x     = NULL;
	double   *pi    = NULL;
	double   *slack = NULL;
	double   *dj    = NULL;

	CPXENVptr env = NULL;
	CPXNETptr net = NULL;
	int       status;
	int status1;
	int       i, j;
	int cnt = 0;

	/* Initialize the CPLEX environment */

	env = CPXopenCPLEX (&status);

	/* If an error occurs, the status value indicates the reason for
	  failure.  A call to CPXgeterrorstring will produce the text of
	  the error message.  Note that CPXopenCPLEX produces no
	  output, so the only way to see the cause of the error is to use
	  CPXgeterrorstring.  For other CPLEX routines, the errors will
	  be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON.  */

	if ( env == NULL ) {
	  char  errmsg[CPXMESSAGEBUFSIZE];
	  fprintf (stderr, "Could not open CPLEX environment.\n");
	  CPXgeterrorstring (env, status, errmsg);
	  fprintf (stderr, "%s", errmsg);
	  goto TERMINATE;
	}

	/* Turn on output to the screen */

	status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_OFF);
	if ( status ) {
	  fprintf (stderr, 
				"Failure to turn on screen indicator, error %d.\n", status);
	  goto TERMINATE;
	}

	/* Create the problem. */

	net = CPXNETcreateprob (env, &status, "netex1");

	/* A returned pointer of NULL may mean that not enough memory
	  was available or there was some other problem.  In the case of 
	  failure, an error message will have been written to the error 
	  channel from inside CPLEX.  In this example, the setting of
	  the parameter CPXPARAM_ScreenOutput causes the error message to
	  appear on stdout.  */

	if ( net == NULL ) {
	  fprintf (stderr, "Failed to create network object.\n");
	  goto TERMINATE;
	}

	/* Fill in the data for the problem.  Note that since the space for
	  the data already exists in local variables, we pass the arrays
	  directly to the routine to fill in the data structures.  */

	double *supply;
	int* head;
	int* tail;
	double* obj;
	double* ub;
	double* lb;
	status = buildNetwork (env, net, this, supply, head, tail, obj, ub, lb, opMode);

	// if(supply != NULL){
	// 	cout << "before not clear" << endl;
	// }

	if ( status ) {
	  fprintf (stderr, "Failed to build network problem.\n");
	  goto TERMINATE;
	}


	/* Optimize the problem and obtain solution. */

	status = CPXNETprimopt (env, net);
	if ( status ) {
	  fprintf (stderr, "Failed to optimize network.\n");
	  goto TERMINATE;
	}

	/* get network dimensions */

	narcs  = CPXNETgetnumarcs  (env, net);
	nnodes = CPXNETgetnumnodes (env, net);

	/* allocate memory for solution data */

	// x     = (double *) malloc (narcs  * sizeof (double));
	// dj    = (double *) malloc (narcs  * sizeof (double));
	// pi    = (double *) malloc (nnodes * sizeof (double));
	// slack = (double *) malloc (nnodes * sizeof (double));

	x     = new double[narcs];
	dj    = new double[narcs];
	pi    = new double[nnodes];
	slack = new double[nnodes];


	status = CPXNETsolution (env, net, &solstat, &objval, x, pi, slack, dj);
	// cout << "status: " << status << endl;
	if ( status ) {
	  // fprintf (stderr, "Failed to obtain solution.\n");
	  goto TERMINATE;
	}

	// cout << "results: " << endl;
	// for (int i = 0; i < narcs; ++i){
	//  cout << x[i] << endl;
	// }

	/* Write the output to the screen. */

	// printf ("\nSolution status = %d\n", solstat);
	if (solstat != 1 && solstat != 6 && solstat != 14){
		// cout << "status" << solstat << endl; 
		status1 = -1;
		goto TERMINATE;
	}
	// printf ("Solution value  = %f\n\n", objval);

	for (int k = 0; k < nnodes; ++k){
		for (int i = 0; i < this->nodeList[k]->edge_out.size(); ++i){
			this->nodeList[k]->edge_out[i].curr = x[cnt];
			WidgetNode* temp = this->nodeList[k]->edge_out[i].nodeTo;
			for (int j = 0; j < temp->edge_in.size(); ++j){
				if (temp->edge_in[j].nodeFrom == this->nodeList[k]){
					temp->edge_in[j].curr = x[cnt];
				}
			}
			cnt++;
		}
	}
	status1 = 0;

	// for (i = 0; i < nnodes; i++) {
	//   printf ("Node %2d:  Slack = %10f  Pi = %10f\n", i, slack[i], pi[i]);
	// }

	// for (j = 0; j < narcs; j++) {
	//   printf ("Arc  %2d:  Value = %10f  Reduced cost = %10f\n",
	//        j, x[j], dj[j]);
	// }

	/* Finally, write a copy of the problem to a file. */

	// status = CPXNETwriteprob (env, net, "netex1.net", NULL);
	// if ( status ) {
	//   fprintf (stderr, "Failed to write network to disk.\n");
	//   goto TERMINATE;
	// }
	
	
TERMINATE:

	delete [] supply;
	delete [] head;
	delete [] tail;
	delete [] obj;
	delete [] ub;
	delete [] lb;

	/* Free memory for solution data */

	delete [] x;
	delete [] dj;
	delete [] pi;
	delete [] slack;

	// free_and_null ((char **) &x);
	// free_and_null ((char **) &dj);
	// free_and_null ((char **) &pi);
	// free_and_null ((char **) &slack);

	/* Free up the problem as allocated by CPXNETcreateprob, if necessary */

	if ( net != NULL ) {
		// cout << "clearing net..." << endl;
	  status = CPXNETfreeprob (env, &net);
	 //  if (net != NULL){
		// cout << "fail to clear net" << endl;
	 //  }
	  if ( status ) {
		 fprintf (stderr, "CPXNETfreeprob failed, error code %d.\n", status);
	  }
	}

	/* Free up the CPLEX environment, if necessary */

	if ( env != NULL ) {
	  status = CPXcloseCPLEX (&env);
	 //  cout << "clearing env..." << endl;
	 //  if (env != NULL){
		// cout << "fail to clear env" << endl;
	 //  }
	//    Note that CPXcloseCPLEX produces no output,
	//   so the only way to see the cause of the error is to use
	//   CPXgeterrorstring.  For other CPLEX routines, the errors will
	//   be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. 

	// if(supply != NULL){
	// 	cout << "after not clear" << endl;
	// }

	  if ( status ) {
	  char  errmsg[CPXMESSAGEBUFSIZE];
		 fprintf (stderr, "Could not close CPLEX environment.\n");
		 CPXgeterrorstring (env, status, errmsg);
		 fprintf (stderr, "%s", errmsg);
	  }
	}

	return (status1);

}  /* END main */



static int
buildNetwork (CPXENVptr env, CPXNETptr net, WidgetGraph* widgetNet, 
	double * &supply, int* &head, int* &tail, double* &obj, double* &ub, double* &lb, int opMode)
{
	int status = 0;

	/* definitions to improve readability */

#  define inf    CPX_INFBOUND

	int nnodes = widgetNet->nodeList.size();
	int narcs = 0;
	for (int i = 0; i < nnodes; ++i){
		narcs += widgetNet->nodeList[i]->edge_out.size();
	}

	// cout << "num of nodes and arcs; " << nnodes << " " << narcs << endl;
	// supply = (double *) malloc (nnodes  * sizeof (double));
	// tail = (int *) malloc (narcs  * sizeof (int));
	// head = (int *) malloc (narcs  * sizeof (int));
	// obj = (double *) malloc (narcs  * sizeof (double));
	// ub = (double *) malloc (narcs  * sizeof (double));
	// lb = (double *) malloc (narcs  * sizeof (double));

	supply = new double [nnodes];
	tail = new int [narcs];
	head = new int [narcs];
	obj = new double [narcs];
	ub = new double [narcs];
	lb = new double [narcs];

	// initialize supply and demand
	for (int i = 0; i < nnodes; i++){
		if (widgetNet->nodeList[i]->originNode == widgetNet->src 
			&& widgetNet->nodeList[i]->type == 2){
			supply[i] = 1 * widgetNet->payment;
		} else if (widgetNet->nodeList[i]->originNode == widgetNet->dest
			&& widgetNet->nodeList[i]->type == 3){
			supply[i] = -1 * widgetNet->payment;
		} else {
			supply[i] = 0;
		}
	}

	// initialize LP with widget net
	int cnt = 0;

	for (int k = 0; k < nnodes; ++k){
		for (int i = 0; i < widgetNet->nodeList[k]->edge_out.size(); ++i){
			WidgetOutEdge temp = widgetNet->nodeList[k]->edge_out[i];
			head[cnt] = widgetNet->nodeList[k]->nodeID;
			tail[cnt] = temp.nodeTo->nodeID;
			
			// obj[cnt] = temp.interest_diff;
            if (opMode == 1) {
                if (widgetNet->nodeList[k]->originNode == widgetNet->src
                    && temp.nodeTo->type == 2){
                    obj[cnt] = temp.interest_diff;
                } else {
                    obj[cnt] = 0;
                }
            } else {
                obj[cnt] = temp.interest_diff;
            }
            

			ub[cnt] = temp.cap;
			lb[cnt] = 0;
			// cout << "cnt: " << cnt << endl;
			cnt++;
		}
	}

	// cout << "node ids: "; 
	// for (int i = 0; i < widgetNet->nodeList.size(); ++i){
	//  cout << widgetNet->nodeList[i]->nodeID << " ";
	// }
	// cout << endl;
	// cout << "supply: ";
	// for (int i = 0; i < nnodes; ++i){
	//  cout << supply[i] << " ";
	// }
	// cout << endl;
	// cout << "arc: " << endl;
	// for (int i = 0; i < narcs; ++i){
	//  cout << head[i] << " " << tail[i] << ", " 
	//      << obj[i] << " " << ub[i] << " " << lb[i] << endl;
	// }

	if ( CPXNETgetnumnodes (env, net) > 0 ) {
	  status = CPXNETdelnodes (env, net, 0,
								CPXNETgetnumnodes (env, net)-1);
	  if ( status ) goto TERMINATE;
	}

	/* Set optimization sense */

	status = CPXNETchgobjsen (env, net, CPX_MAX);
	if ( status ) goto TERMINATE;

	/* Add nodes to network along with their supply values,
	  but without any names. */

	status = CPXNETaddnodes (env, net, nnodes, supply, NULL);
	if ( status ) goto TERMINATE;

	/* Add arcs to network along with their objective values and
	  bounds, but without any names. */

	status = CPXNETaddarcs (env, net, narcs, tail, head, lb, ub, obj, NULL);
	if ( status ) goto TERMINATE;

	// if (supply != NULL){
	// 	cout << "before exit build, supply not empty" << endl;
	// }

TERMINATE:

	return (status);

}  /* END buildnetwork */


static void
free_and_null (char **ptr)
{
	if ( *ptr != NULL ) {
	  free (*ptr);
	  *ptr = NULL;
	}
} /* END free_and_null */