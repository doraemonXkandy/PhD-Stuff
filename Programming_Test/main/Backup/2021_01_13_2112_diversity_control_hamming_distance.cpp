#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cmath>
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

struct chromosome
{
	stop *route[11];
	double num_of_bus[11];
	double total_trip_time[11];
	double Objective;
	bool direct_service[24][30];
};



void inputnetwork(node TSW_stop[], int terminal_list[], int destination_list[], const int num_of_terminal, const int num_of_destination, int demand[][30]);
double Dijkstra(node TSW_stop[], int origin, int destination);
void initialsolution(stop* route[],node* TSW_stop,const int num_of_terminal,const int num_of_destination,int terminal_list[], int destination_list[], const int Route_max, const int Time_max, const int Stop_max, double shortest_time_matrix[][30]);
void add2end(stop*& ptr, int x);
void displayallroute(stop* route[],const int Route_max);
void delist (stop* ptr[],const int Route_max);
void deepCopy(stop* ptr, stop*& copy);
bool exist(int x, stop* ptr);
int length(stop* ptr);	// Total stop (include origin and destination)
void InsertAtPos(int pos, stop*& ptr, int randx); // pos, that route, stop 
double Calculate_Total_Route_Time_on_TSW(int num_of_inter_stop, stop* ptr, double shortest_time_matrix[][30]);
void del(stop*& ptr, int key);
void replace_node(stop*& ptr, stop* ptr2, int key,int new_node, bool& flag_replace, double shortest_time_matrix[][30], const double Time_max); // Change key to new_node // Do repairing for 1 node only
void Calculate_Total_trip_time(double& total_trip_time, stop* ptr, double shortest_time_matrix[][30]);
void Update_frequency_setting(stop* route[], double num_of_bus[],double total_trip_time[],double& Objective,bool direct_service[][30],int demand[][30],double shortest_time_matrix[][30],const int Route_max); // Update for 1 gene
void Crossover_route(chromosome Parent1,chromosome Parent2, chromosome& Child1, chromosome& Child2);
void Crossover_stop(chromosome Parent1,chromosome Parent2, chromosome& Child1, chromosome& Child2, double shortest_time_matrix[][30]);
void deepCopy_start_from(stop* ptr, stop*& copy, int start_from, int numofnodes, int count_node);
void deepCopy_numofnodes(stop* ptr,stop*& copy,int numofnodes, int count_node2);
void check_repeated_node(stop* ptr, int& repeated_node);
void Mutation_insert(stop* route[]);
void Mutation_remove(stop* route[]);
void Mutation_swap(stop* route[]);
void Mutation_transfer(stop* route[]);
void stop_sequence_improvement(chromosome& Child,double shortest_time_matrix[][30], const int Route_max);
void check_repair(chromosome& Child,double shortest_time_matrix[][30], const int Route_max);
void repair_operator(stop*& ptr,double shortest_time_matrix[][30]);
void repair_missing(chromosome& Child,double shortest_time_matrix[][30], const int Route_max, const double Time_max);
void del_gene(chromosome& parent, const int Route_max);
void deepCopy_gene(chromosome parent, chromosome& copy, const int Route_max);
void Diversity_control(chromosome gene[], const int Route_max);

