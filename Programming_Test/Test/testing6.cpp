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
	list = new stop[2];

	list[1].index = 11;
	list[1].time_to_next = 30;
	list[1].next = NULL;
	list[2].index = 22;
	list[2].time_to_next = 60;
	list[2].next = NULL;

	for (int i=1;i<=2;i++)
	{
		cout << list[i].index << list[i].time_to_next << list[i].next;
		cout << endl;
	}
	delete [] list;
	return 0;

}
