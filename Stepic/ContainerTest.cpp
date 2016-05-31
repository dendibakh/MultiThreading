#include <gtest/gtest.h>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>

std::vector<size_t> findXPositions(const std::string& polynomial)
{
	std::vector<size_t> Xpositions;
	auto it = polynomial.begin();
	while (it != polynomial.end())
	{
		it = std::find(it, polynomial.end(), 'x');
		if (it != polynomial.end())
		{
			Xpositions.push_back(it - polynomial.begin());
			++it;
		}
	}
	return Xpositions;
}

std::vector<size_t> findCoeffSignPositions(const std::string& polynomial)
{
	std::vector<size_t> signPositions;
	auto it = polynomial.begin();
	while (it != polynomial.end())
	{
		it = std::find_if(it, polynomial.end(), [] (char c) { return c == '+' || c == '-'; });
		if (it != polynomial.end())
		{
			if (it != polynomial.begin() && *(it - 1) == '^')
			{
				++it;
				continue;
			}
			signPositions.push_back(it - polynomial.begin());
			++it;
		}
	}
	return signPositions;
}

int parseValue(const std::string& str)
{
	if (str.empty())
		return 1;
	std::stringstream stream(str);
	int val = 0;
	stream >> val;
	return val;
}

std::string parseValue(int value)
{
	std::stringstream stream;
	stream << value;
	return stream.str();
}

int parseCoeff (size_t member, const std::vector<size_t>& XPositions, const std::vector<size_t>& signPositions, const std::string& polynomial)
{
	if (XPositions[member] == 0)
		return 1;

	auto beg_pos = signPositions[member] + 1;
	auto end_pos = XPositions[member];
	if (polynomial[end_pos - 1] == '*')
	{
		end_pos = end_pos - 1;
	}
	else
	{
		if (polynomial[signPositions[member]] == '-')
			return -1;
		else
			return 1;
	}
	auto value = parseValue(polynomial.substr(beg_pos, end_pos - beg_pos));

	if (polynomial[signPositions[member]] == '-')
		return -value;
	return value;
}

int parsePowerCoeff (size_t member, const std::vector<size_t>& XPositions, const std::vector<size_t>& signPositions, const std::string& polynomial)
{
	if (XPositions[member] >= polynomial.size() - 1)
		return 1;

	if (polynomial[XPositions[member] + 1] == '^')
	{
		size_t beg_pos = XPositions[member] + 2;
		if (polynomial[beg_pos] == '-' || polynomial[beg_pos] == '+')
			++beg_pos;

		size_t end_pos = std::string::npos;
		if (member < signPositions.size() - 1)
			end_pos = signPositions[member];

		auto value = parseValue(polynomial.substr(beg_pos, end_pos - beg_pos));
		if (polynomial[XPositions[member] + 2] == '-')
			return -value;
		return value;
	}

	return 1;
}

std::string normalize(const std::string& polynomial)
{
	std::vector<size_t> signPositions = findCoeffSignPositions(polynomial);
	if (polynomial[0] != '+' && polynomial[0] != '-')
		signPositions.insert(signPositions.begin(), 0);
	signPositions.insert(signPositions.end(), polynomial.size());

	std::string retStr;
	for (auto it = signPositions.begin(); it + 1 != signPositions.end(); ++it)
	{
		std::string member = polynomial.substr(*it, *(it + 1) - *it);
		retStr.append(member);
		auto iter = std::find(member.begin(), member.end(), 'x');
		if (iter == member.end())
			retStr.append("*x^0");
	}
	return retStr;
}

std::map<int, int> buildCoeffMap(const std::vector<size_t>& XPositions, const std::vector<size_t>& signPositions, const std::string& poly)
{
	std::map<int, int> polynom;

	for (size_t i = 0; i < XPositions.size(); ++i)
	{
		auto power = parsePowerCoeff(i, XPositions, signPositions, poly);
		auto coeff = parseCoeff(i, XPositions, signPositions, poly);
		auto it = polynom.find(power);
		if (it != polynom.end())
			it->second += coeff;
		else
			polynom.insert(std::make_pair(power, coeff));
	}
	return polynom;
}

std::map<int, int> computeDerivative(const std::map<int, int>& polynom)
{
	std::map<int, int> derivativePolynom;

	for (auto i = polynom.begin(); i != polynom.end(); ++i)
	{
		if (i->first > 0)
			derivativePolynom.insert(std::make_pair(i->first - 1, i->first * i->second));
	}
	return derivativePolynom;
}

std::string derivativeToString(const std::map<int, int>& derivativeMap)
{
	std::string retStr;
	for (auto i = derivativeMap.rbegin(); i != derivativeMap.rend(); ++i)
	{
		if (i->second > 0)
			retStr += "+";
		if (i->second != 0)
		retStr += parseValue(i->second);

		if (i->first != 0)
		{
			retStr += "*x";
			if (i->first != 1)
			{
				retStr += "^";
				retStr += parseValue(i->first);
			}
		}
	}

	if (retStr[0] == '+')
		retStr.erase(retStr.begin());
	return retStr;
}

