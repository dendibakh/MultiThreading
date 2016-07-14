#include "MergeSort.h"
#include <string>
#include <fstream>
#include <vector>
#include <streambuf>
#include <sstream>
#include <iostream>
#include <algorithm>

auto parseNumbers(const std::string& numbersStr)
{
	std::vector<size_t> numbers;
	std::stringstream stream;
	size_t val = 0;
	auto it = numbersStr.begin();
	auto beg = it;
	while (it != numbersStr.end())
	{
		it = std::find(it, numbersStr.end(), ' ');
		if (it != numbersStr.end())
		{
			stream.str("");
			stream.clear();
			stream << std::string(beg, it);
			stream >> val;
			numbers.push_back(val);
			++it;
			beg = it;
		}
	}
	stream.str("");
	stream.clear();
	stream << std::string(beg, it);
	stream >> val;
	numbers.push_back(val);
	++it;
	beg = it;
	return numbers;
}

auto outputNumbers(const std::vector<size_t>& numbers)
{
	std::string numbersStr;
	std::stringstream stream;
	for (auto val : numbers)
	{
		stream.str("");
		stream.clear();
		stream << val;
		numbersStr += stream.str();
		numbersStr += " ";
	}
	return numbersStr;
}

int main()
{
    std::string pathInput = "/home/bakhvalo/dataset_12656_4_unsorted.txt";
    std::string pathOutput = "/home/bakhvalo/dataset_12656_4.txt";
    std::ifstream input(pathInput, std::ifstream::binary);

    std::string inputStr((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    auto numbers = parseNumbers(inputStr);
    auto procNumbers = numbers;

    //multiple input data 10000 times
    //for (int i = 0; i < 9999; ++i)
    //	procNumbers.insert(procNumbers.end(), numbers.begin(), numbers.end());

    single::MergeSort(procNumbers);
    auto outputStr = outputNumbers(procNumbers);

    std::ofstream output(pathOutput, std::ifstream::out | std::ifstream::binary);
    output << outputStr;
}
