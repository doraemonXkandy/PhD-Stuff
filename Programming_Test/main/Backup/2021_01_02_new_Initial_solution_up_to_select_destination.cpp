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
void initialsolution(stop* route[],node* TSW_stop,const int num_of_terminal,const int num_of_destination,int terminal_list[], int destination_list[], const int Route_max, const int Time_max, const int Stop_max, double shortest_time_matrix[][30]);
void add2end(stop*& ptr, int x);
void displayallroute(stop* route[],const int Route_max);

int main()
{
	const int num_of_terminal = 7, num_of_destination = 5;
	const int Route_max = 10;
	const double Time_max = 35;
	const int Stop_max = 8;
	srand(time(NULL));

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

	// Randomly generate an initial solution
	stop *route[10];
	for (int j=1;j<=Route_max;j++)
		route[j] = new stop;
	initialsolution(route,TSW_stop,num_of_terminal,num_of_destination,terminal_list,destination_list,Route_max,Time_max,Stop_max,shortest_time_matrix);

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

void initialsolution(stop* route[],node* TSW_stop,const int num_of_terminal,const int num_of_destination,int terminal_list[], int destination_list[], const int Route_max, const int Time_max, const int Stop_max, double shortest_time_matrix[][30])
{
	int check[30];
	int randx;
	bool flag = false;

	while (flag == false)
	{
		for (int j=1;j<= num_of_destination;j++)
			check[j] = 0;

		for (int i=1;i<=Route_max;i++)
		{
			// Randomly select a terminal node
			randx = rand() % num_of_terminal + 1;
			route[i]->index = terminal_list[randx];
			route[i]->next = NULL;

			// Randomly select a destination node
			randx = rand() % num_of_destination + 1;
			add2end(route[i],destination_list[randx]);
			check[randx]++;
		}

		flag = true;
		for (int j=1;j<= num_of_destination;j++)
		{
			if (check[j] == 0)
				flag = false;
		}
		displayallroute(route,Route_max);
	}


};

void add2end(stop*& ptr, int x)
{
	if (ptr == NULL)
	{
		ptr = new stop;
		ptr -> index = x;
		ptr -> next = NULL;
	}
	else
		add2end(ptr->next,x);
}

void displayallroute(stop* route[],const int Route_max)
{
	cout << endl << "Summary: "<< endl;
	cout << "Route [i]: -> [Bus Terminal] (Dummy)-> [Intermediate stop 1] (Shortest travel time from bus terminal to intermediate stop 1)-> [Intermediate stop 2] (Shortest travel time from intermediate stop 1 to intermediate stop 2)-> ..." << endl;
	stop *ptr;
	cout << endl;
	for (int i=1;i<=Route_max;i++)
	{
		cout << "Route " << i << ": ";
		ptr = route[i];
		while (ptr != NULL)
		{
			cout << "-> "<< ptr->index;
			ptr = ptr->next;
		}
		cout << endl;
	}

}
