#include <iostream>
#include <fstream>
using namespace std;
int main()
{
	std::ifstream file("file%20.txt");
	// std::ifstream file("file%20.txt");
	if (file.is_open())
	{
		cout << "hhhhhhhhhhhhhhhhhhhhhhhh\n";
		file.close();
		return 1;
	}
	else
		cout << "no" << endl;
	return 0;
}
