all:
	g++ -g -c Error.cpp -std=c++11
	g++ -g -c CN_DistributionGenerator.cpp -std=c++11
	g++ -g -c CN_Node.cpp CN_BanNode.cpp CN_ConNode.cpp CN_ProNode.cpp CN_FinNode.cpp CN_LabNode.cpp -std=c++11
	g++ -g -c CN_Graph.cpp -std=c++11
	g++ -g -c CN_CreditNet.cpp -std=c++11
	g++ -g -c CN_WidgetGraph.cpp -std=c++11
	g++ -g -c CN_GreedyGraph.cpp -std=c++11
	g++ -o test testWidget.cpp *.o -std=c++11
clean:
	rm *.o test
test:
	g++ -g -c Error.cpp -std=c++11
	g++ -g -c CN_DistributionGenerator.cpp -std=c++11
	g++ -g -c CN_Node.cpp CN_BanNode.cpp CN_ConNode.cpp CN_ProNode.cpp CN_FinNode.cpp CN_LabNode.cpp -std=c++11
	g++ -g -c CN_Graph.cpp -std=c++11
	g++ -g -c CN_CreditNet.cpp -std=c++11
	g++ -g -c test_creditnet.cpp -std=c++11
	g++ -o test *.o
	time ./test > out
	gnuplot simulation
test0:
	g++ -g -c Error.cpp -std=c++11
	g++ -g -c CN_DistributionGenerator.cpp -std=c++11
	g++ -g -c CN_Node.cpp CN_BanNode.cpp CN_ConNode.cpp CN_ProNode.cpp CN_FinNode.cpp CN_LabNode.cpp -std=c++11
	g++ -g -c CN_Graph.cpp -std=c++11
	g++ -g -c CN_CreditNet.cpp -std=c++11
	g++ -g -c CN_WidgetGraph.cpp -std=c++11
	g++ -o test test.cpp *.o
	time ./test > out
	gnuplot simulation0
