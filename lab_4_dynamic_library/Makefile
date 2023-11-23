start_dynamic:
	g++ -fPIC -c lib1.cpp
	g++ -fPIC -c lib2.cpp
	g++ -shared -o liblib1.so lib1.o
	g++ -shared -o liblib2.so lib2.o
	g++ test_dynamic.cpp -c -o  test_dynamic.o
	g++ test_dynamic.o -L. -llib1 -llib2 -o test_dynamic.out -Wl,-rpath -Wl,.
	./test_dynamic.out liblib1.so liblib2.so
start_linking:
	g++ -fPIC -c lib1.cpp
	g++ -shared -o liblib1.so lib1.o
	g++ test_linking.cpp -c -o  test_linking.o
	g++ test_linking.o -L. -llib1 -o test_linking.out -Wl,-rpath -Wl,.
	./test_linking.out 
clean:
	rm -rf *.o *.so  *.out
