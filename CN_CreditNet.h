#ifndef CN_CreditNet
#define CN_CreditNet

#include"CN_Graph.h"
#include<list>

class CreditNet : public Graph{
private:

public:
    CreditNet(int finNumT, int conNumT, int proNumT)
        : Graph(finNumT, conNumT, proNumT){}
    void init();

    // Main Loop
    void update();
    void genTrans();
    void genCostAndDivPay();
    void debtCancel();
    void chargeIR(int time);

    // produer
    void proPayLab(ProNode* p);

    // labor
    void labPayCon();
    
    void executeDefault(FinNode* f, int time);

};


#endif