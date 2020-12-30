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
	int index;
	int shortest;
	int pred;
	path link[4];
};

int numofnode();
void inputnetwork(node* list);

int main()
{
	int n;
	n = numofnode();

	node *mylist;
	mylist = new node[n];

	// initialization
	for (int i=0;i<n;i++)
	{
		mylist[i].index = i+1;
		mylist[i].shortest = 9999;
		mylist[i].pred = 0;
		for (int j=0;j<4;j++)
		{
			mylist[i].link[j].dest = -1;
			mylist[i].link[j].time = 0;
		}
	}

	inputnetwork(mylist);

	delete [] mylist;

	for (int i=0;i<n;i++)
	{
		cout << mylist[i].index << endl;
		cout << mylist[i].shortest << endl;
		cout << mylist[i].pred << endl;
		for (int j=0;j<4;j++)
		{
			cout << mylist[i].link[j].dest << endl;
			cout << mylist[i].link[j].time << endl;
		}
		cout << endl;
	}

	cout << endl;
	return 0;
}

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

	for (int i=0;i<n;i++)
	{
		getline(fin,line);
		fin >> temp;
		fin >> dummy;
		fin >> num_of_path;
		cout << endl << "***" << num_of_path << "***" << endl; 
		getline(fin,line);
		for (int j=0;j < num_of_path;j++)
		{
			fin >> temp;
			list[i].link[j].dest = temp-1;
			fin >> dummy;
			fin >> list[i].link[j].time;
			getline(fin,line);
		}
	}
	fin.close();
};