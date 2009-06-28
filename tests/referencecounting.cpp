
#include "ReferenceCounted.hpp"
using namespace backlot;

#include <iostream>

class TestClass : public ReferenceCounted
{
	public:
		TestClass() : ReferenceCounted()
		{
			std::cout << "Constructor." << std::endl;
		}
		~TestClass()
		{
			std::cout << "Destructor." << std::endl;
		}
};

int main(int argc, char **argv)
{
	// Simple reference counting
	SharedPointer<TestClass> ptr = new TestClass;
	ptr = 0;
	// Weak pointers
	ptr = new TestClass;
	WeakPointer<TestClass> ptr2 = ptr;
	if (ptr2)
		std::cout << "Pointer valid." << std::endl;
	else
		std::cout << "Error: Pointer invalid." << std::endl;
	ptr = 0;
	if (ptr2)
		std::cout << "Error: Pointer valid." << std::endl;
	else
		std::cout << "Pointer invalid." << std::endl;
	return 0;
}