int main()
{
	clock_t start = clock();
	const int num_of_terminal = 7, num_of_destination = 5;
	const int Route_max = 10;
	const double Time_max = 35;
	const int Stop_max = 8;
	srand(time(NULL));


	int terminal_list[num_of_terminal+1];
	int destination_list[num_of_destination+1];
	node TSW_stop[30];
	double shortest_time_matrix[30][30];
	int demand[24][30];

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
	inputnetwork(TSW_stop,terminal_list,destination_list,num_of_terminal,num_of_destination,demand);
	cout << "demand:" << endl;
	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=28;j++)
			cout << demand[i][j] << " ";
		cout << endl;
	}
	// Calculate shortest_time_matrix
	for (int i=1;i<=29;i++)
		for (int j=1;j<=29;j++)
			shortest_time_matrix[i][j] = Dijkstra(TSW_stop,i,j);

	// Randomly generate 20 initial solutions
	chromosome gene[41];
	for (int i=1;i<=20;i++)
	{
		for (int j=1;j<=Route_max;j++)
			gene[i].route[j] = new stop;
		initialsolution(gene[i].route,TSW_stop,num_of_terminal,num_of_destination,terminal_list,destination_list,Route_max,Time_max,Stop_max,shortest_time_matrix);
		for (int j=1;j<=Route_max;j++)
		{
			gene[i].total_trip_time[j] = 0;
			Calculate_Total_trip_time(gene[i].total_trip_time[j],gene[i].route[j],shortest_time_matrix);
		}
		for (int j=1;j<=Route_max;j++)
		{
			if (j<=6)
				gene[i].num_of_bus[j] = 18;
			else 
				gene[i].num_of_bus[j] = 17;
		}
	}

	// allocate optimal frequency setting
	for (int i=1;i<=20;i++)
	{
		Update_frequency_setting(gene[i].route,gene[i].num_of_bus,gene[i].total_trip_time,gene[i].Objective,gene[i].direct_service,demand,shortest_time_matrix,Route_max);
	}

	// Display 20 genes
	for (int i=1;i<=20;i++)
	{
		cout << endl << "gene[" << i << "] 10 routes:" << endl; 
		displayallroute(gene[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].total_trip_time[j] << " ";
		cout << endl;
	}

	int rand_num;
	// Do route/stop crossover
	for (int i=1;i<=19;i=i+2)
	{
		rand_num = rand()%2 + 1;
		if (rand_num == 1)
		{
			cout << endl << "Do route crossover" << endl;
			Crossover_route(gene[i],gene[i+1],gene[i+20],gene[i+1+20]);
		}
		if (rand_num == 2)
		{
			cout << endl << "Do stop crossover" << endl;
			Crossover_stop(gene[i],gene[i+1],gene[i+20],gene[i+1+20],shortest_time_matrix);
		}

	}

	// Display 40 genes (Parents + offspring)
	for (int i=1;i<=40;i++)
	{
		cout << endl << "gene[" << i << "] 10 routes:" << endl; 
		displayallroute(gene[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].total_trip_time[j] << " ";
		cout << endl;
	}

	// Do mutation
	for (int i=21;i<=40;i++)
	{
		rand_num = rand()%10+1;
		if (rand_num >=1 && rand_num <=4)
		{
			cout << endl << "gene["  << i << "] do insert " << endl;
			Mutation_insert(gene[i].route);
		}
		if (rand_num >=5 && rand_num <=8)
		{
			cout << endl << "gene["  << i << "] do remove " << endl;
			Mutation_remove(gene[i].route);
		}
		if (rand_num ==9)
		{
			cout << endl << "gene["  << i << "] do swap " << endl;
			Mutation_swap(gene[i].route);

		}
		if (rand_num ==10)
		{
			cout << endl << "gene["  << i << "] do transfer " << endl;
			Mutation_transfer(gene[i].route);
		}
	}

	// Display 40 genes (Parents + offspring)
	for (int i=1;i<=40;i++)
	{
		cout << endl << "gene[" << i << "] 10 routes:" << endl; 
		displayallroute(gene[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].total_trip_time[j] << " ";
		cout << endl;
	}

	// Stop sequence improvement heuristic
	for (int i=21;i<=40;i++)
	{
		cout << endl << "gene[" << i << "]: " << endl;
		stop_sequence_improvement(gene[i],shortest_time_matrix,Route_max);
	}

	// Display 40 genes (Parents + offspring)
	for (int i=1;i<=40;i++)
	{
		cout << endl << "gene[" << i << "] 10 routes:" << endl; 
		displayallroute(gene[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].total_trip_time[j] << " ";
		cout << endl;
	}

	// Repair operator
	for (int i=21;i<=40;i++)
	{
		cout << endl << "gene[" << i << "]: " << endl;
		check_repair(gene[i],shortest_time_matrix,Route_max);
	}

	// Display 40 genes (Parents + offspring)
	for (int i=1;i<=40;i++)
	{
		cout << endl << "gene[" << i << "] 10 routes:" << endl; 
		displayallroute(gene[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].total_trip_time[j] << " ";
		cout << endl;
	}

	// Repair missing nodes (if missing nodes <= 1 -> repair, otherwise delete it)
	for (int i=21;i<=40;i++)
	{
		cout << endl << "gene[" << i << "]: " << endl;
		repair_missing(gene[i],shortest_time_matrix,Route_max,Time_max);
	}

	// Display 40 genes (Parents + offspring)
	for (int i=1;i<=40;i++)
	{
		cout << endl << "gene[" << i << "] 10 routes:" << endl; 
		displayallroute(gene[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].total_trip_time[j] << " ";
		cout << endl;
	}

	// allocate optimal frequency setting for childs
	for (int i=21;i<=40;i++)
	{
		cout << endl << "gene[" << i << "]: optimal frequency2" << endl;
		if (gene[i].route[1]->index != -1)
			Update_frequency_setting(gene[i].route,gene[i].num_of_bus,gene[i].total_trip_time,gene[i].Objective,gene[i].direct_service,demand,shortest_time_matrix,Route_max);
		else gene[i].Objective = 999999999;
	}

	// Display 40 genes (Parents + offspring)
	for (int i=1;i<=40;i++)
	{
		cout << endl << "gene[" << i << "] 10 routes:" << endl; 
		displayallroute(gene[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].total_trip_time[j] << " ";
		cout << endl << "Objective: " << gene[i].Objective << endl;

	}

	// Diversity control
	cout << endl << "Diversity control" << endl;
	Diversity_control(gene,Route_max);

	// Display 40 genes (Parents+ offspring)
	for (int i=1;i<=40;i++)
	{
		cout << endl << "gene[" << i << "] 10 routes:" << endl; 
		displayallroute(gene[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << gene[i].total_trip_time[j] << " ";
		cout << endl << "Objective: " << gene[i].Objective << endl;
	}


	clock_t end = clock();
	double elapsed = double(end - start)/CLOCKS_PER_SEC;
 	cout << "Time measured: " << elapsed << "seconds." << endl;
	return 0;
}

void inputnetwork(node TSW_stop[], int terminal_list[], int destination_list[], const int num_of_terminal, const int num_of_destination, int demand[][30])
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
	getline(fin, line);

	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=28;j++)
			fin >> demand[i][j] >> dummy;
		getline(fin, line);
	}
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
	int check[30];	// For checking if all destination/stop exists
	int randx;	// variable for 
	bool flag = false;	// For checking if all destination exists
	bool flag_miss = false; // For checking if any stop missed
	bool flag_repeat = true; // For checking if the stops already appear within that route
	bool time_exceed = false;

	int num_of_inter_stop = 0;
	bool flag_replace;
	int set_of_check0[24]; // For repairing
	int set_of_check2[24];	// For repairing
	int index_for_check0; // For repairing
	int index_for_check2; // For repairing
	double shortest_travel_time, travel_time_ABC;
	double shortest_travel_time_for_repairing; // For repairing
	int closest_node; // For repairing
	int temp_shortest_pos;
	stop* temp_ptr;

	int count=0;
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

	displayallroute(route,Route_max);
	stop *temp[11];

	for (int i=1;i<=Route_max;i++)
		deepCopy(route[i],temp[i]);
	
	while (flag_miss == false)
	{
		count++;
		cout << "count: " << count << endl;

		// Return back to origin and destination only
		delist(route,Route_max);
		
		for (int j=1;j<=Route_max;j++)
			deepCopy(temp[j],route[j]);
		displayallroute(route,Route_max);

		// Initialization of check parameter (Check if any missed stops)
		for (int j=1;j<= 23;j++)
			check[j] = 0;
		for (int j=1;j<= Route_max;j++)
		{
			check[route[j]->index]++;
		}

		for (int i=1;i<=Route_max;i++)
		{
			num_of_inter_stop = 0;
			time_exceed = false;

			// Inserting stop until one of the constraints is violated
			while (time_exceed == false && num_of_inter_stop <= Stop_max)
			{
				flag_repeat = true;
				// Randomly select a stop with no repeat
				while (flag_repeat == true) 
				{
					randx = rand()%23 + 1;
					flag_repeat = exist(randx, route[i]);
				}

				cout << endl << "Finding the best position to insert Node" << randx << "." << endl;
				// Find which position result in minimum travel time

				shortest_travel_time = 9999;
				for (int j=1;j<=length(route[i])-1;j++)
				{
					travel_time_ABC = 0;
					temp_ptr = route[i];
					for (int k=1;k<j;k++)
						temp_ptr = temp_ptr -> next;
					travel_time_ABC += shortest_time_matrix[temp_ptr->index][randx] + shortest_time_matrix[randx][temp_ptr->next->index] - shortest_time_matrix[temp_ptr->index][temp_ptr->next->index];
					if (travel_time_ABC < shortest_travel_time)
					{
						shortest_travel_time = travel_time_ABC;
						temp_shortest_pos = j;
					}
				}
				InsertAtPos(temp_shortest_pos,route[i],randx);
				check[randx]++;
				displayallroute(route,Route_max);
				num_of_inter_stop++;
				cout << endl << "Number of inter stop:" << num_of_inter_stop << endl;
				cout << endl << "Total time:  " << Calculate_Total_Route_Time_on_TSW(num_of_inter_stop,route[i],shortest_time_matrix) << endl;
				if (Calculate_Total_Route_Time_on_TSW(num_of_inter_stop,route[i],shortest_time_matrix) > Time_max)
				{
					time_exceed = true;
				}
			}
			// Delete the latest node since it violates one of the constraints
			del(route[i],randx);
			check[randx]--;
		}
		flag_miss = true;

		// Start repairing the randomly generate solution
		// Replace the repeated nodes with missed nodes
		// Find the repeated nodes which have the closest distance with the missed nodes

		for (int j=1;j<= 23;j++)
		{
			set_of_check0[j] = 0;
			set_of_check2[j] = 0; 
		}
		index_for_check0 = 1;
		index_for_check2 = 1;

		for (int j=1;j<=23;j++)
		{
			if (check[j] == 0)
			{
				set_of_check0[index_for_check0] = j;
				index_for_check0++;
			}
			if (check[j] > 1)
			{
				set_of_check2[index_for_check2] = j;
				index_for_check2++;
			}
		}

		// Debug
		cout << endl;
		for (int j=1;j<index_for_check0;j++)
		{
			cout << set_of_check0[j] << " ";
		}
		cout << endl;
		for (int j=1;j<index_for_check2;j++)
		{
			cout << set_of_check2[j] << " ";
		}
		cout << endl;
		//Debug

		// Only repair if num of missed nodes <= 1
		if (index_for_check0-1 <= 1)
		{
			for (int j=1;j<index_for_check0;j++)
			{
				shortest_travel_time_for_repairing = 9999;
				closest_node = 99;
				for (int k=1;k<index_for_check2;k++)
				{
					if (shortest_time_matrix[set_of_check0[j]][set_of_check2[k]] < shortest_travel_time_for_repairing)
					{
						shortest_travel_time_for_repairing = shortest_time_matrix[set_of_check0[j]][set_of_check2[k]];
						closest_node = k;
					}
				}
				flag_replace = false;
				cout << "Closest node: " << set_of_check2[closest_node] << endl;
				for (int a=1;a<=Route_max;a++)
				{
					replace_node(route[a]->next,route[a],set_of_check2[closest_node],set_of_check0[j],flag_replace,shortest_time_matrix,Time_max);
				}
				if (flag_replace == true)
				{
					check[set_of_check2[closest_node]]--;
					check[set_of_check0[j]]++;
				}
				displayallroute(route,Route_max);
			}
		}

		for (int j=1;j<= 23;j++)
		{
			if (check[j] == 0)
				flag_miss = false;
		}
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
	cout << "Route [i]: -> [Bus Terminal] -> [Intermediate stop 1] -> [Intermediate stop 2] -> ..." << endl;
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

void delist (stop* ptr[],const int Route_max)
{
	stop *temp;
	for (int i=1; i<=Route_max;i++)
	{
		while (ptr[i] != NULL)
		{
			temp = ptr[i];
			ptr[i] = ptr[i]->next;
			delete temp;
		}
	}
	for (int i=1; i<=Route_max;i++)
	{
		ptr[i] = NULL;
	}
};

void deepCopy(stop* ptr, stop*& copy)
{		
	if (ptr != NULL)
	{		
		copy = new stop;
		copy->index = ptr->index;
		copy->next = NULL;
		deepCopy(ptr->next,copy->next);
	}
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
}

int length(stop* ptr)
{
	int n=0;
	while (ptr!= NULL)
	{
		n++;
		ptr = ptr-> next;
	}
	return n;
}

void InsertAtPos(int pos, stop*& ptr, int randx)
{
	stop *temp;
	stop *temp2;
	temp2 = ptr;
	for (int i=1;i<pos;i++)
	{
		temp2 = temp2->next;
	}
	temp = new stop;
	temp -> index = randx;
	temp -> next = temp2 -> next;
	temp2 -> next = temp;
}

double Calculate_Total_Route_Time_on_TSW(int num_of_inter_stop, stop* ptr, double shortest_time_matrix[][30])
{
	const double s = 1.5;
	double sum = 0;
	stop* temp;
	temp = ptr;
	sum += num_of_inter_stop * s;
	for (int i=1;i<=num_of_inter_stop;i++)
	{
		sum += shortest_time_matrix[temp->index][temp->next->index];
		temp = temp->next;
	}
	sum += shortest_time_matrix[temp->index][29];
	return sum;
}

void del(stop*& ptr, int key)
{
	if (ptr != NULL)
	{
		if (ptr->index == key)
		{
			stop* temp;
			temp = ptr;
			ptr = ptr -> next;
			delete temp;
		}
		else
			del(ptr->next,key);
	}
}

void replace_node(stop*& ptr, stop* ptr2, int key,int new_node, bool& flag_replace, double shortest_time_matrix[][30], const double Time_max)
{

	if (ptr != NULL)
	{
		if (ptr->index == key && flag_replace == false)
		{
			ptr->index = new_node;
			cout << endl << "TOtal time in TSW" << Calculate_Total_Route_Time_on_TSW(length(ptr2)-2,ptr2,shortest_time_matrix) << endl;
			if (Calculate_Total_Route_Time_on_TSW(length(ptr2)-2,ptr2,shortest_time_matrix) < Time_max)
			{
				
				flag_replace = true;
			}
			else 
				ptr->index = key;
			replace_node(ptr->next, ptr2, key, new_node, flag_replace, shortest_time_matrix, Time_max);
		}
		else
			replace_node(ptr->next, ptr2, key, new_node, flag_replace, shortest_time_matrix, Time_max);

	}

}

void Calculate_Total_trip_time(double& total_trip_time, stop* ptr, double shortest_time_matrix[][30])
{
	const double s = 1.5;
	total_trip_time = 0;
	stop* temp;
	temp = ptr;
	while (temp->next != NULL)
	{
		total_trip_time = total_trip_time + s + shortest_time_matrix[temp->index][temp->next->index];
		temp = temp->next;
	}
	total_trip_time -= s;
}

void Update_frequency_setting(stop* route[], double num_of_bus[],double total_trip_time[],double& Objective,bool direct_service[][30],int demand[][30],double shortest_time_matrix[][30], const int Route_max)
{
	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=28;j++)
			direct_service[i][j] = false;
	}

	stop* temp; 
	stop* temp2;

	// See if there is any direct service
	for (int i=1;i<=Route_max;i++)
	{
		// Find the destination
		temp = route[i];
		temp2 = route[i];
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		// Turn each direct service to the above destination to true
		while (temp2->next->next != NULL)
		{
			direct_service[temp2->index][temp->index] = true;
			temp2 = temp2->next;
		}
		direct_service[temp2->index][temp->index] = true;
	}

	// debug
	cout << endl << "direct_service_matrix" << endl;
	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=28;j++)
			cout << direct_service[i][j] << " ";
	}
	// debug

	double average_travel_time;
	double nominator[24][30];
	double nominator_tran1[24][30];

	double denominator[24][30];
	double denominator_tran1[24][30];

	double nominator_tran2[30][30];
	double denominator_tran2[30][30];
	
	double new_Objective;
	double passed_time;


	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=29;j++)
		{
			nominator[i][j] = 0;
			nominator_tran1[i][j] = 0;
			denominator[i][j] = 0;
			denominator_tran1[i][j] = 0;
		}
	}
	for (int i=1;i<=29;i++)
	{
		for (int j=1;j<=29;j++)
		{
			denominator_tran2[i][j] = 0;
			nominator_tran2[i][j] = 0;
		}
	}

	// Calculate for those O-D pair with direct service
	for (int i=1;i<=Route_max;i++)
	{
		temp = route[i];
		temp2 = route[i];
		passed_time = 0;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		while (temp2->next->next != NULL)
		{
			nominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time);
			denominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i];
			passed_time = passed_time + shortest_time_matrix[temp2->index][temp2->next->index] + 1.5;
			temp2 = temp2->next;
		}
		nominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time);
		denominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i];
	}

	// Calculate for those O-D pair with NO direct service
	for (int i=1;i<=Route_max;i++)
	{
		temp = route[i];
		temp2 = route[i];
		passed_time = 0;
		while (temp->next != NULL)
		{
			temp = temp->next;
		}
		while (temp2->next->next != NULL)
		{
			for (int j=24;j<=28;j++)
			{
				if (direct_service[temp2->index][j] == false)
				{
					nominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time- shortest_time_matrix[29][temp->index]);
					denominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i];
				}
			}
			passed_time = passed_time + shortest_time_matrix[temp2->index][temp2->next->index] + 1.5;
			temp2 = temp2->next;
		}
		for (int j=24;j<=28;j++)
		{
			if (direct_service[temp2->index][j] == false)
			{					
				nominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time- shortest_time_matrix[29][temp->index]);
				denominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i];
			}
		}
		nominator_tran2[29][temp->index] += num_of_bus[i]/2/total_trip_time[i] * shortest_time_matrix[29][temp->index];
		denominator_tran2[29][temp->index] += num_of_bus[i]/2/total_trip_time[i];
	}

	// debug
	cout << endl << "Nominator: " << endl;
	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=28;j++)
		{
			cout << nominator[i][j] << " ";
			
		}
		cout << endl;
	}

	cout << endl << "Nominator_tran1: " << endl;
	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=28;j++)
		{
			cout << nominator_tran1[i][j] << " ";
			
		}
		cout << endl;
	}

	cout << endl << "Nominator_tran2: " << endl;
	for (int i=1;i<=29;i++)
	{
		for (int j=1;j<=29;j++)
		{
			cout << nominator_tran2[i][j] << " ";
			
		}
		cout << endl;
	}
	// debug

	const double B1 = 80;
	const double B2 = 1;
	Objective = 0;
	
	// Calculate original objective function value
	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=28;j++)
		{
			if (direct_service[i][j] == true)
			{
				average_travel_time = (nominator[i][j]+1.0)/denominator[i][j];
				Objective = Objective + B2*average_travel_time* demand[i][j];
			}
			if (direct_service[i][j] == false)
			{
				average_travel_time = (nominator_tran1[i][j]+1.0)/denominator_tran1[i][j] + (nominator_tran2[29][j]+1.0)/denominator_tran2[29][j];
				Objective = Objective + B1 *demand[i][j]+ B2*average_travel_time *demand[i][j];
			}
			cout << endl << i << "," << j << " Objective function: " << Objective << endl;
		}
	}

	bool flag_frequency_change=true;

	// Start finding the optimal frequency
	for (int a=1;a<=Route_max;a++)
	{
		for (int b=a+1;b<=Route_max;b++)
		{
			// move 1 bus from route[a] to route[b]
			flag_frequency_change=true;
			while (flag_frequency_change == true && num_of_bus[a]/2.0/total_trip_time[a]*60.0 >= 4.8)
			{
				num_of_bus[a]--;
				num_of_bus[b]++;
				for (int i=1;i<=23;i++)
				{
					for (int j=24;j<=29;j++)
					{
						nominator[i][j] = 0;
						nominator_tran1[i][j] = 0;
						denominator[i][j] = 0;
						denominator_tran1[i][j] = 0;
					}
				}
				for (int i=1;i<=29;i++)
				{
					for (int j=1;j<=29;j++)
					{
						denominator_tran2[i][j] = 0;
						nominator_tran2[i][j] = 0;
					}
				}

				// Calculate for those O-D pair with direct service
				for (int i=1;i<=Route_max;i++)
				{
					temp = route[i];
					temp2 = route[i];
					passed_time = 0;
					while (temp->next != NULL)
					{
						temp = temp->next;
					}
					while (temp2->next->next != NULL)
					{
						nominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time);
						denominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i];
						passed_time = passed_time + shortest_time_matrix[temp2->index][temp2->next->index] + 1.5;
						temp2 = temp2->next;
					}
					nominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time);
					denominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i];
				}

				// Calculate for those O-D pair with NO direct service
				for (int i=1;i<=Route_max;i++)
				{
					temp = route[i];
					temp2 = route[i];
					passed_time = 0;
					while (temp->next != NULL)
					{
						temp = temp->next;
					}
					while (temp2->next->next != NULL)
					{
						for (int j=24;j<=28;j++)
						{
							if (direct_service[temp2->index][j] == false)
							{
								nominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time- shortest_time_matrix[29][temp->index]);
								denominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i];
							}
						}
						passed_time = passed_time + shortest_time_matrix[temp2->index][temp2->next->index] + 1.5;
						temp2 = temp2->next;
					}
					for (int j=24;j<=28;j++)
					{
						if (direct_service[temp2->index][j] == false)
						{					
							nominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time- shortest_time_matrix[29][temp->index]);
							denominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i];
						}
					}
					nominator_tran2[29][temp->index] += num_of_bus[i]/2/total_trip_time[i] * shortest_time_matrix[29][temp->index];
					denominator_tran2[29][temp->index] += num_of_bus[i]/2/total_trip_time[i];
				}

				new_Objective = 0;
				// Calculate new objective function value
				for (int i=1;i<=23;i++)
				{
					for (int j=24;j<=28;j++)
					{
						if (direct_service[i][j] == true)
						{
							average_travel_time = (nominator[i][j]+1.0)/denominator[i][j];
							new_Objective = new_Objective + B2*average_travel_time * demand[i][j];
						}
						if (direct_service[i][j] == false)
						{
							average_travel_time = (nominator_tran1[i][j]+1.0)/denominator_tran1[i][j] + (nominator_tran2[29][j]+1.0)/denominator_tran2[29][j];
							new_Objective = new_Objective + B1 *demand[i][j] + B2*average_travel_time *demand[i][j];
						}
					}
				}

				cout << endl <<   "New Objective function: " << new_Objective << endl;
				if (new_Objective < Objective)
				{
					cout << "Objective function improved! " << endl << "New frequency setting:" << endl;
					for (int c=1;c<=Route_max;c++)
					{
						cout << num_of_bus[c] << " ";
					}
					cout << endl;
					Objective = new_Objective;
				}
				else if (new_Objective > Objective)
				{
					cout << "Objective function not improved! " << endl << "New frequency setting:" << endl;
					num_of_bus[a]++;
					num_of_bus[b]--;
					flag_frequency_change = false;
					for (int c=1;c<=Route_max;c++)
					{
						cout << num_of_bus[c] << " ";
					}
					cout << endl;
				}

			}

			flag_frequency_change = true;
			
			// move 1 bus from route[b] to route[a]
			while (flag_frequency_change == true && num_of_bus[b]/2.0/total_trip_time[b]*60.0 >= 4.8)
			{
				num_of_bus[b]--;
				num_of_bus[a]++;
				for (int i=1;i<=23;i++)
				{
					for (int j=24;j<=29;j++)
					{
						nominator[i][j] = 0;
						nominator_tran1[i][j] = 0;
						denominator[i][j] = 0;
						denominator_tran1[i][j] = 0;
					}
				}
				for (int i=1;i<=29;i++)
				{
					for (int j=1;j<=29;j++)
					{
						denominator_tran2[i][j] = 0;
						nominator_tran2[i][j] = 0;
					}
				}

				// Calculate for those O-D pair with direct service
				for (int i=1;i<=Route_max;i++)
				{
					temp = route[i];
					temp2 = route[i];
					passed_time = 0;
					while (temp->next != NULL)
					{
						temp = temp->next;
					}
					while (temp2->next->next != NULL)
					{
						nominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time);
						denominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i];
						passed_time = passed_time + shortest_time_matrix[temp2->index][temp2->next->index] + 1.5;
						temp2 = temp2->next;
					}
					nominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time);
					denominator[temp2->index][temp->index] += num_of_bus[i]/2/total_trip_time[i];
				}

				// Calculate for those O-D pair with NO direct service
				for (int i=1;i<=Route_max;i++)
				{
					temp = route[i];
					temp2 = route[i];
					passed_time = 0;
					while (temp->next != NULL)
					{
						temp = temp->next;
					}
					while (temp2->next->next != NULL)
					{
						for (int j=24;j<=28;j++)
						{
							if (direct_service[temp2->index][j] == false)
							{
								nominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time- shortest_time_matrix[29][temp->index]);
								denominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i];
							}
						}
						passed_time = passed_time + shortest_time_matrix[temp2->index][temp2->next->index] + 1.5;
						temp2 = temp2->next;
					}
					for (int j=24;j<=28;j++)
					{
						if (direct_service[temp2->index][j] == false)
						{					
							nominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i]*(total_trip_time[i]-passed_time- shortest_time_matrix[29][temp->index]);
							denominator_tran1[temp2->index][j] += num_of_bus[i]/2/total_trip_time[i];
						}
					}
					nominator_tran2[29][temp->index] += num_of_bus[i]/2/total_trip_time[i] * shortest_time_matrix[29][temp->index];
					denominator_tran2[29][temp->index] += num_of_bus[i]/2/total_trip_time[i];
				}

				new_Objective = 0;
				// Calculate new objective function value
				for (int i=1;i<=23;i++)
				{
					for (int j=24;j<=28;j++)
					{
						if (direct_service[i][j] == true)
						{
							average_travel_time = (nominator[i][j]+1.0)/denominator[i][j];
							new_Objective = new_Objective + B2*average_travel_time *demand[i][j];
						}
						if (direct_service[i][j] == false)
						{
							average_travel_time = (nominator_tran1[i][j]+1.0)/denominator_tran1[i][j] + (nominator_tran2[29][j]+1.0)/denominator_tran2[29][j];
							new_Objective = new_Objective + B1 *demand[i][j] + B2*average_travel_time *demand[i][j];
						}
					}
				}

				cout << endl <<   "New Objective function: " << new_Objective << endl;
				if (new_Objective < Objective)
				{
					cout << "Objective function improved! " << endl << "New frequency setting:" << endl;
					for (int c=1;c<=Route_max;c++)
					{
						cout << num_of_bus[c]<< " ";
					}
					cout << endl;
					Objective = new_Objective;
				}
				else if (new_Objective > Objective)
				{
					cout << "Objective function not improved! " << endl << "New frequency setting:" << endl;
					num_of_bus[b]++;
					num_of_bus[a]--;
					flag_frequency_change = false;
					for (int c=1;c<=Route_max;c++)
					{
						cout << num_of_bus[c] << " ";
					}
					cout << endl;
				}

			}			

		}
	}
}

