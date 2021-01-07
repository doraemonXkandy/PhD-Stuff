#include <iostream>
using namespace std;

int main()
{
	int *test;
	test = new int;

	for (int i=2;i<=13;i++)
	{
		*test = i;
	}

	for (int i=1;i<=16;i++)
	{
		cout << *test;
	}
	delete [] test;
	return 0;
}