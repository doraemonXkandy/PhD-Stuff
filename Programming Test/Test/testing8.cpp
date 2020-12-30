#include <iostream>
#include <cstdlib>
#include <ctime>
using namespace std;

int main()
{
	srand (time(NULL));
	int x;
	x = rand() %7;
	cout << x;
	x = rand() %5;
	cout << x;
	x = rand() %7;
	cout << x;
	x = rand() %5;
	cout << x;
		x = rand() %7;
	cout << x; 
		x = rand() %5;
	cout << x; 
		x = rand() %7;
	cout << x;  
		x = rand() %5;
	cout << x; 
		x = rand() %7;
	cout << x; 
	return 0;
}