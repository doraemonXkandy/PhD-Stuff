#include <iostream>
using namespace std;

int main()
{
	int n = 6;
	int* num;
	num = new int[6];
	for (int i = 1; i <= n; i++)
		num[i] = i;
	for (int i = 1; i <= n; i++)
		cout << num[i];
	delete[] num;
	cin >> n;
	return 0;
}