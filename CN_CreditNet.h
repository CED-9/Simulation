#ifndef CN_CreditNet
#define CN_CreditNet

#include <unordered_map>
#include <list>
#include "CN_Graph.h"
#include "CN_WidgetGraph.h"

class CreditNet : public Graph{
private:

public:
    CreditNet(int finNumT, int conNumT, int proNumT);
    ~CreditNet();
    void init();

    // Inter Bank Trans
    // liquidity test
    int genInterBankTransGreedy();
    int genInterBankTransWidget();
    int genInterBankTrans();

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

    ///////////////////////////////////////////////
    void printTranPerCon();
    void printProPrice();
    void printBanlancePerCon();
    
    void printPayoff();
};


#endif