void Crossover_route(chromosome Parent1,chromosome Parent2, chromosome& Child1, chromosome& Child2)
{
	int rand1, rand2, temp;
	rand1 = rand()%10+1;
	rand2 = rand()%10+1;
	while (rand2 == rand1)
	{
		rand2 = rand()%10 +1;
	}
	if (rand1 > rand2)
	{
		temp = rand1;
		rand1 = rand2;
		rand2 = temp;
	}


	for (int i=1;i<=10;i++)
	{

		if (i>=rand1 && i<=rand2)
		{
			deepCopy(Parent2.route[i],Child1.route[i]);
			Child1.total_trip_time[i] = Parent2.total_trip_time[i];
			deepCopy(Parent1.route[i],Child2.route[i]);
			Child2.total_trip_time[i] = Parent1.total_trip_time[i];
		}
		else
		{
			deepCopy(Parent1.route[i],Child1.route[i]);
			Child1.total_trip_time[i] = Parent1.total_trip_time[i];
			deepCopy(Parent2.route[i],Child2.route[i]);
			Child2.total_trip_time[i] = Parent2.total_trip_time[i];
		}
	}
	for (int i=1;i<=10;i++)
	{
		if (i<=6)
		{
			Child1.num_of_bus[i] = 18;
			Child2.num_of_bus[i] = 18;
		}
		else
		{
			Child1.num_of_bus[i] = 17;
			Child2.num_of_bus[i] = 17;
		}
	}
}

