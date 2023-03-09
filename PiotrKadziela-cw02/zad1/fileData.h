#pragma once
#include <string>

struct singleFileData {
	int lines;
	int words;
	int chars;
};

std::istream& operator>>(std::istream& is, singleFileData& pr);


class fileData {
private:
	int _size;

	int _maxSize;

	singleFileData** _dataPointers;

public:
	fileData(int desiredSize);

	~fileData();

	int size();

	int maxSize();

	singleFileData& operator[](int index);

	void pushFromFile(std::string path);

	void push(singleFileData& ref);

	void pop();

	void erase(int index);

	void erase(int beginIndex, int endIndex);

};