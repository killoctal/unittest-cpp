#ifndef UNITTEST_PARAMETERIZEDSUITE_H
#define UNITTEST_PARAMETERIZEDSUITE_H

#include <functional>
#include <vector>
#include <memory>
#include <iostream>

#include "Test.h"

namespace UnitTest
{
	using namespace std;

	class ParameterizedSuiteAbstract
	{
	public:

		ParameterizedSuiteAbstract(const string & suiteName)
			: _suiteName(suiteName),
			_testName(suiteName + "ParameterizedSuite_starter")
		{
			// WARNING: this is pointer because of memory problems with suiteName/testName.c_str(),
			// the constructor does not initialize them in the right order
			_testAnchor = make_unique<TestAnchor>(*this);
		}

		size_t getIteration()
		{
			return _iteration;
		}

	protected:

		virtual void peekCurrentValue() = 0;
		virtual size_t valuesSize() = 0;

		void ensureInitialized()
		{
			tryPeekFirstValue();
			tryBranchUniqueTest();
		}

		bool hasMoreValues(int advance = 0)
		{
			return (_iteration + advance < (int)valuesSize());
		}

	private:

		class TestAnchor : public Test
		{
		public:
			TestAnchor(ParameterizedSuiteAbstract & parameterized)
				: Test(parameterized._testName.c_str(), parameterized._suiteName.c_str()),
				_parameterized(parameterized)
			{
				Test::GetTestList().Add(this);
			}
			virtual void RunImpl() const override
			{
				_parameterized.onNewIteration();
			}
		private:
			ParameterizedSuiteAbstract & _parameterized;
		};

		bool tryPeekFirstValue()
		{
			if (_iterationBranched)
			{
				return false;
			}

			peekCurrentValue();
			return false;
		}

		bool tryBranchUniqueTest()
		{
			if (_iterationBranched)
			{
				return false;
			}

			//TODO find a way to allow single parameterized test execution
			static bool messageDisplayed = false;
			if (!messageDisplayed)
			{
				messageDisplayed = true;
				cout << "WARNING: test of parameterized suite " << _suiteName << " can not be executed alone, test will be executed with first value only.";
			}

			return true;
		}

		bool branchTestsForIteration()
		{
			if (_iterationBranched)
			{
				return false;
			}

			for (Test* iTest = _testAnchor.get(); iTest != nullptr; iTest = iTest->m_nextTest)
			{
				bool inSameSuite = (strcmp(iTest->m_details.suiteName, _suiteName.c_str()) == 0);
				bool ownAnchor = (iTest == _testAnchor.get());
				bool isOtherParameterizedSuite = (iTest != _testAnchor.get() && dynamic_cast<TestAnchor*>(iTest) != nullptr);
				if (!inSameSuite || isOtherParameterizedSuite)
				{
					_firstOuterTest = iTest;
					break;
				}
				_lastOfSuite = iTest;
			}

			_lastOfSuite->m_nextTest = _testAnchor.get();

			_iterationBranched = true;
			return true;
		}

		bool unbranchIterationIfLast()
		{
			if (!_iterationBranched)
			{
				return false;
			}

			if (hasMoreValues(1))
			{
				return false;
			}

			_lastOfSuite->m_nextTest = _firstOuterTest;
			return true;
		}

		void onNewIteration()
		{
			if (_iterationBranched)
			{
				_iteration++;
			}

			bool justBranched = branchTestsForIteration();
			bool justUnbranched = unbranchIterationIfLast();

			if (justBranched && justUnbranched)
			{
				// If no values to test, skip all tests
				if (valuesSize() == 0)
				{
					_testAnchor->m_nextTest = _firstOuterTest;
					return;
				}
			}

			peekCurrentValue();
		}

		volatile bool _iterationBranched = false;
		size_t _iteration = 0;
		const string _suiteName;
		const string _testName;
		Test* _firstOuterTest = nullptr;
		Test* _lastOfSuite = nullptr;
		unique_ptr<TestAnchor> _testAnchor;
	};


	/**
	 * @brief Repeat full test suite and gives different values, like a for(:) loop
	 * TODO: iteration should be over tests instead over suite
	 *
	 * @code
	 * SUITE(Testing) {
	 *   vector<int> suitesParams { 1, 5, 10 };
	 *   ParameterizedSuite<int, suitesParams> parameters(UnitTestSuite::GetSuiteName());
	 *   TEST(Show)
	 *   {
	 *     cout << parameters.getCurrent() << endl;
	 *   }
	 * }
	 * @endcode
	 */
	template<class T_Value, const vector<T_Value> & V_Values>
	class ParameterizedSuite : public ParameterizedSuiteAbstract
	{
	public:
		ParameterizedSuite(const string & suiteName)
			: ParameterizedSuite(suiteName, [](T_Value) {})
		{
		}

		ParameterizedSuite(const string & suiteName, function<void(T_Value current)> onNextIterationMethod)
			: ParameterizedSuiteAbstract(suiteName),
			_onNextIterationMethod(onNextIterationMethod)
		{
		}

		T_Value getCurrent()
		{
			ensureInitialized();
			return _currentValue;
		}

		T_Value operator()()
		{
			return getCurrent();
		}

	protected:

		virtual void peekCurrentValue() override
		{
			_currentValue = V_Values[getIteration()];
			_onNextIterationMethod(_currentValue);
		}

		virtual size_t valuesSize() override
		{
			return V_Values.size();
		}

	private:

		function<void(T_Value current)> _onNextIterationMethod;
		T_Value _currentValue;
	};
}

#endif