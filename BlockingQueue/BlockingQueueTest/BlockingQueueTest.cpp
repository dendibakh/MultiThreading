#include "stdafx.h"
#include "BlockingQueue.h"

using namespace System;
using namespace System::Text;
using namespace System::Collections::Generic;
using namespace	Microsoft::VisualStudio::TestTools::UnitTesting;

namespace BlockingQueueTest
{
	[TestClass]
	public ref class BlockingQueueTest
	{
	public: 
		[TestMethod]
		void OneThread()
		{
			BlockingQueue bq;
			Assert::IsTrue(bq.isEmpty());
			Assert::IsFalse(bq.isFull());
			bq.push(1);
			Assert::IsFalse(bq.isEmpty());
			Assert::IsFalse(bq.isFull());
			bq.pop();
			Assert::IsTrue(bq.isEmpty());
			Assert::IsFalse(bq.isFull());
			for (int i = 0; i < 10; i++)
			{
				bq.push(i);
			}
			Assert::IsFalse(bq.isEmpty());
			Assert::IsTrue(bq.isFull());
			bq.pop();
			Assert::IsFalse(bq.isEmpty());
			Assert::IsFalse(bq.isFull());
			bq.push(9);
			Assert::IsFalse(bq.isEmpty());
			Assert::IsTrue(bq.isFull());
			for (int i = 0; i < 10; i++)
			{
				bq.pop();
			}
			Assert::IsTrue(bq.isEmpty());
			Assert::IsFalse(bq.isFull());
		}
	};
}
