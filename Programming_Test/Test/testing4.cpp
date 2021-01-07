#include <iostream>
using namespace std;

int main()
{
	int *p1, *p2;

	p1 = new int;
	
	*p1 = 6;
	p1 [2] = 4;

	delete p1;
	

	cout << *p1;
	return 0;
}