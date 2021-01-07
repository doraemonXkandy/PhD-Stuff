#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

void inputnetwork(int*& mylist);

int main()
{

	// Create nodes
	int *mylist;
	inputnetwork(mylist);
	for (int i=1;i<=6;i++)
	{
		cout << mylist[i];
	}
	delete [] mylist;
	return 0;

}

void inputnetwork(int*& mylist)
{
	mylist = new int[6];
	for (int i=1;i<=6;i++)
	{
		mylist[i] = i;
	}
};