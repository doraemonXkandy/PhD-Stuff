#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;
void timetest(int& x,  int i);

int main()
{
	int x;
	for (int i=1;i<=10;i++)
	{
		timetest(x,i);
		cout << x;
	}
	return 0;
}

void timetest(int& x, int i)
{

		srand (time(NULL));
	for (int j=1;j<=i;j++)
		x = rand()%100 + 1;
}