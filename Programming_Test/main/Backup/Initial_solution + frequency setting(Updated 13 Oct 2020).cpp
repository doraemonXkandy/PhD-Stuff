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
	double shortest; // This variable used for Dijkstra's algorithm
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

struct Eachgene
{
	double obj_value;
	stop *route[10];
	int *frequency[10];
};

int Find_Remaining_Shortest(node* list, int n);
double Dijkstra(node* list, int n, int origin, int destination);
int numofnode();
void inputnetwork(node* list, int* terminal_list, int* destination_list, const int a, const int b,int demand[][6]);
void initialsolution(int count, stop* route[], int n, node* list,const int a,const int b,int*& terminal_list, int*& destination_list, const int R_max, const double T_max, const int S_max, double shortest_time_matrix[][30]);
void displayroute(int i, stop* ptr,const int R_max);
void add2front(stop*& ptr, int x,double t);
void add2end(stop*& ptr, int x,double t);
void delist(stop* ptr[],const int R_max);
bool exist(int x, stop* ptr);
int length(stop* ptr);
double Sumandnextstop(stop*& temp);
void InsertAfter(int pos,stop*& ptr,int x,double t1, double t2);
double Calculate_Time_on_TSW(int num_of_inter_stop,stop* ptr);
void del(stop*& ptr, int key);
void displayallroute(stop* route[],const int R_max);
void updateinitialsolution(stop* route[],int n,node* list,const int num_of_terminal,const int num_of_destination, int* terminal_list,int* destination_list, const int R_max, const double T_max,const int S_max, double shortest_time_matrix[][30]);


int main()
{
	const int num_of_terminal = 7, num_of_destination = 5;
	const int R_max = 10;	// route max
	const double T_max = 35;	// Time max
	const int S_max = 8;	// Intermediate stop max
	int count = 1; // Let the randomize process won't hit the same number again
	int *check2;
	check2 = new int[23];
	stop *ptr;
	int n;
	int demand[24][6];
	double shortest_time_matrix[30][30];
	const int population_size = 20;
	const int selection_size = 40;


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
	inputnetwork(mylist,terminal_list,destination_list,num_of_terminal,num_of_destination,demand);

	// Calculate shortest_time_matrix
	for (int i=1;i<=29;i++)
		for (int j=1;j<=29;j++)
			shortest_time_matrix[i][j] = Dijkstra(mylist,n,i,j);

	// Checking shortest_time_matrix

	//for (int i=1;i<=29;i++)
	//{
	//	for (int j=1;j<=29;j++)
	//	{
	//		cout << shortest_time_matrix[i][j] << " ";
	//	}
	//	cout << endl;
	//}

	Eachgene gene[population_size]; // gene no.0-19
	

	for (int i=0;i<population_size;i++)
	{		
		// Evaluate initial solution of bus route
		{
		gene[i].obj_value = 0;

		for (int j=1;j<=R_max;j++)
		{
			gene[i].route[j] = new stop;
			gene[i].frequency[j] = new int;
		}
		initialsolution(count,gene[i].route,n,mylist,num_of_terminal,num_of_destination,terminal_list,destination_list,R_max,T_max,S_max,shortest_time_matrix);

		displayallroute(gene[i].route,R_max);

		// This "flag" checks if the routes contain all 1-23 nodes
		bool flag = false;


		for (int j=1;j<= 23;j++)
			check2[j] = 0;

		for (int j=1;j<= R_max;j++)
		{
			ptr = gene[i].route[j];
			while (ptr->index <= 23)
			{
				for (int k=1;k<= 23;k++)
				{
					if (ptr->index == k)
						check2[k]++;
				}
				ptr = ptr->next;
			}
		}

			for (int j=1;j<= 23;j++)
			{
				cout << check2[j] << "," ;
			}
			cout << endl;

		for (int j=1;j<= 23;j++)
		{
			if (check2[j] == 0)
				flag = true;
		}

		while (flag == true)
		{
			cout << endl << endl;

			flag = false;
			delist(gene[i].route,R_max);
			for (int j=1;j<=R_max;j++)
				gene[i].route[j] = new stop;

			count++;
			initialsolution(count,gene[i].route,n,mylist,num_of_terminal,num_of_destination,terminal_list,destination_list,R_max,T_max,S_max,shortest_time_matrix);
			for (int j=1;j<= 23;j++)
				check2[j] = 0;

			for (int j=1;j<= R_max;j++)
			{
				ptr = gene[i].route[j];
				while (ptr->index <= 23)
				{
					for (int k=1;k<= 23;k++)
					{
						if (ptr->index == k)
							check2[k]++;
					}
					ptr = ptr->next;
				}
			}
			for (int j=1;j<= 23;j++)
			{
				if (check2[j] == 0)
					flag = true;
			}
			displayallroute(gene[i].route,R_max);
			for (int j=1;j<= 23;j++)
			{
				cout << check2[j] << "," ;
			}
			cout << endl;
		}


		cout << endl << endl;
		updateinitialsolution(gene[i].route,n,mylist,num_of_terminal,num_of_destination,terminal_list,destination_list,R_max,T_max,S_max,shortest_time_matrix);
		}
	// End of initial solution

	}


	// Display gene 0-19
	for (int i=0; i< population_size;i++)
	{
		cout << endl << "gene[" << i << "]";
		cout << endl << gene[i].obj_value;
		displayallroute(gene[i].route,R_max);
		cout << endl;
		for (int j=1; j<= R_max;j++)
			cout << gene[i].frequency[j] << ',';
		cout << endl; 
	}

	// Display demand
	cout << endl << "demand: " << endl;
	for (int i=1;i<=23;i++)
	{
		for (int j=1;j<=5;j++)
		{
			cout << demand[i][j] << ",";
		}
		cout << endl;
	}

	// Return the memory to OS
	delete [] check2;
	delete [] mylist;
	delete [] terminal_list;
	delete [] destination_list;

	for (int i=0;i<population_size;i++)
	{
		delist(gene[i].route,R_max);
		delete [] gene[i].frequency;
	}
	return 0;
}

