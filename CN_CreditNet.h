#ifndef CN_CreditNet
#define CN_CreditNet

#include <unordered_map>
#include <list>
#include "CN_Graph.h"

class CreditNet : public Graph{
private:

public:
    CreditNet(int finNumT, int conNumT, int proNumT);
    void init();

    // Inter Bank Trans
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
};


#endif