std::string derivative(std::string polynomial)
{
	auto poly = normalize(polynomial);
	std::vector<size_t> XPositions = findXPositions(poly);
	std::vector<size_t> signPositions = findCoeffSignPositions(poly);

	if (signPositions.size() < XPositions.size())
		signPositions.insert(signPositions.begin(), -1);

	std::map<int, int> coeffMap = buildCoeffMap(XPositions, signPositions, poly);
	std::map<int, int> derivativeMap = computeDerivative(coeffMap);

	return derivativeToString(derivativeMap);
}

template <class T>
void checkTwoVectorsAreEqual(const T& lhs, const T& rhs)
{
     EXPECT_EQ(lhs.size(), rhs.size());
     EXPECT_TRUE(std::equal(lhs.begin(), lhs.end(), rhs.begin()));
}

TEST(DerivativeUnitTest, helper_test1)
{
	std::string poly = "x^2+x";
	auto actualX = findXPositions(poly);
	std::vector<size_t> expectedX = {0, 4};
	checkTwoVectorsAreEqual(expectedX, actualX);

	auto actualSign = findCoeffSignPositions(poly);
	std::vector<size_t> expectedSign = {3};
	checkTwoVectorsAreEqual(expectedSign, actualSign);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	EXPECT_EQ(1, parseCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(1, parseCoeff(1, actualX, actualSign, poly));

	EXPECT_EQ(2, parsePowerCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(1, parsePowerCoeff(1, actualX, actualSign, poly));
}

TEST(DerivativeUnitTest, helper_test2)
{
	std::string poly = "2*x^2+3*x";
	auto actualX = findXPositions(poly);
	std::vector<size_t> expectedX = {2, 8};
	checkTwoVectorsAreEqual(expectedX, actualX);

	auto actualSign = findCoeffSignPositions(poly);
	std::vector<size_t> expectedSign = {5};
	checkTwoVectorsAreEqual(expectedSign, actualSign);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	EXPECT_EQ(2, parseCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(3, parseCoeff(1, actualX, actualSign, poly));

	EXPECT_EQ(2, parsePowerCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(1, parsePowerCoeff(1, actualX, actualSign, poly));
}

TEST(DerivativeUnitTest, helper_test3)
{
	std::string poly = "-2*x^2+3*x";
	auto actualX = findXPositions(poly);
	std::vector<size_t> expectedX = {3, 9};
	checkTwoVectorsAreEqual(expectedX, actualX);

	auto actualSign = findCoeffSignPositions(poly);
	std::vector<size_t> expectedSign = {0, 6};
	checkTwoVectorsAreEqual(expectedSign, actualSign);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	EXPECT_EQ(-2, parseCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(3, parseCoeff(1, actualX, actualSign, poly));

	EXPECT_EQ(2, parsePowerCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(1, parsePowerCoeff(1, actualX, actualSign, poly));
}

TEST(DerivativeUnitTest, helper_test4)
{
	std::string poly = "+2*x^2-3*x";
	auto actualX = findXPositions(poly);
	std::vector<size_t> expectedX = {3, 9};
	checkTwoVectorsAreEqual(expectedX, actualX);

	auto actualSign = findCoeffSignPositions(poly);
	std::vector<size_t> expectedSign = {0, 6};
	checkTwoVectorsAreEqual(expectedSign, actualSign);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	EXPECT_EQ(2, parseCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(-3, parseCoeff(1, actualX, actualSign, poly));

	EXPECT_EQ(2, parsePowerCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(1, parsePowerCoeff(1, actualX, actualSign, poly));
}

TEST(DerivativeUnitTest, helper_test5)
{
	std::string poly = "+2*x^-2-3*x^7";
	auto actualX = findXPositions(poly);
	std::vector<size_t> expectedX = {3, 10};
	checkTwoVectorsAreEqual(expectedX, actualX);

	auto actualSign = findCoeffSignPositions(poly);
	std::vector<size_t> expectedSign = {0, 7};
	checkTwoVectorsAreEqual(expectedSign, actualSign);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	EXPECT_EQ(2, parseCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(-3, parseCoeff(1, actualX, actualSign, poly));

	EXPECT_EQ(-2, parsePowerCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(7, parsePowerCoeff(1, actualX, actualSign, poly));
}

TEST(DerivativeUnitTest, normalization_test)
{
	EXPECT_STREQ("2*x+1*x^0", normalize("2*x+1").c_str());
	EXPECT_STREQ("x^10000+x+1*x^0", normalize("x^10000+x+1").c_str());
}

TEST(DerivativeUnitTest, derivativeToString)
{
	std::map<int, int> derivativeMap = { {0, 2} };
	EXPECT_STREQ("2", derivativeToString(derivativeMap).c_str());
}

TEST(DerivativeUnitTest, test1)
{
	std::string poly = normalize("2*x+1");
	auto actualX = findXPositions(poly);
	auto actualSign = findCoeffSignPositions(poly);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	EXPECT_EQ(2, parseCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(1, parseCoeff(1, actualX, actualSign, poly));

	EXPECT_EQ(1, parsePowerCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(0, parsePowerCoeff(1, actualX, actualSign, poly));

	auto actualCoeffMap = buildCoeffMap(actualX, actualSign, poly);
	std::map<int, int> expectedCoeffMap = { {0, 1}, {1, 2} };
	checkTwoVectorsAreEqual(expectedCoeffMap, actualCoeffMap);

	auto actualDerivativeMap = computeDerivative(actualCoeffMap);
	std::map<int, int> expectedDerivativeMap = { {0, 2} };
	checkTwoVectorsAreEqual(expectedDerivativeMap, actualDerivativeMap);

	ASSERT_STREQ("2*x+1", derivative("x^2+x").c_str());
}

TEST(DerivativeUnitTest, test2)
{
	std::string poly = normalize("2*x^100+100*x^2");
	auto actualX = findXPositions(poly);
	auto actualSign = findCoeffSignPositions(poly);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	EXPECT_EQ(2, parseCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(100, parseCoeff(1, actualX, actualSign, poly));

	EXPECT_EQ(100, parsePowerCoeff(0, actualX, actualSign, poly));
	EXPECT_EQ(2, parsePowerCoeff(1, actualX, actualSign, poly));

	auto actualCoeffMap = buildCoeffMap(actualX, actualSign, poly);
	std::map<int, int> expectedCoeffMap = { {2, 100}, {100, 2} };
	checkTwoVectorsAreEqual(expectedCoeffMap, actualCoeffMap);

	auto actualDerivativeMap = computeDerivative(actualCoeffMap);
	std::map<int, int> expectedDerivativeMap = { {1, 200}, {99, 200} };
	checkTwoVectorsAreEqual(expectedDerivativeMap, actualDerivativeMap);

	ASSERT_STREQ("200*x^99+200*x", derivative("2*x^100+100*x^2").c_str());
}

TEST(DerivativeUnitTest, test3)
{
	std::string poly = normalize("x^10000+x+1");
	auto actualX = findXPositions(poly);
	auto actualSign = findCoeffSignPositions(poly);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	auto actualCoeffMap = buildCoeffMap(actualX, actualSign, poly);
	std::map<int, int> expectedCoeffMap = { {0, 1}, {1, 1}, {10000, 1} };
	checkTwoVectorsAreEqual(expectedCoeffMap, actualCoeffMap);

	auto actualDerivativeMap = computeDerivative(actualCoeffMap);
	std::map<int, int> expectedDerivativeMap = { {0, 1}, {9999, 10000} };
	checkTwoVectorsAreEqual(expectedDerivativeMap, actualDerivativeMap);

	ASSERT_STREQ("10000*x^9999+1", derivative("x^10000+x+1").c_str());
}

TEST(DerivativeUnitTest, test4)
{
	std::string poly = normalize("-x^2-x^3");
	auto actualX = findXPositions(poly);
	auto actualSign = findCoeffSignPositions(poly);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	auto actualCoeffMap = buildCoeffMap(actualX, actualSign, poly);
	std::map<int, int> expectedCoeffMap = { {2, -1}, {3, -1} };
	checkTwoVectorsAreEqual(expectedCoeffMap, actualCoeffMap);

	auto actualDerivativeMap = computeDerivative(actualCoeffMap);
	std::map<int, int> expectedDerivativeMap = { {1, -2}, {2, -3} };
	checkTwoVectorsAreEqual(expectedDerivativeMap, actualDerivativeMap);

	ASSERT_STREQ("-3*x^2-2*x", derivative("-x^2-x^3").c_str());
}

TEST(DerivativeUnitTest, test5)
{
	std::string poly = normalize("x+x+x+x+x+x+x+x+x+x");
	auto actualX = findXPositions(poly);
	auto actualSign = findCoeffSignPositions(poly);

	if (actualSign.size() < actualX.size())
		actualSign.insert(actualSign.begin(), -1);

	auto actualCoeffMap = buildCoeffMap(actualX, actualSign, poly);
	std::map<int, int> expectedCoeffMap = { {1, 10} };
	checkTwoVectorsAreEqual(expectedCoeffMap, actualCoeffMap);

	auto actualDerivativeMap = computeDerivative(actualCoeffMap);
	std::map<int, int> expectedDerivativeMap = { {0, 10} };
	checkTwoVectorsAreEqual(expectedDerivativeMap, actualDerivativeMap);

	ASSERT_STREQ("10", derivative("x+x+x+x+x+x+x+x+x+x").c_str());
}
