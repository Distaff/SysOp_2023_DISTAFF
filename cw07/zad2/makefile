CC=g++
CFLAGS=-c -Wall -std=c++17
OPTLVL=-O0 -pthread

all: compile

clean: cleanTest cleanCompile cleanCMake




run: compile
	./bin/out

compile: mainObjFile
	$(CC) -Wall $(OPTLVL) ./bin/main.o -o ./bin/out -lrt

mainObjFile: prep main.cpp
	$(CC) $(CFLAGS) $(OPTLVL) main.cpp -o ./bin/main.o





test: compile
	@echo
	@echo
	@echo
	./bin/out
	@echo
	@echo
	@echo

prep:
	mkdir -p bin/

cleanTest:
	rm -f -r ./test/result/
	
cleanCompile:
	rm -f -r ./bin/

cleanCMake:
	rm -f -r ./out/
	
cleanVS:
	rm -f -r ./.vs/