#ifndef UNITTEST_PARAMETERIZEDMANAGER_H
#define UNITTEST_PARAMETERIZEDMANAGER_H

#include <vector>
#include <map>
#include "TestDetails.h"
#include "TestList.h"
#include "ParameterizedTest.h"

namespace UnitTest
{
	using namespace std;

	class ParameterizedManager
	{
	public:
		static ParameterizedManager & getInstance();
		Test* const getCurrentTest() const;
		bool isCurrentTest(TestDetails const * const details) const;
		void beginExecute(TestDetails const * const details);
		void endExecute(TestDetails const * const details);
		void updateParameter(ParameterizedTestAbstract* const parameterized);
		const vector<ParameterizedTestAbstract*> & getStack(TestDetails const * const details) const;

	private:
		enum RegisterThen //TODO Can remove this ?
		{
			FIRST,
			ITERATE,
			IDLE
		};

		ParameterizedManager();
		virtual ~ParameterizedManager();
		TestListNode* const retrieveTest(TestDetails const * const details);
		RegisterThen registerParameter(ParameterizedTestAbstract* const parameterized);

		TestListNode* _currentTest;
		TestListNode* _nextTestBackup;
		vector<ParameterizedTestAbstract*> _stack;
		volatile bool _iterationDone;
	};
}

#endif