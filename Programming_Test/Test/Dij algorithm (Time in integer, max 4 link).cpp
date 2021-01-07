#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

struct path
{
	int dest;
	int time;
};

struct node
{
	int shortest;
	int pred;
	bool flag;
	path link[4];
};

int Find_Remaining_Shortest(node* list, int n);
void Dijkstra(node* list, int n);
int numofnode();
void inputnetwork(node* list);

int main()
{
	int n;

	// Obtaining the number of node from the text file
	n = numofnode();

	// Create nodes
	node *mylist;
	mylist = new node[n];

	// initialization
	for (int i=1;i<=n;i++)
	{
		mylist[i].shortest = 9999;
		mylist[i].pred = 0;
		for (int j=0;j<4;j++)
		{
			mylist[i].link[j].dest = -1;
			mylist[i].link[j].time = 0;
			mylist[i].flag = false;
		}
	}

	// Input the network from the text file
	inputnetwork(mylist);

	// For testing
	//for (int i=1;i<=n;i++)
	//{
	//	cout << mylist[i].shortest << endl;
	//	cout << mylist[i].pred << endl;
	//	for (int j=0;j<4;j++)
	//	{
	//		cout << mylist[i].link[j].dest << endl;
	//		cout << mylist[i].link[j].time << endl;
	//	}
	//	cout << endl;
	//}

	// Finding shortest path by Dijkstra's algorithm
	Dijkstra(mylist,n);

	// Return the memory to OS
	delete [] mylist;

	return 0;
}

void Dijkstra(node* list, int n)
{
	int origin, destination,key,next,temp;
	temp = 0;

	cout << "Please input the origin node: " << endl;
	cin >> origin;
	cout << "Please input the destination node: " << endl;
	cin >> destination; 

	list[origin].pred = 0;
	list[origin].shortest = 0;

	// start finding the solution
	for (int i=0; i<n;i++)
	{
		key = Find_Remaining_Shortest(list,n);
		list[key].flag = true;
		for (int j=0;j<4;j++)
		{
			next = list[key].link[j].dest;
			if (next != -1)
			{
				if (list[next].flag == false)
				{
					if (list[next].shortest > list[key].shortest + list[key].link[j].time)
					{
						list[next].shortest = list[key].shortest + list[key].link[j].time;
						list[next].pred = key;
					}
				}
			}
		}
	}

	cout << "Shortest time: " << list[destination].shortest << endl;
	cout << "The path: " << endl;
	cout << destination << " <- ";
	temp = list[destination].pred;
	
	while (temp != origin)
	{
		cout << temp << " <- ";
		temp = list[temp].pred;
	}
	cout << origin << endl;
};

int Find_Remaining_Shortest(node* list, int n)
{
	int shortest_time = 9999;
	int corresponding;

	for (int i=1;i<=n;i++)
	{
		if (list[i].flag == false)
		{
			if (list[i].shortest <= shortest_time)
			{
				corresponding = i;
				shortest_time = list[i].shortest;
			}
		}
	}

	return corresponding;
};

int numofnode()
{
	ifstream fin;
	fin.open("network.txt");

	if (fin.fail())
	{
		cout << "Error opening file" << endl;
		exit (1);
	}

	int n;
	fin >> n;
	fin.close();
	return n;
};

void inputnetwork(node* list)
{
	ifstream fin;
	int n, temp, num_of_path;
	char dummy;
	string line;

	fin.open("network.txt");

	if (fin.fail())
	{
		cout << "Error opening file" << endl;
		exit (1);
	}

	fin >> n;
	getline(fin,line);

	for (int i=1;i<=n;i++)
	{
		getline(fin,line);
		fin >> temp;
		fin >> dummy;
		fin >> num_of_path;
 
		getline(fin,line);
		for (int j=0;j < num_of_path;j++)
		{
			fin >> temp;
			list[i].link[j].dest = temp;
			fin >> dummy;
			fin >> list[i].link[j].time;
			getline(fin,line);
		}
	}
	fin.close();
};