#include <iostream>
using namespace std;

int main()
{
	int *num;
	num = new int[6];
	int num2;
	for (int i=0;i<=5;i++)
		num[i] = i;
	
	num2 = num;
	cout << num2 << endl;
	delete [] num;

	num2 = num;
	cout << num2;
	return 0;
}