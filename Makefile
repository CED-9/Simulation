all:
	g++ -g -c CN_Node.cpp CN_ConNode.cpp CN_ProNode.cpp CN_FinNode.cpp CN_LabNode.cpp -std=c++11
	g++ -g -c CN_Graph.cpp CN_BanNode.cpp CN_CreditNet.cpp -std=c++11
	g++ -o test test_creditnet.cpp *.o
clean:
	rm *.o test
test:
	g++ -g -c CN_Node.cpp CN_ConNode.cpp CN_ProNode.cpp CN_FinNode.cpp CN_LabNode.cpp -std=c++11
	g++ -g -c CN_Graph.cpp CN_BanNode.cpp CN_CreditNet.cpp -std=c++11
	g++ -o test test_creditnet.cpp *.o
	./test