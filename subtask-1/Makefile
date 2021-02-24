
do:
	@read -p "Enter file name: " file; \
	echo "Your file is: " $$file; \

empty.jpg: runfile
	@read -p "Enter file name: " file; \
	echo "Your file is: " $(file); \
	[ -f ./$(file)] && ./sub_task1 $(file) || echo "File doesn't exist. Please make sure you have {file} saved in the same folder."
traffic.jpg: runfile
	@[ -f ./imageName ] && ./sub_task1 traffic.jpg || echo "File doesn't exist. Please make sure you have traffic.jpg saved in the same folder."
runfile:
	g++ -ggdb sub_task1.cpp -o sub_task1 `pkg-config --cflags --libs opencv`


trafficcheck:
	@[ -f ./traffic.jpg ] && true || false
	exit 0
