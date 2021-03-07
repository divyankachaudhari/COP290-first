run:
	g++ -ggdb subtask2.cpp warpAndCrop.cpp bsfilter.cpp densities.cpp -o subtask2 `pkg-config --cflags --libs opencv`
	./subtask2
	python3 plot.py
clean:
	rm -rf subtask2
