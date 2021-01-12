#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main()
{
	int randx;
	srand(time(NULL));
	randx = rand();
	cout << randx;
	return 0;
}