void Crossover_stop(chromosome Parent1,chromosome Parent2, chromosome& Child1, chromosome& Child2, double shortest_time_matrix[][30])
{
	stop* temp;
	stop* temp2;
	int rand_num2,rand_num3;
	bool flag_same_dest = false;
	bool array1_10[11];
	int len1, len2; // only count no. of intermediate stops

	rand_num2 = rand() % 10 + 1;

	temp = Parent1.route[rand_num2];

	// Find the destination of a random route in parent 1
	while (temp->next != NULL)
	{
		temp = temp->next;
	}

	for (int i=1;i<=10;i++)
		array1_10[i] = false; // Random generation with no repeat

	// Randomly find a route in parent 2 which has the same destination
	while (flag_same_dest == false)
	{
		rand_num3 = rand() % 10 + 1;
		while (array1_10[rand_num3] == true)
		{
			rand_num3 = rand() % 10 + 1;
		}
		array1_10[rand_num3] = true;

		temp2 = Parent2.route[rand_num3];
		while (temp2->next != NULL)
		{
			temp2 = temp2->next;
		}
		if (temp->index == temp2 ->index)
			flag_same_dest = true;
	}

	len1 = length(Parent1.route[rand_num2])-2;
	len2 = length(Parent2.route[rand_num3])-2;

	// prevent length == 2
	while (len1 == 0 || len2 == 0)
	{
		flag_same_dest = false;
		rand_num2 = rand() % 10 + 1;

		temp = Parent1.route[rand_num2];

		// Find the destination of a random route in parent 1
		while (temp->next != NULL)
		{
			temp = temp->next;
		}

		for (int i=1;i<=10;i++)
			array1_10[i] = false; // Random generation with no repeat

		// Randomly find a route in parent 2 which has the same destination
		while (flag_same_dest == false)
		{
			rand_num3 = rand() % 10 + 1;
			while (array1_10[rand_num3] == true)
			{
				rand_num3 = rand() % 10 + 1;
			}
			array1_10[rand_num3] = true;

			temp2 = Parent2.route[rand_num3];
			while (temp2->next != NULL)
			{
				temp2 = temp2->next;
			}
			if (temp->index == temp2 ->index)
				flag_same_dest = true;
		}

		len1 = length(Parent1.route[rand_num2])-2;
		len2 = length(Parent2.route[rand_num3])-2;
	}

	// Randomly pick a starting node for exchange
	int rand_start1, rand_start2;
	int exchange_len1, exchange_len2;

	rand_start1 = rand()%len1 +1;
	rand_start2 = rand()%len2 +1;

	exchange_len1 = rand()% (len1-rand_start1+1)+1;
	exchange_len2 = rand()% (len2-rand_start2+1)+1;

	cout << endl << rand_num2 << " " << len1 << " test9 " << rand_start1 << " " << exchange_len1 << endl;
	cout << endl << rand_num3 << " " << len2 << " test8 " << rand_start2 << " " << exchange_len2 << endl;

	// Copy Parent1 to Child1 and Copy Parent2 to Child2
	for (int i=1;i<=10;i++)
	{
		deepCopy(Parent1.route[i],Child1.route[i]);
		Child1.total_trip_time[i] = Parent1.total_trip_time[i];
		deepCopy(Parent2.route[i],Child2.route[i]);
		Child2.total_trip_time[i] = Parent2.total_trip_time[i];
	}

	// Delete the routes that required to do stop crossover
	stop *temp3;
	while (Child1.route[rand_num2] != NULL)
	{
		temp3 = Child1.route[rand_num2];
		Child1.route[rand_num2] = Child1.route[rand_num2]->next;
		delete temp3;
	}
	Child1.route[rand_num2] = NULL;
	Child1.total_trip_time[rand_num2] = 0;
	while (Child2.route[rand_num3] != NULL)
	{
		temp3 = Child2.route[rand_num3];
		Child2.route[rand_num3] = Child2.route[rand_num3]->next;
		delete temp3;
	}
	Child2.route[rand_num3] = NULL;
	Child2.total_trip_time[rand_num3] = 0;

	// Do stop crossover
	stop* temp_Parent1_pos;
	stop* temp_Parent2_pos;
	temp_Parent1_pos = Parent1.route[rand_num2];
	temp_Parent2_pos = Parent2.route[rand_num3];
	int count_node = 1;
	deepCopy_start_from(temp_Parent1_pos,Child1.route[rand_num2],1,rand_start1,count_node);
	deepCopy_start_from(temp_Parent2_pos,Child2.route[rand_num3],1,rand_start2,count_node);
	for (int i=1;i<=rand_start1;i++)
	{
		temp_Parent1_pos = temp_Parent1_pos->next;
	}
	for (int i=1;i<=rand_start2;i++)
	{
		temp_Parent2_pos = temp_Parent2_pos->next;
	}
	deepCopy_start_from(temp_Parent2_pos,Child1.route[rand_num2],rand_start1+1,exchange_len2,count_node);
	deepCopy_start_from(temp_Parent1_pos,Child2.route[rand_num3],rand_start2+1,exchange_len1,count_node);
	for (int i=1;i<=exchange_len1;i++)
	{
		temp_Parent1_pos = temp_Parent1_pos->next;
	}
	for (int i=1;i<=exchange_len2;i++)
	{
		temp_Parent2_pos = temp_Parent2_pos->next;
	}
	deepCopy_start_from(temp_Parent1_pos,Child1.route[rand_num2],rand_start1+exchange_len2+1,len1+2-(rand_start1+exchange_len1),count_node);
	deepCopy_start_from(temp_Parent2_pos,Child2.route[rand_num3],rand_start2+exchange_len1+1,len2+2-(rand_start2+exchange_len2),count_node);
	
	// Delete repeated nodes
	int repeated_node = -1;
	check_repeated_node(Child1.route[rand_num2],repeated_node);
	while (repeated_node != -1)
	{
		del(Child1.route[rand_num2]->next,repeated_node);
		repeated_node = -1;
		check_repeated_node(Child1.route[rand_num2],repeated_node);
	}

	repeated_node = -1;
	check_repeated_node(Child2.route[rand_num3],repeated_node);
	while (repeated_node != -1)
	{
		del(Child2.route[rand_num3]->next,repeated_node);
		repeated_node = -1;
		check_repeated_node(Child2.route[rand_num3],repeated_node);
	}

	// Calculate total trip time for the route
	Calculate_Total_trip_time(Child1.total_trip_time[rand_num2], Child1.route[rand_num2], shortest_time_matrix);
	Calculate_Total_trip_time(Child2.total_trip_time[rand_num3], Child2.route[rand_num3], shortest_time_matrix);

	// Allocate initial num. of bus
	for (int i=1;i<=10;i++)
	{
		if (i<=6)
		{
			Child1.num_of_bus[i] = 18;
			Child2.num_of_bus[i] = 18;
		}
		else
		{
			Child1.num_of_bus[i] = 17;
			Child2.num_of_bus[i] = 17;
		}
	}
}

