// Hello World example
// This example shows basic usage of DOM-style API.

#include "include/rapidjson/document.h"     // rapidjson's DOM-style API
#include "include/rapidjson/prettywriter.h" // for stringify JSON
#include "include/rapidjson/filewritestream.h"
#include "include/rapidjson/filereadstream.h"
#include <iostream>
#include <string>
#include "CN_CreditNet.h"
#include <iostream>
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
    
    const Value& configObj = doc["configuration"];
    config.numNodes = configObj["numNodes"].GetInt();
    config.edgeProb = configObj["edgeProb"].GetDouble();
    config.window = configObj["window"].GetInt();
    config.smoothing = configObj["smoothing"].GetInt();
    config.numIR = configObj["numIR"].GetInt();
    
    
    
    const Value& a = doc["assignment"];
    const Value& b = a["All"];
    
    // rapidjson uses SizeType instead of size_t.
    for (rapidjson::SizeType i = 0; i < b.Size(); i++)
    {
        config.assignedStrategy.push_back(b[i].GetString());
        printf("%s \n", b[i].GetString());
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
        object.AddMember("roll", "All", allocator);
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

////////////////////////////////////////////////////////////////////
void singleSimulation(
                      int finNum, int conNum, int proNum,
                      double threshold, int numIR, int mechanismGenMode,
                      int window_size,
                      double* resultRate)
{
    // config the network
    CreditNet creditNet(finNum, conNum, proNum);
    creditNet.genTest0Graph(threshold, numIR);
    creditNet.setRoutePreference(mechanismGenMode);
    
    // main loop
    // first window_size runs
    int failRate1 = 0;
    int failRate2 = 0;
    int failRateTotal = 0;
    vector<int> array;
    for (int i = 0; i < window_size; ++i){
        int temp;
        
        temp = creditNet.genInterBankTrans();
        
        array.push_back(temp);
        failRate1 += temp;
        failRateTotal += temp;
    }
    
    for (int i = 0; i < window_size; ++i){
        int temp;
        
        temp = creditNet.genInterBankTrans();
        
        array.push_back(temp);
        failRate2 += temp;
        failRateTotal += temp;
    }
    
    int cnt = 0;
    while (1){
        if (abs(failRate1 - failRate2) <= window_size * 2.0 / 1000.0){
            break;
        }
        // move on
        int temp;
        
        temp = creditNet.genInterBankTrans();
        
        failRate1 = failRate1 - array[0] + array[window_size];
        failRate2 = failRate2 - array[window_size] + temp;
        failRateTotal += temp;
        array.erase(array.begin());
        array.push_back(temp);
        cnt++;
    }
    lock_rates.lock();
    *resultRate = failRateTotal / (cnt + 2.0 * window_size + 1.0);
    lock_rates.unlock();
    lock_cout.lock();
    cout << "new thread: threshold " << threshold << " mechanism " << mechanismGenMode << *resultRate << endl;
    lock_cout.unlock();
    
    vector<PlayerInfo> myList;
    for (int i = 0; i < 10; ++i) {
        PlayerInfo p;
        p.payoff = 10.0;
        p.strategy = "LP_SOURCE";
        p.payoff = 10.0;
        p.role = "All";
        myList.push_back(p);
    }
    writePayoff(myList, "output.json");
}


// argv[1]: initialize mode
// argv[2]: number of interest rates
int main(int argc, char* argv[]){
    
    Config config;
    readConfig(config, "simulation_spec.json");
    
    
    
//    int finNum = 200;
//    int conNum = 0;
//    int proNum = 0;
//    
//    double threshold;
//    int numIR = atoi(argv[2]);
//    int mechanismGenMode = atoi(argv[1]);
//    int window_size = 1000;
//    int iter = 1;
//    double degrees [10] = {0.01,0.02,0.04,0.06,0.09,0.12,0.15,0.20,0.25,0.35};
//    
//    // 10 rounds
//    for (int i = 0; i < 1; ++i){
//        threshold = 0.09;
//        double* rates = new double [iter];
//        vector<std::thread*> threadPool;
//        double rateFinal = 0;
//        
//        // smooth the result
//        for (int j = 0; j < iter; ++j){
//            singleSimulation(finNum, conNum, proNum,
//                             threshold, numIR, mechanismGenMode,
//                             window_size, rates + j);
//        }
//        
//        for (int j = 0; j < iter; j++) {
//            cout << rates[j] << " ";
//            rateFinal += rates[j];
//        }
//        
//        delete [] rates;
//        rateFinal /= iter;
//        
//        cout << endl << (double)(threshold*199) << " " << 1 - rateFinal << endl;
//    }
    
    vector<PlayerInfo> myList;
    for (int i = 0; i < 10; ++i) {
        PlayerInfo p;
        p.payoff = 10.0;
        p.strategy = "LP_SOURCE";
        p.payoff = 10.0;
        p.role = "All";
        myList.push_back(p);
    }
    writePayoff(myList, "output.json");
    
    return 0;
}
