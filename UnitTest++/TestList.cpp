#include "TestList.h"
#include "Test.h"
#include "TestDetails.h"

#include <cassert>

namespace UnitTest {

   TestList::TestList()
      : m_head(0)
      , m_tail(0)
   {}

   void TestList::Add(Test* test)
   {
      if (m_tail == 0)
      {
         assert(m_head == 0);
         m_head = test;
         m_tail = test;
      }
      else
      {
         m_tail->m_nextTest = test;
         m_tail = test;
      }
   }

   Test* TestList::GetHead() const
   {
      return m_head;
   }

   Test* const TestList::find(TestDetails const * const details)
   {
      for (Test* iTest = GetHead(); iTest != nullptr; iTest = iTest->m_nextTest)
      {
         if (iTest->m_details.sameTest(*details))
         {
            return iTest;
         }
      }
      return nullptr;
   }

   ListAdder::ListAdder(TestList& list, Test* test)
   {
      list.Add(test);
   }

}
