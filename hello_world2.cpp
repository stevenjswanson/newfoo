#include <fstream>

int main(int argc, char *argv[])
{
	std::ofstream ofs ("hello.txt", std::ofstream::out);
	ofs << "Hello cse142L!\n";
	ofs.close();
	return 0;  
}
