#ifndef CN_CreditNet
#define CN_CreditNet

#include <unordered_map>
#include <list>
#include "CN_Graph.h"
#include "CN_WidgetGraph.h"
#include "CN_DistributionGenerator.h"

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
    int testInterBankTransWidget(int fid1, int fid2);
    int genInterBankTrans();
	int genCensoredTrans(double irCap);

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