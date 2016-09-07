#ifndef UNITTEST_PARAMETERIZEDMACROS_H
#define UNITTEST_PARAMETERIZEDMACROS_H

#include "TestParameter.h"


#define UNITTEST_SET_TEST_PARAMETER_LISTENER(Type, Name, ListenerPtr) \
	class ParameterizedCreator##Name \
	{ \
	public: \
		ParameterizedCreator##Name() : globalInstance(getGlobalInstance()) {} \
		UnitTest::TestParameter<Type> & globalInstance; \
	private: \
		static UnitTest::TestParameter<Type> & getGlobalInstance() \
		{ \
			static UnitTest::TestParameter<Type> instance(#Name, create(), ListenerPtr); \
			return instance; \
		} \
		static vector<Type> create() { vector<Type> values; createInternal(values); return values; } \
		inline static void createInternal(vector<Type> & values); \
	} static parameterizedCreator##Name##Instance; \
	\
	static UnitTest::TestParameter<Type> & Name(parameterizedCreator##Name##Instance.globalInstance); \
	void ParameterizedCreator##Name::createInternal(vector<Type> & values)


#define UNITTEST_SET_TEST_PARAMETER(Type, Name) \
	UNITTEST_SET_TEST_PARAMETER_LISTENER(Type, Name, nullptr)


#ifndef UNITTEST_DISABLE_SHORT_MACROS
	#ifdef SET_TEST_PARAMETER_LISTENER
		#error SET_TEST_PARAMETER_LISTENER already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_SET_TEST_PARAMETER_LISTENER instead
	#else
		#define SET_TEST_PARAMETER_LISTENER UNITTEST_SET_TEST_PARAMETER_LISTENER
	#endif

	#ifdef SET_TEST_PARAMETER
		#error SET_TEST_PARAMETER already defined, re-configure with UNITTEST_ENABLE_SHORT_MACROS set to 0 and use UNITTEST_SET_TEST_PARAMETER instead
	#else
		#define SET_TEST_PARAMETER UNITTEST_SET_TEST_PARAMETER
	#endif
#endif

#endif