CC=g++
CFLAGS=-c -Wall
OPTLVL=-O0

all: compile compileShared

clean: cleanTest cleanCompile cleanCMake




run: compile
	./bin/out

compile: mainObjFile
	$(CC) -Wall $(OPTLVL) ./bin/main.o -o ./bin/out

mainObjFile: prep main.cpp
	$(CC) $(CFLAGS) $(OPTLVL) main.cpp -o ./bin/main.o





test: compile
	@echo
	@echo
	@echo
	./bin/out 0.000000001 2
	@echo
	@echo
	@echo
	./bin/out 0.000000001 8
	@echo
	@echo
	@echo		
	./bin/out 0.000000001 64

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