void deepCopy_start_from(stop* ptr, stop*& copy, int start_from, int numofnodes, int count_node)
{
	if (start_from == count_node)
	{
		int count_node2 = 0;
		deepCopy_numofnodes(ptr,copy,numofnodes,count_node2);
	}
	else deepCopy_start_from(ptr,copy->next,start_from,numofnodes,count_node + 1);
}

void deepCopy_numofnodes(stop* ptr,stop*& copy,int numofnodes, int count_node2)
{
	if (count_node2 < numofnodes)
	{
		copy = new stop;
		copy->index = ptr->index;
		copy->next = NULL;
		deepCopy_numofnodes(ptr->next, copy->next,numofnodes,count_node2+1);
	}
}

void check_repeated_node(stop* ptr, int& repeated_node)
{
	stop *temp;
	temp = ptr;
	int check[29];
	for (int i=1;i<=28;i++)
		check[i] = 0;

	while (temp != NULL)
	{
		check[temp->index]++;
		if (check[temp->index]>1)
			repeated_node = temp->index;
		temp = temp->next;
	}
}

void Mutation_insert(stop* route[])
{
	int rand_route;
	int rand_pos;
	int rand_stop;
	int len;
	rand_route = rand()%10 +1;
	bool check[29];

	// Check for used stops
	for (int i=1;i<=29;i++)
		check[i] = false;

	stop* temp;
	temp = route[rand_route];
	while (temp->next != NULL)
	{
		check[temp->index] = true;
		temp = temp->next;
	}

	// Randomly generate an unused stop
	rand_stop = rand()%23+1;
	while (check[rand_stop] == true)
	{
		rand_stop = rand()%23+1;
	}

	// Randomly select a inserting pos
	len = length(route[rand_route]);
	rand_pos = rand()% (len-1)+1;

	cout << endl << "node "<< rand_stop << " insert to route[" << rand_route << "] at pos" << rand_pos << endl;
	InsertAtPos(rand_pos,route[rand_route],rand_stop);
}

