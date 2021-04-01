compile1:
	g++ -ggdb method4_try1.cpp  -o method4 `pkg-config --cflags --libs opencv` -lpthread

compile2:
	g++ -ggdb method4_try2.cpp  -o method4 `pkg-config --cflags --libs opencv` -lpthread

clean:
	rm -rf subtask2
run:
	./method4 bg.png trafficvideo0.mp4
