all:
	g++ -o s server.cpp
	gcc -o t trainCommand.c
	gcc $( pkg-config --cflags gtk4 ) -o d StationDisplay.c $( pkg-config --libs gtk4 )
	gcc $( pkg-config --cflags gtk4 ) -o c Client.c  $( pkg-config --libs gtk4 )
