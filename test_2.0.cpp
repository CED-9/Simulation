// Hello World example
// This example shows basic usage of DOM-style API.

#include "include/rapidjson/document.h"     // rapidjson's DOM-style API
#include "include/rapidjson/prettywriter.h" // for stringify JSON
#include "include/rapidjson/filewritestream.h"
#include "include/rapidjson/filereadstream.h"
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <cmath>
#include <thread>
#include <algorithm>
#include <thread>
#include <mutex>


using namespace rapidjson;
using namespace std;

struct Config {
    int numNodes;
    double edgeProb;
    double transVal;
    int window;
    int smoothing;
    int numIR;
    vector<string> assignedStrategy;
};

struct PlayerInfo {
    double payoff;
    string strategy;
    string role;
    int features[10];
};

void readConfig (Config &config, string inPath) {
    FILE* fp = fopen("simulation_spec.json", "r"); // non-Windows use "r"
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document doc;
    doc.ParseStream(is);
    fclose(fp);
    
    printf("\nModified JSON with reformatting:\n");
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    doc.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    puts(sb.GetString());
    
    
    const Value& configObj = doc["configuration"];
    config.numNodes = configObj["numNodes"].GetInt();
    config.edgeProb = configObj["edgeProb"].GetDouble();
    config.transVal = configObj["transVal"].GetDouble();
    config.window = configObj["window"].GetInt();
    config.smoothing = configObj["smoothing"].GetInt();
    config.numIR = configObj["numIR"].GetInt();
    
    
    const Value& a = doc["assignment"];
    const Value& b = a["All"];
    
    // rapidjson uses SizeType instead of size_t.
    for (rapidjson::SizeType i = 0; i < b.Size(); i++)
    {
        config.assignedStrategy.push_back(b[i].GetString());
        // printf("%s \n", b[i].GetString());
    }
}

void writePayoff (std::vector<PlayerInfo> &players, string outPath) {
    
    rapidjson::Document result;
    result.SetObject();
    rapidjson::Value playerArray(rapidjson::kArrayType);
    rapidjson::Document::AllocatorType& allocator = result.GetAllocator();
    
    for (int i = 0; i < players.size(); ++i) {
        // create a rapidjson object type
        rapidjson::Value object(rapidjson::kObjectType);
        object.SetObject();
        object.AddMember("role", "All", allocator);
        object.AddMember("strategy", "LP_OVERALL", allocator);
        object.AddMember("payoff", players[i].payoff, allocator);
        playerArray.PushBack(object, allocator);
    }
    
    result.AddMember("players", playerArray, allocator);
    
    rapidjson::Value object(rapidjson::kObjectType);
    object.SetObject();
    result.AddMember("features", object, allocator);
    printf("\nModified JSON with reformatting:\n");
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    result.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    puts(sb.GetString());
    
    FILE* fp = fopen(outPath.c_str(), "w"); // non-Windows use "w"
    char writeBuffer[65536];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    Writer<FileWriteStream> writer1(os);
    result.Accept(writer1);
    fclose(fp);

}


int defaultList[100];
ofstream fout_trans;
ofstream fout_int;

mutex lock_rates;
mutex lock_cout;

#include "CN_CreditNet.h"

int main(int argc, char* argv[]){
    
    std::string json_folder = argv[1];
    int num_obs = atoi(argv[2]);
	// cout << json_folder << "   " << num_obs << endl;
    Config config;
    readConfig(config, json_folder+"/simulation_spec.json");

    for (int i = 0; i < num_obs; ++i){
     std::vector<double> payoffs(finNum,0.0);
      
        int finNum = config.numNodes;

       double transVal = config.transVal;
       int conNum = 0;
       int proNum = 0;
       
       double threshold = config.edgeProb;
       int numIR = config.numIR;
       int window_size = config.window;
       int iter = config.smoothing;
       
       // config the network
       for (int j = 0; j < iter; ++j){
         CreditNet creditNet(finNum, conNum, proNum);
          for (int i = 0; i<finNum; ++i){
         creditNet.finAgent[i]->transactionNum = 0;
       }
         creditNet.genTest0Graph(threshold, numIR);
         
         creditNet.setRoutePreference(5, config.assignedStrategy);
         
         // main loop
         int failRateTotal = 0;

            for (int i = 0; i < window_size; ++i){
               int temp;
               temp = creditNet.genInterBankTrans();
               failRateTotal += temp;
           }
           // cout << failRateTotal/(double) window_size << endl;
            for (int k = 0; k < finNum; ++k){
               // cout << creditNet.finAgent[k]->transactionNum << "  " << creditNet.finAgent[k]->getCurrBanlance() << endl;
               payoffs[k] += (double) creditNet.finAgent[k]->transactionNum * (double)transVal + (double)creditNet.finAgent[k]->getCurrBanlance();
			}
	   }
        vector<PlayerInfo> myList;
        for (int i = 0; i < finNum; ++i) {
            PlayerInfo p;
            p.strategy = config.assignedStrategy[i];
            p.payoff = (double)payoffs[i]/(double)iter;
            p.role = "All";
            myList.push_back(p);
        }
       writePayoff(myList, json_folder + "/observation" + std::to_string(i) + ".json");
        
    }
        return 0;

}   
   