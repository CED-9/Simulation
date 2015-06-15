// Hello World example
// This example shows basic usage of DOM-style API.

#include "include/rapidjson/document.h"     // rapidjson's DOM-style API
#include "include/rapidjson/prettywriter.h" // for stringify JSON
#include "include/rapidjson/filewritestream.h"
#include <iostream>
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

void output(const rapidjson::Document & document)
{
    // treat object types similar to std::map when querying
    std::cout << document["hello"].GetString() << std::endl;
    std::cout << document["number"].GetInt() << std::endl;
    
    // requires SizeType since the literal zero in c++ can mean a
    // numeric type (int, unsigned, etc.) or a null pointer of any type
    std::cout << document["array"][rapidjson::SizeType(0)].GetString() << std::endl;
    
    std::cout << document["array"][1].GetString() << std::endl;
    
    std::cout << document["object"]["hello"].GetString() << std::endl;
    printf("\nModified JSON with reformatting:\n");
    StringBuffer sb;
    PrettyWriter<StringBuffer> writer(sb);
    document.Accept(writer);    // Accept() traverses the DOM and generates Handler events.
    puts(sb.GetString());
    
    // ...
    FILE* fp = fopen("output.json", "w"); // non-Windows use "w"
    char writeBuffer[65536];
    FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
    Writer<FileWriteStream> writer1(os);
    document.Accept(writer1);
    fclose(fp);
}

struct PlayerInfo {
    double payoff;
    int strategy;
    string roll;
    int features[10];
};

void writePayoff (std::vector<PlayerInfo> &players, int playerNum, double unitTransactionPayoff) {
    
    
//    rapidjson::Document result;
//    result.SetObject();
//    rapidjson::Value playerArray(rapidjson::kArrayType);
//    rapidjson::Document::AllocatorType& allocator = result.GetAllocator();
//    for (int i = 0; i < playerNum; ++i) {
//        array.PushBack("hello", allocator).PushBack("world", allocator);
//        
//        // create a rapidjson object type
//        rapidjson::Value object(rapidjson::kObjectType);
//        object.AddMember("roll", "All", allocator);
//        object.AddMember("strategy", players[i].strategy, allocator);
//        object.AddMember("payoff", players.payoff, allocator);
//        
//        
//        fromScratch.AddMember("object", object, allocator);
//        fromScratch["object"]["hello"] = "world";
//        
//    }
 
    
    
//    // ---- create from scratch ----
//    
//    // document is the root of a json message
//    rapidjson::Document fromScratch;
//    
//    // define the document as an object rather than an array
//    fromScratch.SetObject();
//    
//    // create a rapidjson array type with similar syntax to std::vector
//    rapidjson::Value array(rapidjson::kArrayType);
//    
//    // must pass an allocator when the object may need to allocate memory
//    rapidjson::Document::AllocatorType& allocator = fromScratch.GetAllocator();
//    
//    // chain methods as rapidjson provides a fluent interface when modifying its objects
//    array.PushBack("hello", allocator).PushBack("world", allocator);
//    
//    fromScratch.AddMember("hello", "world", allocator);
//    fromScratch.AddMember("number", 2, allocator);
//    fromScratch.AddMember("array", array, allocator);
//    
//    // create a rapidjson object type
//    rapidjson::Value object(rapidjson::kObjectType);
//    object.AddMember("hello", "world", allocator);
//    fromScratch.AddMember("object", object, allocator);
//    fromScratch["object"]["hello"] = "world";
//    
//    output(fromScratch);
//    
//    // ---- parse from string ----
//    
//    // Convert JSON document to string
//    rapidjson::StringBuffer strbuf;
//    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
//    fromScratch.Accept(writer);
//    
//    // parse json string
//    rapidjson::Document parsedFromString;
//    parsedFromString.Parse<0>(strbuf.GetString());
//    
//    output(parsedFromString);
}


int main(int, char*[]) {
    // ---- create from scratch ----
    
    // document is the root of a json message
    rapidjson::Document fromScratch;
    
    // define the document as an object rather than an array
    fromScratch.SetObject();
    
    // create a rapidjson array type with similar syntax to std::vector
    rapidjson::Value array(rapidjson::kArrayType);
    
    // must pass an allocator when the object may need to allocate memory
    rapidjson::Document::AllocatorType& allocator = fromScratch.GetAllocator();
    
    // chain methods as rapidjson provides a fluent interface when modifying its objects
    array.PushBack("hello", allocator).PushBack("world", allocator);
    
    
    // create a rapidjson object type
    rapidjson::Value object(rapidjson::kObjectType);
    object.AddMember("hello", "world", allocator);
    // fromScratch.AddMember("object", object, allocator);
    // fromScratch["object"]["hello"] = NULL;
    
    array.PushBack(object, allocator);
    
    
    fromScratch.AddMember("hello", "world", allocator);
    fromScratch.AddMember("number", 2, allocator);
    fromScratch.AddMember("array", array, allocator);
    
    
    output(fromScratch);
    
    // ---- parse from string ----
    
    // Convert JSON document to string
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
    fromScratch.Accept(writer);
    
    // parse json string
    rapidjson::Document parsedFromString;
    parsedFromString.Parse<0>(strbuf.GetString());
    
    output(parsedFromString);
    return 0;
}