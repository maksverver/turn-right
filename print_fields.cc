#include <iostream>
#include "base.hh"

int main()
{
	Field f;
	while(std::cin >> f)
		print_field(std::cout, f);
}
