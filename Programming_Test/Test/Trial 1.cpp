#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

struct path
{
	int dest;
	double time;
};

struct node
{
	double shortest;
	int pred;
	bool flag;
	path link[7];
};

int Find_Remaining_Shortest(node* list, int n);
void Dijkstra(node* list, int n);
int numofnode();
void inputnetwork(node* list, int* terminal_list, int* destination_list, const int a, const int b);

int main()
{
	const int num_of_terminal = 7, num_of_destination = 5;

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
		mylist[i].flag = false;
		for (int j=0;j<7;j++)
		{
			mylist[i].link[j].dest = -1;
			mylist[i].link[j].time = 0;
		}
	}

	// Create destination list and terminal list
	int *terminal_list;
	int *destination_list;

	terminal_list = new int[num_of_terminal];
	destination_list = new int[num_of_destination];
	// Input the network from the text file
	inputnetwork(mylist,terminal_list,destination_list,num_of_terminal,num_of_destination);


	// For testing
	//for (int i=1;i<= num_of_terminal;i++)
	//{
	//	cout << terminal_list[i];
	//	cout << ",";
	//}

//	cout << endl;
//	for (int i=1;i<= num_of_destination;i++)
//	{
//		cout << destination_list[i];
//		cout << ",";
//	}

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
	delete [] terminal_list;
	delete [] destination_list;

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
		for (int j=0;j<7;j++)
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

void inputnetwork(node* list, int* terminal_list, int* destination_list, const int a, const int b)
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

	cout << "Check1";
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

	getline(fin,line);
	getline(fin,line);

	for (int i=1;i<=a;i++)
	{
		fin >> terminal_list[i];
		fin >> dummy;
	}
	getline(fin,line);

	getline(fin,line);
	getline(fin,line);

	for (int i=1;i<=b;i++)
	{
		fin >> destination_list[i];
		fin >> dummy;
	}
	getline(fin,line);

	fin.close();
};