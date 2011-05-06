all:
	g++ `pkg-config --cflags --libs gtk+-2.0 libconfig++` gexecute.cpp -o gexecute
