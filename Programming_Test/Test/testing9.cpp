#include <iostream>
using namespace std;

int main()
{
	int *test;
	test = new int[5];
	for (int i=1;i<=5;i++)
		test[i] = i;
	delete [] test;
	for (int i=1;i<=5;i++)
		cout << test[i];
	return 0;
	
}