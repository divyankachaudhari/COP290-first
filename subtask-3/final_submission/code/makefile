compile1:
	g++ -ggdb method1.cpp functions.cpp -o method1 `pkg-config --cflags --libs opencv`
clean1:
	rm -rf method1

compile2:
	g++ -ggdb method2.cpp functions.cpp -o method2 `pkg-config --cflags --libs opencv`
clean2:
	rm -rf method2

compile3:
	g++ -ggdb method3.cpp functions.cpp -o method3 `pkg-config --cflags --libs opencv` -lpthread
clean3:
	rm -rf method3

compile4:
	g++ -ggdb method4.cpp functions.cpp  -o method4 `pkg-config --cflags --libs opencv` -lpthread
clean4:
	rm -rf method4
