#include <iostream>
#include "pin_tags.h"
#include "archlab.hpp"

int main(int argc, char *argv[])
{	
	if (argc > 1) {
		std::cout << "Goodbye summer!\n";
	} else {
		std::cout << "Hello cse142L!\n";
	}
	return 0;  
}
