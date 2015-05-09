#include "CN_WidgetGraph.h"


void WidgetNode::print(){
	cout << "/////////////////////////////////////////////" << endl;
	cout << "WidgetNode, type " << type << " of " << originNode->getNodeID() 
	<< " local ID: " << localID << "global ID: " << nodeID << endl;
	cout << "in size: " << edge_in.size() << endl;
	cout << "out size: " << edge_out.size() << endl;
	for (int i = 0; i < edge_in.size(); ++i){
		cout << " From: " << edge_in[i].nodeFrom->nodeID 
		<< " type " << edge_in[i].nodeFrom->type 
		<< " port: " << edge_in[i].nodeFrom->localID 
		<< " cap: " << edge_in[i].c_in_max << endl;
	}
	for (int i = 0; i < edge_out.size(); ++i){
		cout << " To: " << edge_out[i].nodeTo->nodeID
		<< " type " << edge_out[i].nodeTo->type 
		<< " port: " << edge_out[i].nodeTo->localID 
		<< " cap: " << edge_out[i].c_out_max << endl;
	}
}

void WidgetGraph::print(){
	cout << "////////////////////////// Widget Net //////////////////////////" << endl;
	cout << "num of nodes in widget net: " << nodeList.size() << endl;
	for (int i = 0; i < nodeList.size(); ++i){
		nodeList[i]->print();
	}
}

