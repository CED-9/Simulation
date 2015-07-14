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
    string numNodes;
    string edgeProb;
    string transVal;
    string window;
    string smoothing;
    string numIR;
    vector<string> assignedStrategy;
};

struct PlayerInfo {
    double payoff;
    string strategy;
    string role;
    int features[10];
};

void readConfig (Config &config, string inPath) {
	// cout<<"trying to read"<<endl;
    FILE* fp = fopen(inPath.c_str(), "r"); // non-Windows use "r"
	// cout<<"fopened"<<endl;
    char readBuffer[65536];
    FileReadStream is(fp, readBuffer, sizeof(readBuffer));
    Document doc;
    doc.ParseStream(is);
    fclose(fp);
	// cout<<"fclosed"<<endl;

    printf("\nModified JSON with reformatting:\n");
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    doc.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    puts(sb.GetString());
    
    
    const Value& configObj = doc["configuration"];
    config.numNodes = configObj["numNodes"].GetString();
    config.edgeProb = configObj["edgeProb"].GetString();
    config.transVal = configObj["transVal"].GetString();
    config.window = configObj["window"].GetString();
    config.smoothing = configObj["smoothing"].GetString();
    config.numIR = configObj["numIR"].GetString();
    
    
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
		rapidjson::Value s;
		s = rapidjson::StringRef(players[i].strategy.c_str());
        rapidjson::Value object(rapidjson::kObjectType);
        object.SetObject();
        object.AddMember("role", "All", allocator);
		// cout<<players[i].strategy<<endl;
		// cout<<players[i].strategy.c_str()<<endl;
        object.AddMember("strategy", s, allocator);
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
	// cout<<json_folder<<endl;
    int num_obs = atoi(argv[2]);
	// cout <<  num_obs << endl;
    Config config;
	// cout << "configed" << endl;
    readConfig(config, json_folder+"/simulation_spec.json");
	// cout << "spec read" << endl;
	
    for (int i = 0; i < num_obs; ++i){

        int finNum = stoi(config.numNodes);
		std::vector<double> payoffs(finNum,0.0);

       double transVal = stod(config.transVal);
       int conNum = 0;
       int proNum = 0;
       
       double threshold = stod(config.edgeProb);
       int numIR = stoi(config.numIR);
       int window_size = stoi(config.window);
       int iter = stoi(config.smoothing);
       
       // config the network
       for (int j = 0; j < iter; ++j){
         CreditNet creditNet(finNum, conNum, proNum);
          for (int i = 0; i<finNum; ++i){
         creditNet.finAgent[i]->transactionNum = 0;
       }
         creditNet.genTest0Graph(threshold, numIR);
         
         creditNet.setRoutePreference(5, config.assignedStrategy);
         // for (int o = 0; o<finNum;++o){
			 // creditNet.finAgent[o]->routePreference = config.assignedStrategy[o];
		 // }
         // main loop
         int failRateTotal = 0;

            for (int i = 0; i < window_size; ++i){
               int temp;
               temp = creditNet.genInterBankTrans();
               failRateTotal += temp;
           }
           cout << window_size - failRateTotal << "   "<<endl;;
            for (int k = 0; k < finNum; ++k){
               // cout << creditNet.finAgent[k]->transactionNum << "  " << creditNet.finAgent[k]->getCurrBanlance()<<"   ";
               payoffs[k] += (double) creditNet.finAgent[k]->transactionNum * (double)transVal + (double)creditNet.finAgent[k]->getCurrBanlance();
			   // cout  << payoffs[k] << "   ";
			}
			// cout << endl;
	   }
        vector<PlayerInfo> myList;
        for (int k = 0; k < finNum; ++k) {
            PlayerInfo p;
            p.strategy = config.assignedStrategy[k];
            p.payoff = (double)payoffs[k]/(double)iter;
            p.role = "All";
            myList.push_back(p);
        }
       writePayoff(myList, json_folder + "/observation" + std::to_string(i) + ".json");
        
    }
        return 0;

}   
   