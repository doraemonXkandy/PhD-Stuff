#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

struct stop
{
	int index;
	int time_to_next;
	stop *next;
};

int main()
{

	// Create nodes
	stop *list;
	list = new stop;

	list.index = 11;
	list.time_to_next = 30;
	list.next = NULL;


	for (int i=1;i<=1;i++)
	{
		cout << list[i].index << list[i].time_to_next << list[i].next;
		cout << endl;
	}
	delete [] list;
	return 0;

}