WidgetGraph::WidgetGraph(){}
WidgetGraph::~WidgetGraph(){
	for (int i = 0; i < nodeList.size(); ++i){
		delete nodeList[i];
	}
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

	for (int k = 0; k < graphT->finAgent.size(); ++k){
		Node* temp = graphT->finAgent[k];
		// cout << "fin agent: " << k << " " << temp->edge_in.size() << " " 
			// << temp->edge_out.size() << endl;
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
	// set node ID
	for (int i = 0; i < this->nodeList.size(); ++i){
		nodeList[i]->nodeID = i;
	}
}

void WidgetGraph::copyBack(){
	for (int k = 0; k < nodeList.size(); ++k){
		WidgetNode* temp = nodeList[k];
		if (temp->type == 1)
			temp->originNode->setOutEdge(temp->edge_out[0].nodeTo->originNode, 
			temp->edge_out[0].c_out_max, temp->edge_out[0].d_in_current, 
			temp->edge_out[0].interest_rate, EQ);
	}
	// clear info in the original graph
	for (int k = 0; k < originGraph->finNum; ++k){
		Node* temp = originGraph->finAgent[k];
		temp->widgetNodeIn.clear();
		temp->widgetNodeOut.clear();
	}
}

/////////////////////////////////////////////////////////////////
// lp-solver
/////////////////////////////////////////////////////////////////
#include <ilcplex/cplex.h>
#include <stdlib.h>
#include <string.h>

/* Forward declaration for function at end of program */

static int
	buildNetwork  (CPXENVptr env, CPXNETptr net, WidgetGraph* widgetNet);

static void
	free_and_null (char **ptr);

int WidgetGraph::lpSolver()
{
	/* Declare variables and arrays for retrieving problem data and
	  solution information later on. */
	cout << "lp solver" << endl;

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
	int       i, j;

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

	status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
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

	status = buildNetwork (env, net, this);

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

	x     = (double *) malloc (narcs  * sizeof (double));
	dj    = (double *) malloc (narcs  * sizeof (double));
	pi    = (double *) malloc (nnodes * sizeof (double));
	slack = (double *) malloc (nnodes * sizeof (double));

	if ( x     == NULL ||
		dj    == NULL ||
		pi    == NULL ||
		slack == NULL   ) {
	  fprintf (stderr, "Failed to allocate arrays.\n");
	  goto TERMINATE;
	}

	status = CPXNETsolution (env, net, &solstat, &objval, x, pi, slack, dj);
	cout << "status: " << status << endl;
	if ( status ) {
	  fprintf (stderr, "Failed to obtain solution.\n");
	  goto TERMINATE;
	}

	cout << "results: " << endl;
	for (int i = 0; i < narcs; ++i){
		cout << x[i] << endl;
	}

	/* Write the output to the screen. */

	printf ("\nSolution status = %d\n", solstat);
	printf ("Solution value  = %f\n\n", objval);

	for (i = 0; i < nnodes; i++) {
	  printf ("Node %2d:  Slack = %10f  Pi = %10f\n", i, slack[i], pi[i]);
	}

	for (j = 0; j < narcs; j++) {
	  printf ("Arc  %2d:  Value = %10f  Reduced cost = %10f\n",
			  j, x[j], dj[j]);
	}

	/* Finally, write a copy of the problem to a file. */

	status = CPXNETwriteprob (env, net, "netex1.net", NULL);
	if ( status ) {
	  fprintf (stderr, "Failed to write network to disk.\n");
	  goto TERMINATE;
	}
	
	
TERMINATE:

	/* Free memory for solution data */

	free_and_null ((char **) &x);
	free_and_null ((char **) &dj);
	free_and_null ((char **) &pi);
	free_and_null ((char **) &slack);

	/* Free up the problem as allocated by CPXNETcreateprob, if necessary */

	if ( net != NULL ) {
	  status = CPXNETfreeprob (env, &net);
	  if ( status ) {
		 fprintf (stderr, "CPXNETfreeprob failed, error code %d.\n", status);
	  }
	}

	/* Free up the CPLEX environment, if necessary */

	if ( env != NULL ) {
	  status = CPXcloseCPLEX (&env);

	  /* Note that CPXcloseCPLEX produces no output,
		 so the only way to see the cause of the error is to use
		 CPXgeterrorstring.  For other CPLEX routines, the errors will
		 be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON. */

	  if ( status ) {
	  char  errmsg[CPXMESSAGEBUFSIZE];
		 fprintf (stderr, "Could not close CPLEX environment.\n");
		 CPXgeterrorstring (env, status, errmsg);
		 fprintf (stderr, "%s", errmsg);
	  }
	}

	return (status);

}  /* END main */



static int
buildNetwork (CPXENVptr env, CPXNETptr net, WidgetGraph* widgetNet)
{
	int status = 0;

	/* definitions to improve readability */

#  define inf    CPX_INFBOUND

	int nnodes = widgetNet->nodeList.size();
	int narcs = 0;
	for (int i = 0; i < nnodes; ++i){
		narcs += widgetNet->nodeList[i]->edge_out.size();
	}

	cout << "num of nodes and arcs; " << nnodes << " " << narcs << endl;
	double * supply = (double *) malloc (nnodes  * sizeof (double));
	int * tail = (int *) malloc (narcs  * sizeof (int));
	int * head = (int *) malloc (narcs  * sizeof (int));
	double * obj = (double *) malloc (narcs  * sizeof (double));
	double * ub = (double *) malloc (narcs  * sizeof (double));
	double * lb = (double *) malloc (narcs  * sizeof (double));

	// initialize supply and demand
	for (int i = 0; i < nnodes; i++){
		if (widgetNet->nodeList[i]->originNode == widgetNet->src){
			supply[i] = -1 * widgetNet->payment;
		} else if (widgetNet->nodeList[i]->originNode == widgetNet->dest){
			supply[i] = 1 * widgetNet->payment;
		} else {
			supply[i] = 0;
		}
	}

	// initialize LP with widget net
	int cnt = 0;
	for (int k = 0; k < nnodes; ++k){
		if (widgetNet->nodeList[k]->originNode == widgetNet->src){
			for (int i = 0; i < widgetNet->nodeList[k]->edge_out.size(); ++i){
				WidgetOutEdge temp = widgetNet->nodeList[k]->edge_out[i];
				tail[cnt] = widgetNet->nodeList[k]->nodeID;
				head[cnt] = temp.nodeTo->nodeID;
				obj[cnt] = 1;
				ub[cnt] = temp.c_out_max;
				lb[cnt] = 0;
				cout << "cnt: " << cnt << endl;
				cnt++;
			}
			continue;
		}
		if (widgetNet->nodeList[k]->originNode == widgetNet->dest){
			for (int i = 0; i < widgetNet->nodeList[k]->edge_out.size(); ++i){
				WidgetOutEdge temp = widgetNet->nodeList[k]->edge_out[i];
				tail[cnt] = widgetNet->nodeList[k]->nodeID;
				head[cnt] = temp.nodeTo->nodeID;
				obj[cnt] = 0;
				ub[cnt] = temp.c_out_max;
				lb[cnt] = 0;
				cout << "cnt: " << cnt << endl;
				cnt++;
			}
			continue;
		}
		for (int i = 0; i < widgetNet->nodeList[k]->edge_out.size(); ++i){
			WidgetOutEdge temp = widgetNet->nodeList[k]->edge_out[i];
			tail[cnt] = widgetNet->nodeList[k]->nodeID;
			head[cnt] = temp.nodeTo->nodeID;
			obj[cnt] = 0;
			ub[cnt] = temp.c_out_max;
			lb[cnt] = 0;
			cout << "cnt: " << cnt << endl;
			cnt++;
		}
	}
	obj[0] = 1;

	cout << "node ids: "; 
	for (int i = 0; i < widgetNet->nodeList.size(); ++i){
		cout << widgetNet->nodeList[i]->nodeID << " ";
	}
	cout << endl;
	cout << "supply: ";
	for (int i = 0; i < nnodes; ++i){
		cout << supply[i] << " ";
	}
	cout << endl;
	cout << "arc: " << endl;
	for (int i = 0; i < narcs; ++i){
		cout << head[i] << " " << tail[i] << ", " 
			<< obj[i] << " " << ub[i] << " " << lb[i] << endl;
	}

	// for (int i = 0; i < narcs; i++){
	// 	tail[i] = 1;
	// 	head[i] = 0;
	// 	obj[i] = 0;
	// 	ub[i] = 0;
	// 	lb[i] = 0;
	// }

	/* Define list of supply values for the nodes */

	// double supply[NNODES] = {20.0, 0.0, 0.0, -15.0, 5.0, 0.0, 0.0, -10.0};

	/* Define list of tail or from-node indices as well as head or
	  to-node indices for the arcs.  Notice that according to C
	  standard the first node has index 0. */

	// int    tail[NARCS] = {   0,    1,    2,    3,    6,    5,    4,
	// 						4,    2,    3,    3,    5,    5,    1};
	// int    head[NARCS] = {   1,    2,    3,    6,    5,    7,    7,
	// 						1,    1,    4,    5,    3,    4,    5};

	/* Define list of objective values and lower and upper bound values
	  for the arcs */

	// double obj [NARCS] = { 3.0,  3.0,  4.0,  3.0,  5.0,  6.0,  7.0,
	// 					  4.0,  2.0,  6.0,  5.0,  4.0,  3.0,  6.0};
	// double ub  [NARCS] = {24.0, 25.0, 12.0, 10.0,  9.0,  inf, 20.0,
	// 					 10.0,  5.0, 15.0, 10.0, 11.0,  6.0,  inf};
	// double lb  [NARCS] = {18.0,  0.0, 12.0,  0.0,  0.0, -inf,  0.0,
						  // 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0};

	/* Delete existing network.  This is not necessary in this
	  context since we know we have an empty network object.
	  Notice that CPXNETdelnodes deletes all arcs incident to
	  the deleted nodes as well.  Therefore this one function
	  call effectively deletes an existing network problem. */

	if ( CPXNETgetnumnodes (env, net) > 0 ) {
	  status = CPXNETdelnodes (env, net, 0,
								CPXNETgetnumnodes (env, net)-1);
	  if ( status ) goto TERMINATE;
	}

	/* Set optimization sense */

	status = CPXNETchgobjsen (env, net, CPX_MIN);
	if ( status ) goto TERMINATE;

	/* Add nodes to network along with their supply values,
	  but without any names. */

	status = CPXNETaddnodes (env, net, nnodes, supply, NULL);
	if ( status ) goto TERMINATE;

	/* Add arcs to network along with their objective values and
	  bounds, but without any names. */

	status = CPXNETaddarcs (env, net, narcs, tail, head, lb, ub, obj, NULL);
	if ( status ) goto TERMINATE;

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