void Mutation_remove(stop* route[])
{
	int rand_route;
	int rand_pos;
	int len;
	int remove_node;

	// Randomly select a route
	rand_route = rand()%10 +1;

	// Randomly select a removing pos
	len = length(route[rand_route]);

	// Prevent len == 2
	while (len == 2)
	{
		rand_route = rand()%10 +1;
		len = length(route[rand_route]);
	}
	rand_pos = rand()% (len-2)+2;

	// Find the stop at the above pos.
	stop* temp;
	temp = route[rand_route];
	for (int i=1;i<rand_pos;i++)
	{
		temp = temp->next;
	}
	remove_node = temp->index;
	cout << endl << "route[" << rand_route << "] at pos " << rand_pos << " is removed" << endl;
	del(route[rand_route],remove_node);

}

void Mutation_swap(stop* route[])
{
	int rand_route1;
	int rand_route2;
	int rand_pos1;
	int rand_pos2;

	int len1, len2;
	// Randomly select two route
	rand_route1 = rand()%10 +1;
	rand_route2 = rand()%10 +1;
	while (rand_route2 == rand_route1)
	{
		rand_route2 = rand()%10+1;
	}

	
	len1 = length(route[rand_route1]);
	len2 = length(route[rand_route2]);

	// prevent len == 2
	while (len1 == 2 || len2 == 2)
	{
		rand_route1 = rand()%10 +1;
		rand_route2 = rand()%10 +1;
		while (rand_route2 == rand_route1)
		{
			rand_route2 = rand()%10+1;
		}
		len1 = length(route[rand_route1]);
		len2 = length(route[rand_route2]);
	}

	// Randomly select two pos
	rand_pos1 = rand() % len1 + 1;

	if (rand_pos1 == 1)
	{
		rand_pos2 = 1;
	}
	else if (rand_pos1 == len1)
	{
		rand_pos2 = len2;
	}
	else
	{
		rand_pos2 = rand()% (len2-2)+2;
	}

	int swap_node1, swap_node2;

	// Find the stop at the above pos.
	stop* temp1;
	temp1 = route[rand_route1];
	for (int i=1;i<rand_pos1;i++)
	{
		temp1 = temp1->next;
	}
	swap_node1 = temp1->index;

	stop* temp2;
	temp2 = route[rand_route2];
	for (int i=1;i<rand_pos2;i++)
	{
		temp2 = temp2->next;
	}
	swap_node2 = temp2->index;

	cout << endl << "route[" << rand_route1 << "] at pos " << rand_pos1 << " swaps with " << "route[" << rand_route2 << "] at pos " << rand_pos2 <<endl;

	// Do swapping
	temp1->index = swap_node2;
	temp2->index = swap_node1;

	// Check for repeated nodes 
	// Remove if repeated
	int repeated_node = -1;
	check_repeated_node(route[rand_route1],repeated_node);
	if (repeated_node != -1)
	{
		del(route[rand_route1]->next,repeated_node);
	}

	repeated_node = -1;
	check_repeated_node(route[rand_route2],repeated_node);
	if (repeated_node != -1)
	{
		del(route[rand_route2]->next,repeated_node);
	}
}

void Mutation_transfer(stop* route[])
{
	int rand_route1;
	int rand_route2;
	int rand_pos1;
	int rand_pos2;

	int len1, len2;
	// Randomly select two route
	rand_route1 = rand()%10 +1;
	rand_route2 = rand()%10 +1;
	while (rand_route2 == rand_route1)
	{
		rand_route2 = rand()%10+1;
	}

	len1 = length(route[rand_route1]);
	len2 = length(route[rand_route2]);

	// prevent len == 2
	while (len1 == 2 || len2 == 2)
	{
		rand_route1 = rand()%10 +1;
		rand_route2 = rand()%10 +1;
		while (rand_route2 == rand_route1)
		{
			rand_route2 = rand()%10+1;
		}

		len1 = length(route[rand_route1]);
		len2 = length(route[rand_route2]);
	}

	// Randomly select two pos
	rand_pos1 = rand() % (len1-2) + 2;
	rand_pos2 = rand() % (len2-2) + 2;

	int transfer_node1, transfer_node2;
	// Find the stop at the above pos.
	stop* temp1;
	temp1 = route[rand_route1];
	for (int i=1;i<rand_pos1;i++)
	{
		temp1 = temp1->next;
	}
	transfer_node1 = temp1->index;

	stop* temp2;
	temp2 = route[rand_route2];
	for (int i=1;i<rand_pos2;i++)
	{
		temp2 = temp2->next;
	}
	transfer_node2 = temp2->index;

	cout << endl << "node " << transfer_node1 << " from route[" << rand_route1 << "] is transfered to route[" << rand_route2 << "] after the node " << transfer_node2 << endl;
	InsertAtPos(rand_pos2, route[rand_route2], transfer_node1);
	del(route[rand_route1], transfer_node1);

	// Check for repeated nodes 
	// Remove if repeated
	int repeated_node = -1;
	check_repeated_node(route[rand_route2],repeated_node);
	if (repeated_node != -1)
	{
		del(route[rand_route2]->next,repeated_node);
	}
}

