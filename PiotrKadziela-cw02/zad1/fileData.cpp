#include "fileData.h"

#include <algorithm>
#include <exception>
#include <string>
#include <fstream>

using namespace std::string_literals;	//for "foobar"s style std::string literals

const std::string tempFilePath = "//tmp//fileData.tmp";

fileData::fileData(int desiredSize) {
	this->_size = 0;
	this->_maxSize = desiredSize;
	this->_dataPointers = new singleFileData* [desiredSize];
};

fileData::~fileData() {
	while(this->_size) pop();
	delete[] this->_dataPointers;
};

int fileData::size() {
	return this->_size;
};

int fileData::maxSize() {
	return this->_maxSize;
};

singleFileData& fileData::operator[](int index) {
	if(index < 0 || index >= this->_size)
		throw std::out_of_range("fileData::operator[] error: index: "s + std::to_string(index));
	return *(this->_dataPointers[index]);
};

void fileData::pushFromFile(std::string path) {
	//checking if file exists
	std::ifstream inFile;
	inFile.open(path);
	if (!(inFile.good() && inFile.is_open()))
		throw std::runtime_error("fileData::pushFromFile error: couldn't open file \""s + path + "\""s);
	inFile.close();

	int errCode = system(("wc "s + path + " > "s + tempFilePath).c_str());
	if (errCode) {
		throw std::system_error(errCode, std::system_category(), "System error encountered when using wc"s);
	}

	inFile.open(tempFilePath);

	singleFileData* buffer = new singleFileData;
	inFile >> buffer->lines;
	inFile >> buffer->words;
	inFile >> buffer->chars;

	inFile.close();

	push(*buffer);
};

void fileData::push(singleFileData &ref) {
	if (this->_size >= this->_maxSize)
		throw std::out_of_range("fileData::push error: _size: "s + std::to_string(this->_size) + "_maxSize: "s + std::to_string(this->_maxSize));

	this->_dataPointers[this->_size] = &ref;
	this->_size++;
};

void fileData::pop() {
	delete (this->_dataPointers[this->_size - 1]);
	this->_size--;
};

void fileData::erase(int index) {
	erase(index, index + 1);
};

void fileData::erase(int beginIndex, int endIndex) {
	int erasedCounter = endIndex - beginIndex;

	if (beginIndex < 0 || endIndex > this->_size || erasedCounter <= 0)
		throw std::range_error("fileData::erase error: beginIndex: "s + std::to_string(beginIndex) + "endIndex: "s + std::to_string(endIndex));

	for (int i = beginIndex; i < endIndex; i++) {
		delete (this->_dataPointers[i]);
	}

	std::move(&_dataPointers[endIndex], &_dataPointers[this->_size], &_dataPointers[beginIndex]);
	this->_size -= erasedCounter;
};