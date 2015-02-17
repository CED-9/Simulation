#ifndef CN_CreditNet
#define CN_CreditNet

#include"CN_Graph.h"
#include<list>

class CreditNet : public Graph{
private:
	// producer act
	void proPayLab(ProNode* p);
	void proPayDiv(ProNode* p);
	void labPayCon();

public:
	CreditNet(int finNumT, int conNumT, int proNumT)
		: Graph(finNumT, conNumT, proNumT){}
	void init();
	void update();
	void genTrans();
	void payCosts();
	void payDividends();
	void chargeIR();



};


#endif