void stop_sequence_improvement(chromosome& Child,double shortest_time_matrix[][30], const int Route_max)
{
	int len;
	int temp_stop;
	stop* temp;
	stop* temp2;
	bool improved;
	double new_total_trip_time;

	for (int r=1;r<=Route_max;r++)
	{
		Calculate_Total_trip_time(Child.total_trip_time[r],Child.route[r],shortest_time_matrix);
		len = length(Child.route[r]);
		improved = false;

		cout << endl << "Route [" << r << "]: " << endl;
		for (int i=2;i<=len-2;i++)
		{
			for (int j=i+1;j<=len-1;j++)
			{
				temp = Child.route[r];
				temp2 = Child.route[r];
				// temp points to stop no. j
				for (int k=1;k<i;k++)
				{
					temp = temp->next;
				}
				for (int k=1;k<j;k++)
				{
					temp2 = temp2->next;
				}

				//	exchange stop no. j and no. j+1
				temp_stop = temp->index;
				temp->index = temp2->index;
				temp2->index = temp_stop;
				Calculate_Total_trip_time(new_total_trip_time,Child.route[r],shortest_time_matrix);

				cout << endl << "i:" << i << ", j: " << j << " _ "  << new_total_trip_time << endl;
				if (new_total_trip_time < Child.total_trip_time[r]) 
				{
					// New is better
					cout << endl << "Route [" << r << "] exchange node no." << i << " and " << j << endl;
					Child.total_trip_time[r] = new_total_trip_time;
					improved = true;
				}
				else
				{
					temp_stop = temp->index;
					temp->index = temp2->index;
					temp2->index = temp_stop;
				}
			}
		}
		while (improved == true)
		{
			Calculate_Total_trip_time(Child.total_trip_time[r],Child.route[r],shortest_time_matrix);
			len = length(Child.route[r]);
			improved = false;

			cout << endl << "Route [" << r << "]: " << endl;
			for (int i=2;i<=len-2;i++)
			{
				for (int j=i+1;j<=len-1;j++)
				{
					temp = Child.route[r];
					temp2 = Child.route[r];
					// temp points to stop no. j
					for (int k=1;k<i;k++)
					{
						temp = temp->next;
					}
					for (int k=1;k<j;k++)
					{
						temp2 = temp2->next;
					}

					//	exchange stop no. j and no. j+1
					temp_stop = temp->index;
					temp->index = temp2->index;
					temp2->index = temp_stop;
					Calculate_Total_trip_time(new_total_trip_time,Child.route[r],shortest_time_matrix);

					cout << endl << "i:" << i << ", j: " << j << " _ "  << new_total_trip_time << endl;
					if (new_total_trip_time < Child.total_trip_time[r]) 
					{
						// New is better
						cout << endl << "Route [" << r << "] exchange node no." << i << " and " << j << endl;
						Child.total_trip_time[r] = new_total_trip_time;
						improved = true;
					}
					else
					{
						temp_stop = temp->index;
						temp->index = temp2->index;
						temp2->index = temp_stop;
					}
				}
			}
		}
	}
}

void check_repair(chromosome& Child,double shortest_time_matrix[][30], const int Route_max)
{
	int num_of_inter_stop;
	double time_on_TSW;
	for (int r=1;r<=Route_max;r++)
	{
		num_of_inter_stop = length(Child.route[r])-2;
		time_on_TSW = Calculate_Total_Route_Time_on_TSW(num_of_inter_stop,Child.route[r],shortest_time_matrix);
		while (num_of_inter_stop > 8 || time_on_TSW > 35)
		{
			cout << endl << "route [" << r << "]_ time on TSW: " << time_on_TSW << endl;
			repair_operator(Child.route[r],shortest_time_matrix);
			num_of_inter_stop = length(Child.route[r])-2;
			time_on_TSW = Calculate_Total_Route_Time_on_TSW(num_of_inter_stop,Child.route[r],shortest_time_matrix);
		}
	}
}

void repair_operator(stop*& ptr,double shortest_time_matrix[][30])
{
	stop* temp;
	stop* temp2;
	temp = NULL;
	int len;
	int del_save;
	int del_stop;
	double total_trip_time_1 = 9999;
	double new_total_trip_time_1;
	len = length(ptr);
	for (int i=2;i<=len-1;i++)
	{
		// Clear temp
		while (temp != NULL)
		{
			temp2 = temp;
			temp = temp->next;
			delete temp2;
		}
		temp = NULL;

		// Copy ptr to temp
		deepCopy(ptr, temp);

		// Delete temp's stop no. i
		temp2 = temp;
		for (int j=1;j<i;j++)
		{
			temp2 = temp2->next;
		}
		del_stop = temp2->index;
		del(temp,del_stop);

		// Check if the new total trip time is reduced
		Calculate_Total_trip_time(new_total_trip_time_1,temp,shortest_time_matrix);
		if (new_total_trip_time_1 < total_trip_time_1)
		{
			total_trip_time_1 = new_total_trip_time_1;
			del_save = del_stop;
		}
	}

	cout << endl << "Delete node " << del_save << " gives the max. reduction of time" << endl;
	del(ptr, del_save);

}

void repair_missing(chromosome& Child,double shortest_time_matrix[][30], const int Route_max, const double Time_max)
{
	// count no. of visit
	int check[30];
	for (int i=1;i<=29;i++)
		check[i] = 0;

	stop* temp;
	for (int r=1;r<=Route_max;r++)
	{
		temp = Child.route[r];
		while (temp != NULL)
		{
			check[temp->index]++;
			temp = temp->next;
		}
	}

	int set_of_check0[24]; // For repairing
	int set_of_check2[24];	// For repairing
	int index_for_check0; // For repairing
	int index_for_check2; // For repairing
	double shortest_travel_time_for_repairing; // For repairing
	int closest_node; // For repairing
	bool flag_replace; // For repairing

	for (int j=1;j<= 23;j++)
	{
		set_of_check0[j] = 0;
		set_of_check2[j] = 0; 
	}
	index_for_check0 = 1;
	index_for_check2 = 1;

	for (int j=1;j<=23;j++)
	{
		if (check[j] == 0)
		{
			set_of_check0[index_for_check0] = j;
			index_for_check0++;
		}
		if (check[j] > 1)
		{
			set_of_check2[index_for_check2] = j;
			index_for_check2++;
		}
	}

	// Debug
	cout << endl << "Missing: " << endl;
	for (int j=1;j<index_for_check0;j++)
	{
		cout << set_of_check0[j] << " ";
	}
	cout << endl;
	for (int j=1;j<index_for_check2;j++)
	{
		cout << set_of_check2[j] << " ";
	}
	cout << endl;
	//Debug

	// Only repair if num of missed nodes <= 1
	if (index_for_check0-1 <= 1)
	{
		for (int j=1;j<index_for_check0;j++)
		{
			shortest_travel_time_for_repairing = 9999;
			closest_node = 99;
			for (int k=1;k<index_for_check2;k++)
			{
				if (shortest_time_matrix[set_of_check0[j]][set_of_check2[k]] < shortest_travel_time_for_repairing)
				{
					shortest_travel_time_for_repairing = shortest_time_matrix[set_of_check0[j]][set_of_check2[k]];
					closest_node = k;
				}
			}
			flag_replace = false;
			cout << "Closest node: " << set_of_check2[closest_node] << endl;
			for (int a=1;a<=Route_max;a++)
			{
				replace_node(Child.route[a]->next,Child.route[a],set_of_check2[closest_node],set_of_check0[j],flag_replace,shortest_time_matrix,Time_max);
			}
			if (flag_replace == true)
			{
				check[set_of_check2[closest_node]]--;
				check[set_of_check0[j]]++;
			}
			
		}
	}
	if (index_for_check0-1 > 1 || (flag_replace == false && index_for_check0-1 == 1))
	{
		cout << endl << "Repair fails, destroy this child" << endl;
		Child.route[1]->index = -1;
	}

	// Repair destination (route crossover can miss some destination)
	int set_of_dest_check0[29];
	int set_of_dest_check2[29];
	int index_for_dest_check0;
	int index_for_dest_check2;

	for (int j=24;j<= 28;j++)
	{
		set_of_dest_check0[j] = 0;
		set_of_dest_check2[j] = 0; 
	}
	index_for_dest_check0 = 1;
	index_for_dest_check2 = 1;

	for (int j=24;j<=28;j++)
	{
		if (check[j] == 0)
		{
			set_of_dest_check0[index_for_dest_check0] = j;
			index_for_dest_check0++;
		}
		if (check[j] > 1)
		{
			set_of_dest_check2[index_for_dest_check2] = j;
			index_for_dest_check2++;
		}
	}

	// Debug
	for (int j=1;j<index_for_dest_check0;j++)
	{
		cout << endl << "Missing destination: " << endl;
		cout << set_of_dest_check0[j] << " ";
	}
	cout << endl;
	for (int j=1;j<index_for_dest_check2;j++)
	{
		cout << set_of_dest_check2[j] << " ";
	}
	cout << endl;
	//Debug

	if (index_for_dest_check0-1 > 1)
	{
		cout << endl << "Missing too much destination, destroy this child" << endl;
		Child.route[1]->index = -1;		
	}

	int num_of_destination_stop = 0;
	int replace_destination;
	bool flag_destination_replace = false;
	stop* temp9;

	// Repair if missing destination == 1
	if (index_for_dest_check0-1 == 1)
	{
		for (int j=1;j<=index_for_dest_check2-1;j++)
		{
			if (check[set_of_dest_check2[j]] > num_of_destination_stop)
			{
				num_of_destination_stop = check[set_of_dest_check2[j]];
				replace_destination = set_of_dest_check2[j];
			}
		}

		for (int r=1;r<=10 && flag_destination_replace == false;r++)
		{
			temp9 = Child.route[r];
			while (temp9->next != NULL)
			{
				temp9 = temp9 -> next;
			}
			if (temp9 -> index == replace_destination)
			{
				temp9 -> index = set_of_dest_check0[1];
				flag_destination_replace = true;
				cout << endl << "destination* " << replace_destination << " is replaced by " << set_of_dest_check0[1] << endl;
			}
		}
	}
}

