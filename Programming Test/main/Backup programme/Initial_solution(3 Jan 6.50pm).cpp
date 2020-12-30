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
	path link[7];
};

struct stop
{
	int index;
	double time_to_next;
	stop *next;
};

int Find_Remaining_Shortest(node* list, int n);
double Dijkstra(node* list, int n, int origin, int destination);
int numofnode();
void inputnetwork(node* list, int* terminal_list, int* destination_list, const int a, const int b);
void initialsolution(stop* route[], int n, node* list,const int a,const int b,int*& terminal_list, int*& destination_list, const int R_max, const double T_max);
void displayroute(stop* ptr[],const int R_max);
void add2front(stop*& ptr, int x,double t);
void add2end(stop*& ptr, int x,double t);
void delist(stop* ptr[],const int R_max);
bool exist(int x, stop* ptr);
int length(stop* ptr);
double Sumandnextstop(stop*& temp);
void InsertAfter(int pos,stop*& ptr,int x,double t1, double t2);

int main()
{
	const int num_of_terminal = 7, num_of_destination = 5;
	const int R_max = 10;
	const double T_max = 35;

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

	// Evaluate initial solution of bus route
	stop *route[R_max];
	for (int i=1;i<=R_max;i++)
		route[i] = new stop;
	initialsolution(route,n,mylist,num_of_terminal,num_of_destination,terminal_list,destination_list,R_max,T_max);

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


	// Return the memory to OS
	delete [] mylist;
	delete [] terminal_list;
	delete [] destination_list;
	delist(route,R_max);
	return 0;
}

void initialsolution(stop* route[], int n, node* list,const int a,const int b,int*& terminal_list, int*& destination_list, const int R_max, const double T_max)
{
	srand (time(NULL));
	int x;
	int pos;

	double t;
	double time_sum, time1, time2;
	double shortest_time_sum=9999, shortest_time1, shortest_time2;
	stop *temp;
	stop *temp2;

	for (int i=1;i<=10;i++)
	{
		// Randomly select a terminal node
		x = rand() %a + 1;
		route[i]->index=terminal_list[x];
		route[i]->time_to_next = 0;
		route[i]->next = NULL;

		// Randomly select a destination node
		x = rand() %b + 1;
		t = Dijkstra(list, n, route[i]->index, destination_list[x]);
		add2end(route[i],destination_list[x],t);


		// Randomly inset the stops in between with no repeat & no time exceed
		x = rand() %23 + 1;
		while (exist(x,route[i]) == true)
		{
			x = rand() %23 + 1;
			exist(x,route[i]);
		}
		shortest_time_sum = 9999;


		// Find which position result in min. travel time
		for (int j=1; j<=length(route[i])-1 ;j++)
		{
			time_sum = 0;
			temp = route[i];
			for (int k=1;k<=j;k++)
				time_sum += Sumandnextstop(temp);
			
			temp2 = route[i];
			for (int k=1;k<j;k++)
				temp2 = temp2 -> next;

			time1 = Dijkstra(list,n,temp2->index,x);
			time_sum += time1;
			temp2 = temp2 -> next;
			time2 = Dijkstra(list,n,x,temp2->index);
			time_sum += time2;

			temp = temp -> next;
			for (int k=j; k<length(route[i])-1;k++)
				time_sum += Sumandnextstop(temp);

			if (time_sum < shortest_time_sum)
			{
				shortest_time_sum = time_sum;
				pos = j;
				shortest_time1 = time1;
				shortest_time2 = time2;
			}
		}
		InsertAfter(pos,route[i],x,shortest_time1,shortest_time2 = time2);
	}
	displayroute(route,R_max);
};

void InsertAfter(int pos,stop*& ptr,int x,double t1, double t2)
{
	stop *temp;
	stop *temp2;
	temp2 = ptr;
	for (int i=1;i<pos;i++)
	{
		temp2 = temp2->next;
	}
	temp = new stop;
	temp -> index = x;
	temp -> time_to_next = t1;
	temp -> next = temp2 -> next;
	temp2 -> next = temp;
};

double Sumandnextstop(stop*& temp)
{
	double a = 0;
	a = temp->time_to_next;
	temp = temp->next;
	return a;
}

int length(stop* ptr)
{
	int n=0;
	while (ptr!=NULL)
	{
		n++;
		ptr = ptr->next;
	}
	return n;
}

bool exist(int x, stop* ptr)
{
	if (ptr != NULL)
	{
		if (ptr->index == x)
			return true;
	}
	else
		exist(x,ptr->next);
	return false;
};

void add2front(stop*& ptr, int x,double t)
{
	stop *temp;
	temp = new stop;
	temp->index = x;
	temp->time_to_next=0;
	temp->next = ptr;
	temp->next->time_to_next=t;
	ptr = temp;
};

void add2end(stop*& ptr, int x,double t)
{
	if (ptr == NULL)
	{
		ptr = new stop;
		ptr->index = x;
		ptr->time_to_next = t;
		ptr->next = NULL;
	}
	else
		add2end(ptr->next,x,t);
}

void displayroute(stop* ptr[],const int R_max)
{
	for (int i=1; i<= R_max;i++)
	{
		cout << "Route " << i << ": ";
		while (ptr[i] != NULL)
		{
			cout << "-> "<< ptr[i]->index << " (" << ptr[i]->time_to_next << ")";
			ptr[i] = ptr[i]->next;
		}
		cout << endl;
	}
};

void delist (stop* ptr[],const int R_max)
{
	stop *temp;
	for (int i=1; i<=R_max;i++)
	{
		while (ptr[i] != NULL)
		{
			temp = ptr[i];
			ptr[i] = ptr[i]->next;
			delete temp;
		}
	}
	ptr = NULL;
};

double Dijkstra(node* list, int n, int origin, int destination)
{
	int key,next,temp;
	temp = 0;

	for (int i=1;i<=n;i++)
	{
		list[i].shortest = 9999;
		list[i].pred = 0;
		list[i].flag = false;
	}
	list[origin].pred = 0;
	list[origin].shortest = 0;


	// start finding the solution
	for (int i=1; i<=n;i++)
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

	// For checking
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

	return list[destination].shortest;
};

int Find_Remaining_Shortest(node* list, int n)
{
	double shortest_time = 9999;
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

