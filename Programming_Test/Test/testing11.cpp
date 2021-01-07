#include <iostream>
using namespace std;

int main()
{
	int demand[23][5];

	for (int i=0;i<=22;i++)
	{
		for (int j=0;j<=4;j++)
		{
			demand[i][j] = i*j;
			cout << demand[i][j] << endl;
		}
	}
	return 0;
}