void del_gene(chromosome& parent, const int Route_max)
{
	delist(parent.route,Route_max);
	for (int i=1;i<=Route_max;i++)
	{
		parent.num_of_bus[i] = 0;
		parent.total_trip_time[i] = 0;
		parent.Objective = 999999999;
	}
}

void deepCopy_gene(chromosome parent, chromosome& copy, const int Route_max)
{
	for (int i=1;i<=Route_max;i++)
	{
		deepCopy(parent.route[i],copy.route[i]);
		copy.num_of_bus[i] = parent.num_of_bus[i];
		copy.total_trip_time[i] = parent.total_trip_time[i];
	}
	copy.Objective = parent.Objective;

	for (int i=1;i<=23;i++)
	{
		for (int j=24;j<=28;j++)
		{
			copy.direct_service[i][j] = parent.direct_service[i][j];
		}
	}

}

void Diversity_control(chromosome gene[], const int Route_max)
{
	int rank[41];
	bool check_rank[41];
	for (int i=1; i<=40;i++)
		check_rank[i] = false;
	for (int i=1; i<=40;i++)
		rank[i] = 0;

	double lowest_objective = 0;
	int lowest_save;
	// selection sort according to fitness value (1/objective) (From low objective to high objective)
	for (int i=1; i<=40; i++)
	{
		lowest_objective = 2000000000;
		for (int j=1;j<=40;j++)
		{
			if (check_rank[j] == false)
			{
				if (gene[j].Objective <= lowest_objective)
				{
					lowest_objective = gene[j].Objective;
					lowest_save = j;
				}
			}
		}
		rank[i] = lowest_save;
		check_rank[lowest_save] = true;
	}

	// debug
	cout << endl << "Check ranking" << endl;
	for (int i=1; i<=40;i++)
		cout << rank[i] << " ";
	cout << endl;
	// debug

	// Copy gene to control according to rank
	chromosome control[41];
	for (int i=1;i<=40;i++)
	{
		for (int r=1;r<= Route_max;r++)
		{
			control[i].route[r] = NULL;
			control[i].num_of_bus[r] = 0;
			control[i].total_trip_time[r] = 0;
		}
		control[i].Objective = 999999999;
	}

	for (int i=1;i<=40;i++)
	{
		deepCopy_gene(gene[rank[i]],control[i],Route_max);
	}

	// Display 40 controls (Parents+ offspring)
	for (int i=1;i<=40;i++)
	{
		cout << endl << "control[" << i << "] 10 routes:" << endl; 
		displayallroute(control[i].route,Route_max);
		cout << "Num of bus: ";
		for (int j=1;j<=Route_max;j++)
			cout << control[i].num_of_bus[j] << " ";
		cout << endl << "Total trip time: ";
		for (int j=1;j<=Route_max;j++)
			cout << control[i].total_trip_time[j] << " ";
		cout << endl << "Objective: " << control[i].Objective << endl;
	}

	// Delete all genes
	for (int i=1;i<=40;i++)
	{
		del_gene(gene[i],Route_max);
	}
	// Copy the best individual to gene[1]
	deepCopy_gene(control[1],gene[1],Route_max);

	// calculate hamming distance and prob. of survival
	const double a = 0.002;
	const double c = 0.08;
	double h;
	int node_pair[11][10][3]; // [route index][node_pair index][the two nodes]
	int num_of_node_pair[11]; // length(route)-1;
	bool node_pair_repeat[10]; // Save if the node pair is repeated
	int new_node_pair[3];

	double L1;

	// Save the node_pair of the best individual and calculate L for the best individual
	stop* temp;
	L1 = 0;
	for (int r=1;r<=Route_max;r++)
	{
		num_of_node_pair[r] = length(control[1].route[r])-1;
		temp = control[1].route[r];

		for (int i=1;i<= num_of_node_pair[r];i++)
		{
			L1++;
			node_pair[r][i][1] = temp->index;
			node_pair[r][i][2] = temp->next->index;
			temp = temp->next;
		}
	}

	// debug start
	cout << endl << "L1: " << L1 << endl;
	for (int r=1;r<=Route_max;r++)
	{
		cout << endl;
		for (int i=1;i<= num_of_node_pair[r];i++)
		{
			cout << node_pair[r][i][1] << " " << node_pair[r][i][2] << ", ";
		}
		cout << endl;
	}
	// debug end

	int index_for_gene = 1;
	double L2;
	double prob_survive;

	for (int i=2;i<=40;i++)
	{
		if (control[i].route[1]->index != -1)
		{
			h = L1;
			L2 = 0;
			for (int r=1;r<=Route_max;r++)
			{
				// Initialize node_pair_repeat
 				for (int a=1;a<=9;a++)
 					node_pair_repeat[a] = false;
 				temp = control[i].route[r];

 				// Calculate hamming distance for each route
 				for (int k=1;k<= length(control[i].route[r])-1;k++)
				{
					L2++;
					h++;
					new_node_pair[1] = temp->index;
					new_node_pair[2] = temp->next->index;

					// Check if the node pair is repeated
					for (int j=1; j<=num_of_node_pair[r];j++)
					{
						if (new_node_pair[1] == node_pair[r][j][1] && new_node_pair[2] == node_pair[r][j][2])
						{
							node_pair_repeat[j] = true;
							h = h-2;
						}
					}
					temp = temp->next;
				}
			}
			prob_survive = pow(((1-c)*h/(L1+L2)+c), a);
			cout << endl << "control [" << i << "], L2: " << L2 << ", h: " << h << " , prob: "<< prob_survive << endl;
		}
	}

}



