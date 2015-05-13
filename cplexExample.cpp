// -------------------------------------------------------------- -*- C++ -*-
// File: examples/src/ilolpex1.cpp
// Version 8.1
// --------------------------------------------------------------------------
//  Copyright (C) 1999-2002 by ILOG.
//  All Rights Reserved.
//  Permission is expressly granted to use this example in the
//  course of developing applications that use ILOG products.
// --------------------------------------------------------------------------
//
// ilolpex1.cpp - Entering and optimizing a problem.  Demonstrates different
// methods for creating a problem.  The user has to choose the method
// on the command line:
//
//    ilolpex1  -r     generates the problem by adding rows
//    ilolpex1  -c     generates the problem by adding columns
//    ilolpex1  -n     generates the problem by adding a list of coefficients

#include <sstream>

#include <ilcplex/ilocplex.h>
ILOSTLBEGIN

static void populatebyrow(IloModel model, IloNumVarArray var, IloRangeArray con);

int main (int argc, char **argv){
	IloEnv env;
 	try {
		IloModel model(env);
		IloNumVarArray var(env);
		IloRangeArray con(env);
		populatebyrow (model, var, con);
		IloCplex cplex(model);

		// Optimize the problem and obtain solution.
		if ( !cplex.solve() ) {
			 env.error() << "Failed to optimize LP" << endl;
			 throw(-1);
		}

		IloNumArray vals(env);
		// env.out() << "Solution status = " << cplex.getStatus() << endl;
		// env.out() << "Solution value  = " << cplex.getObjValue() << endl;
		cplex.getValues(vals, var);
		env.out() << "Values        = " << vals << endl;

		for (unsigned int i = 0; i < 3; ++i){
			double a = vals[i];
			cout << a << " ";
		}
		cout << endl;
		// cplex.getSlacks(vals, con);
		// env.out() << "Slacks        = " << vals << endl;
		// cplex.getDuals(vals, con);
		// env.out() << "Duals         = " << vals << endl;
		// cplex.getReducedCosts(vals, var);
		// env.out() << "Reduced Costs = " << vals << endl;

		// cplex.exportModel("lpex1.lp");
	}
		catch (IloException& e) {
		cerr << "Concert exception caught: " << e << endl;
	}
	catch (...) {
		cerr << "Unknown exception caught" << endl;
	}
	env.end();
	return 0;
}


// To populate by row, we first create the variables, and then use them to
// create the range constraints and objective.

static void
populatebyrow (IloModel model, IloNumVarArray x, IloRangeArray c)
{
 IloEnv env = model.getEnv();

 x.add(IloNumVar(env, 0.0, 40.0));
 x.add(IloNumVar(env));
 x.add(IloNumVar(env));
 model.add(IloMaximize(env, x[0] + 2 * x[1] + 3 * x[2]));
 int a = 3;
 c.add( 0 * x[0] +     x[1] + x[2] <= 20);
 c.add(   x[0] - a * x[1] + x[2] <= 30);
 model.add(c);

}  // END populatebyrow
