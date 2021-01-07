#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
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
	path link[8];
};

struct stop
{
	int index;
	stop *next;
};



void inputnetwork(node TSW_stop[], int terminal_list[], int destination_list[], const int num_of_terminal, const int num_of_destination);
double Dijkstra(node TSW_stop[], int origin, int destination);

int main()
{
	const int num_of_terminal = 7, num_of_destination = 5;
	const int Route_max = 10;
	const double Time_max = 35;
	const int Stop_max = 8;

	int terminal_list[num_of_terminal+1];
	int destination_list[num_of_destination+1];
	node TSW_stop[30];
	double shortest_time_matrix[30][30];

	// Initialization
	for (int i=1;i<=29;i++)
	{
		TSW_stop[i].shortest = 9999;
		TSW_stop[i].pred = 0;
		TSW_stop[i].flag = false;
		for (int j=1;j<8;j++)
		{
			TSW_stop[i].link[j].dest = -1;
			TSW_stop[i].link[j].time = 0;
		}
	}

	// Input the network from txt file
	inputnetwork(TSW_stop,terminal_list,destination_list,num_of_terminal,num_of_destination);


	// Calculate shortest_time_matrix
	for (int i=1;i<=29;i++)
		for (int j=1;j<=29;j++)
			shortest_time_matrix[i][j] = Dijkstra(TSW_stop,i,j);


	return 0;
}

void inputnetwork(node TSW_stop[], int terminal_list[], int destination_list[], const int num_of_terminal, const int num_of_destination)
{
	ifstream fin;
	int n, temp, num_of_path;
	char dummy;
	string line;

	// Read txt file
	fin.open("network.txt");

	if (fin.fail())
	{
		cout << "Error opening txt file" << endl;
		exit (1);
	}

	fin >> n;
	getline(fin,line);

	// read network
	for (int i=1;i<=n;i++)
	{
		getline(fin, line);
		fin >> temp;
		fin >> dummy;
		fin >> num_of_path;

		getline(fin, line);
		for (int j=1; j <= num_of_path;j++)
		{
			fin >> TSW_stop[i].link[j].dest;
			fin >> dummy;
			fin >> TSW_stop[i].link[j].time;
			getline(fin, line);
		}
	}

	getline(fin, line);
	getline(fin, line);

	// read terminal list and destination list
	for (int i=1;i<=num_of_terminal;i++)
	{
		fin >> terminal_list[i];
		fin >> dummy;
	}
	getline(fin, line);
	getline(fin, line);
	getline(fin, line);

	for (int i=1;i<=num_of_destination;i++)
	{
		fin >> destination_list[i];
		fin >> dummy;
	}
	getline(fin, line);

	fin.close();

};

double Dijkstra(node TSW_stop[], int origin, int destination)
{
	int Remaining_shortest, next, temp;
	double shortest_time;

	temp = 0;

	for (int i=1; i<=29; i++)
	{
		TSW_stop[i].shortest = 9999;
		TSW_stop[i].pred = 0;
		TSW_stop[i].flag = false;
	}
	TSW_stop[origin].pred = 0;
	TSW_stop[origin].shortest = 0;

	// start finding the solution
	for (int i=1;i<=29;i++)
	{
		shortest_time = 9999;
		for (int j=1;j<=29;j++)
		{
			if (TSW_stop[j].flag == false)
			{
				if (TSW_stop[j].shortest <= shortest_time)
				{
					Remaining_shortest = j;
					shortest_time = TSW_stop[j].shortest;
				}
			}
		} 
		TSW_stop[Remaining_shortest].flag = true;
		for (int j=1;j<=7;j++)
		{
			next = TSW_stop[Remaining_shortest].link[j].dest;
			if (next != -1)
			{
				if (TSW_stop[next].flag == false)
				{
					if (TSW_stop[next].shortest > TSW_stop[Remaining_shortest].shortest + TSW_stop[Remaining_shortest].link[j].time)
					{
						TSW_stop[next].shortest = TSW_stop[Remaining_shortest].shortest + TSW_stop[Remaining_shortest].link[j].time;
						TSW_stop[next].pred = Remaining_shortest;
					}
				}
			}
		}
	}

	return TSW_stop[destination].shortest;
};