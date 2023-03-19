CC=g++
CFLAGS=-c -Wall
OPTLVL=-O0

all: compileStatic compileShared

clean: cleanShared cleanStatic cleanTest
	rm -f -r ./bin/
	
mainObjFile: prepShared main.cpp
	$(CC) $(CFLAGS) $(OPTLVL) main.cpp -o ./bin/main.o

runStatic: compileStatic
	./bin/outStatic

compileStatic: libStatic mainObjFile
	$(CC) -Wall $(OPTLVL) ./bin/main.o -L./bin/static -lfileData -o ./bin/outStatic

libStatic: libObjFileStatic prepStatic
	ar rcs ./bin/static/libfileData.a ./bin/fileData.o
	
libObjFileStatic: fileData.cpp
	mkdir -p bin
	$(CC) $(CFLAGS) $(OPTLVL) fileData.cpp -o ./bin/fileData.o
	
prepStatic:
	mkdir -p bin/static
	
cleanStatic:
	rm -f -r ./bin/static ./bin/fileData.o *.a
	
	
	
	
runShared: compileShared
	./bin/outShared

compileShared: libShared mainObjFile
	$(CC) -Wall $(OPTLVL) ./bin/main.o -L./bin/shared -lfileData -o ./bin/outShared

libShared: libObjFileShared prepShared
	$(CC) $(OPTLVL) -shared ./bin/fileData.o -o ./bin/shared/libfileData.so
	sudo cp ./bin/shared/libfileData.so /usr/lib/
	
libObjFileShared: fileData.cpp
	mkdir -p bin
	$(CC) $(CFLAGS) $(OPTLVL) -fPIC fileData.cpp -o ./bin/fileData.o
	
prepShared:
	mkdir -p bin/shared
	
cleanShared:
	rm -f -r ./bin/shared ./bin/fileData.o *.a
	
	
	
testStatic: compileStatic prepTest
	./bin/outStatic < ./test/expectedTest.in > ./test/results/results_static_$(OPTLVL).txt
	
testShared: compileShared prepTest
	./bin/outShared < ./test/expectedTest.in > ./test/results/results_shared_$(OPTLVL).txt
	
prepTest:
	mkdir -p test/results/
	
test: testStatic testShared

cleanTest:
	rm -f -r ./test/results/