CXXFLAGS = 

main.exe: main.o negative_queue_size_exception.o empty_queue_exception.o
	g++ main.o negative_queue_size_exception.o empty_queue_exception.o -o main.exe -std=c++0x

main.o: main.cpp
	g++ -c main.cpp -o main.o -std=c++0x

negative_queue_size_exception.o: negative_queue_size_exception.cpp
	g++ -c negative_queue_size_exception.cpp -o negative_queue_size_exception.o

empty_queue_exception.o: empty_queue_exception.cpp
	g++ -c empty_queue_exception.cpp -o empty_queue_exception.o

.PHONY:
clean:
	rm *.exe *.o
