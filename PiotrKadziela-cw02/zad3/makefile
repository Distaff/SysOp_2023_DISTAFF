CC=g++
CFLAGS=-c -Wall -std=c++17
OPTLVL=-O0

all: compile compileShared

clean: cleanTest cleanCompile cleanCMake




run: compile
	@echo
	@echo
	@echo
	@./bin/out

compile: mainObjFile
	$(CC) -Wall $(OPTLVL) ./bin/main.o -o ./bin/out

mainObjFile: prep main.cpp
	$(CC) $(CFLAGS) $(OPTLVL) main.cpp -o ./bin/main.o





test: run

prep:
	mkdir -p bin/

prepTest:
	mkdir -p ./test/result/

cleanTest:
	rm -f -r ./test/result/
	
cleanCompile:
	rm -f -r ./bin/

cleanCMake:
	rm -f -r ./out/
	
cleanVS:
	rm -f -r ./.vs/