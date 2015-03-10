#ifndef CN_Error
#define CN_Error
#include <iostream>
#include <vector>
// Define error status types

using namespace std;

enum Status{
	GOOD,  TYPE_NOT_DEFINE, 

	// search 
	NO_PATH_FOUND, TARGET_NODE_NOT_EXIST, 

	// get debt, credit
	GET_NODE_NOT_REACHABLE, 

	// set debt, credit
	OP_NOT_DEFINE, NODE_NOT_REACHABLE, INVALID_OP,
    
    // routing a payment
    INSUFFICIENT_FLOW,

	// Consumers
	CON_I_AM_FULL, CON_I_DONT_WANT_TO
};

enum NodeType{
    FINANCIAL, CONSUMER, PRODUCER, BANK, LABOR, DIVIDEND
};

class Utility {
public:
    void print(NodeType nodeType){
        switch (nodeType) {
            case FINANCIAL:
                std::cout<<"FIN ";
                break;
            case CONSUMER:
                std::cout<<"CON ";
                break;
            case PRODUCER:
                std::cout<<"PRO ";
                break;
            case BANK:
                std::cout<<"BAN ";
                break;
            case LABOR:
                std::cout<<"LAB ";
                break;
            case DIVIDEND:
                std::cout<<"DIV ";
                break;
            default: 
                break;
        }
    }

    void print(Status& status){
        switch (status) {
            case GOOD:
                std::cout<<"GOOD"<<std::endl;
                break;
            case TYPE_NOT_DEFINE:
                std::cout<<"TYPE_NOT_DEFINE"<<std::endl;
                break;
            case GET_NODE_NOT_REACHABLE:
                std::cout<<"GET_NODE_NOT_REACHABLE"<<std::endl;
                break;
            case NO_PATH_FOUND:
                std::cout<<"NO_PATH_FOUND"<<std::endl;
                break;
            case TARGET_NODE_NOT_EXIST:
                std::cout<<"TARGET_NODE_NOT_EXIST"<<std::endl;
                break;
            case INVALID_OP:
                std::cout<<"INVALID_OP"<<std::endl;
                break;
            case INSUFFICIENT_FLOW:
                std::cout<<"INSUFFICIENT_FLOW"<<std::endl;
                break;
            default:
                break;
        }
    }
};


class DisGenerator{
public:
    void normalPartition(double total, int cnt, vector<double> & result);
};





#endif