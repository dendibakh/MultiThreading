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
			stream.clear();
			stream << std::string(beg, it);
			stream >> val;
			numbers.push_back(val);
			++it;
			beg = it;
		}
	}
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
		stream.clear();
		stream << val;
		numbersStr += stream.str();
		numbersStr += " ";
	}
	return numbersStr;
}

int main()
{
    std::string pathInput = "/home/denis/dataset_12656_4_unsorted.txt";
    std::string pathOutput = "/home/denis/dataset_12656_4.txt";
    std::ifstream input(pathInput, std::ifstream::binary);

    std::string inputStr((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

    auto numbers = parseNumbers(inputStr);
    //std::cout << numbers.size() << "\n";
    MergeSort(numbers);
    //std::cout << numbers.size() << "\n";
    //for (auto val : numbers)
    //	std::cout << val << " ";

    auto outputStr = outputNumbers(numbers);

    auto numbers2 = parseNumbers(outputStr);
    std::cout << numbers2.size() << "\n";

    std::ofstream output(pathOutput, std::ifstream::out | std::ifstream::binary);
    output << outputStr;
}