void updateinitialsolution(stop* route[],int n,node* list,const int num_of_terminal,const int num_of_destination, int* terminal_list,int* destination_list, const int R_max, const double T_max,const int S_max, double shortest_time_matrix[][30])
{
	for (int i=1;i<= num_of_destination;i++)
		cout << destination_list[i] << ",";

	// To ensure all destination are present
	int *check;
	check = new int[num_of_destination];

	for (int i=1;i<= num_of_destination;i++)
		check[i] = 0;

	stop *ptr;

	for (int i=1;i<= R_max;i++)
	{
		ptr = route[i];
		while (ptr->next != NULL)
		{
			ptr = ptr->next;
		}
		for (int j=1;j<= num_of_destination;j++)
		{
			if (ptr->index == destination_list[j])
				check[j]++;
		}
	}


	int key,max,add;
	add = -1;
	max = check[1];
	key = destination_list[1];
	
	for (int i=1;i<= num_of_destination;i++)
	{
		if (check[i] > max)
		{
			max = check[i];
			key = destination_list[i];
		}
		if (check[i] == 0)
			add = destination_list[i];
	}

	stop *ptr2;
	double t;
	while (add != -1)
	{
		for (int i=1;i<= R_max;i++)
		{
			ptr = route[i];
			while (ptr->next != NULL)
			{
				ptr2 = ptr;
				ptr = ptr->next;
			}
			if (ptr->index == key)
			{
				
				for (int j=1;j<= num_of_destination;j++)
				{
					if (add == destination_list[j])
						check[j]++;
				}
				t = shortest_time_matrix[ptr2->index][add];
				del(route[i],key);
				add2end(route[i],add,t);
				
				break;
			}
		}
		add = -1;
		max = check[1];
		key = destination_list[1];
		
		for (int i=1;i<= num_of_destination;i++)
		{
			if (check[i] > max)
			{
				max = check[i];
				key = destination_list[i];
			}
			if (check[i] == 0)
				add = destination_list[i];
		}
	}
	displayallroute(route,R_max);

	// Add node 29
	int pos;
	double t1,t2;
	for (int i=1;i<= R_max;i++)
	{
		pos = 0;
		ptr = route[i];
		while (ptr->next != NULL)
		{
			pos++;
			ptr2 = ptr;
			ptr = ptr->next;
		}
		t1 = shortest_time_matrix[ptr2->index][29];
		t2 = shortest_time_matrix[29][ptr->index];
		InsertAfter(pos,route[i],29,t1,t2);

	}

	displayallroute(route,R_max);

 	delete [] check;



};

