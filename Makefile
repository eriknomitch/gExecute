all:
	g++ `pkg-config --cflags --libs gtk+-2.0 libconfig++` main.cpp -o gexecute
