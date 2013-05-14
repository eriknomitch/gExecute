all:
	#g++ `pkg-config --cflags --libs gtk+-2.0 libconfig++` gexecute.cpp -o gexecute
	g++ `pkg-config --cflags gtk+-2.0 libconfig++` gexecute.cpp -o gexecute \
	    `pkg-config --libs gtk+-2.0 libconfig++`