void initialsolution(int count, stop* route[], int n, node* list,const int a,const int b,int*& terminal_list, int*& destination_list, const int R_max, const double T_max, const int S_max, double shortest_time_matrix[][30])
{
	srand (time(NULL));
	int x;
	int pos;
	int num_of_inter_stop;

	bool time_exceed,flag;
	double t,Tn,t3;
	double time_sum, time1, time2;
	double shortest_time_sum=9999, shortest_time1, shortest_time2;
	stop *temp;
	stop *temp2;
	stop *temp3;

	for (int i=1;i<=R_max;i++)
	{
		// Randomly select a terminal node
		for (int j=1;j<= count;j++)
			x = rand() %a + 1;
		route[i]->index=terminal_list[x];
		route[i]->time_to_next = 0;
		route[i]->next = NULL;

		cout << endl << "Bus terminal: Node " << terminal_list[x] << " is selected." << endl;
		// Randomly select a destination node
		for (int j=1;j<= count;j++)
			x = rand() %b + 1;
		t = shortest_time_matrix[route[i]->index][destination_list[x]];
		add2end(route[i],destination_list[x],t);

		cout << "Destination: Node " << destination_list[x] << " is selected." << endl;

		displayroute(i,route[i],R_max);
		Tn = 0;
		num_of_inter_stop = 0;
		time_exceed = false;

		while (time_exceed == false && num_of_inter_stop <= S_max)
		{

			// Randomly insert the stops in between with no repeat & no time exceed
			for (int j=1;j<= count;j++)
				x = rand() %23 + 1;
			flag = exist(x,route[i]);
			while (flag == true)
			{
				for (int j=1;j<= count;j++)
					x = rand() %23 + 1;
				flag = exist(x,route[i]);
			}
			shortest_time_sum = 9999;
			cout << endl << "Finding the best position to insert Node " << x << "." << endl; 
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

				time1 = shortest_time_matrix[temp2->index][x];
				time_sum += time1;
				temp2 = temp2 -> next;
				time2 = shortest_time_matrix[x][temp2->index];
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
			InsertAfter(pos,route[i],x,shortest_time1,shortest_time2);
			cout << "Insert node " << x << "." << endl;
			displayroute(i,route[i],R_max);
			cout << endl;
			num_of_inter_stop++;
			Tn = Calculate_Time_on_TSW(num_of_inter_stop,route[i]);
			if (Tn > T_max)
				time_exceed = true;
		}
		
		temp3 = route[i];
		for (int j=1;j<pos;j++)
			temp3 = temp3 -> next;
		t3 = shortest_time_matrix[temp3->index][temp3->next->next->index];
		temp3->next->next->time_to_next = t3;
		del(route[i],x);
		cout << "Overload. Remove node " << x << "." << endl;
		displayroute(i,route[i],R_max);
	}

};

void displayallroute(stop* route[],const int R_max)
{
	cout << endl << "Summary: "<< endl;
	cout << "Route [i]: -> [Bus Terminal] (Dummy)-> [Intermediate stop 1] (Shortest travel time from bus terminal to intermediate stop 1)-> [Intermediate stop 2] (Shortest travel time from intermediate stop 1 to intermediate stop 2)-> ..." << endl;
	stop *ptr;
	cout << endl;
	for (int i=1;i<=R_max;i++)
	{
		cout << "Route " << i << ": ";
		ptr = route[i];
		while (ptr != NULL)
		{
			cout << "-> "<< ptr->index << " (" << ptr->time_to_next << ")";
			ptr = ptr->next;
		}
		cout << endl;
	}

}

void del(stop*& ptr, int key)
{
	if (ptr != NULL)
	{
		if (ptr->index == key)
		{
			stop *temp;
			temp = ptr;
			ptr = ptr->next;
			delete temp;
		}
	
		else
			del (ptr->next,key);
	}
};

double Calculate_Time_on_TSW(int num_of_inter_stop,stop* ptr)
{
	const double s = 1.5;
	double sum = 0;
	sum += num_of_inter_stop * s;
	for (int i=0;i<= num_of_inter_stop;i++)
	{
		sum += ptr->time_to_next;
		ptr = ptr->next;
	}
	return sum;

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
	temp -> next -> time_to_next = t2;
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
	while (ptr != NULL)
	{
		if (ptr->index == x)
			return true;
		else
			ptr = ptr->next;
	}
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

void displayroute(int i, stop* ptr,const int R_max)
{
		cout << "Route " << i << ": ";
		while (ptr != NULL)
		{
			cout << "-> "<< ptr->index << " (" << ptr->time_to_next << ")";
			ptr = ptr->next;
		}
		cout << endl;
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
//	cout << "Shortest time: " << list[destination].shortest << endl;
//	cout << "The path: " << endl;
//	cout << destination << " <- ";
//	temp = list[destination].pred;
	
//	while (temp != origin)
//	{
//		cout << temp << " <- ";
//		temp = list[temp].pred;
//	}
//	cout << origin << endl;

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

void inputnetwork(node* list, int* terminal_list, int* destination_list, const int a, const int b, int demand[][6])
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
	getline(fin,line);

	// Input demand below

	for (int i=1;i<=23;i++)
	{
		for (int j=1;j<=5;j++)
		{
			fin >> demand[i][j];
			fin >> dummy;
		}
		getline(fin,line);
	}
	fin.close();
};

