#include <gtest/gtest.h>
#include <iostream>

namespace
{
	class StringPointer
	{
	public:
		std::string *operator->()
		{
			if (!ptr)
			{
				was_allocated = true;
				ptr = new std::string("");
			}
			return ptr;
		}
		operator std::string*()
		{
			if (!ptr)
			{
				was_allocated = true;
				ptr = new std::string("");
			}
			return ptr;
		}
		StringPointer(std::string *Pointer) : ptr(Pointer) {}
		~StringPointer()
		{
			if (was_allocated)
				delete ptr;
		}
	private:
		bool was_allocated{false};
		std::string* ptr;
	};
}

TEST(SmartPointerUnitTest, test1)
{
	std::string s1 = "Hello, world!";

	StringPointer sp1(&s1);
	StringPointer sp2(NULL);

	std::cout << sp1->length() << std::endl;
	std::cout << *sp1 << std::endl;
	std::cout << sp2->length() << std::endl;
	std::cout << *sp2 << std::endl;
}
