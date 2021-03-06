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
void InsertAtPos(int pos, stop*& ptr, int randx);
double Calculate_Total_Route_Time_on_TSW(int num_of_inter_stop, stop* ptr, double shortest_time_matrix[][30]);
void del(stop*& ptr, int key);
void replace_node(stop*& ptr, stop* ptr2, int key,int new_node, bool& flag_replace, double shortest_time_matrix[][30], const double Time_max); // Change key to new_node // Do repairing for 1 node only
void Calculate_Total_trip_time(double& total_trip_time, stop* ptr, double shortest_time_matrix[][30]);
void Update_frequency_setting(stop* route[], double num_of_bus[],double total_trip_time[],double& Objective,bool direct_service[][30],int demand[][30],double shortest_time_matrix[][30],const int Route_max); // Update for 1 gene

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
	chromosome gene[50];
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
				Objective = Objective + B2*average_travel_time;
			}
			if (direct_service[i][j] == false)
			{
				average_travel_time = (nominator_tran1[i][j]+1.0)/denominator_tran1[i][j] + (nominator_tran2[29][j]+1.0)/denominator_tran2[29][j];
				Objective = Objective + B1 + B2*average_travel_time;
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
							new_Objective = new_Objective + B2*average_travel_time;
						}
						if (direct_service[i][j] == false)
						{
							average_travel_time = (nominator_tran1[i][j]+1.0)/denominator_tran1[i][j] + (nominator_tran2[29][j]+1.0)/denominator_tran2[29][j];
							new_Objective = new_Objective + B1 + B2*average_travel_time;
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
							new_Objective = new_Objective + B2*average_travel_time;
						}
						if (direct_service[i][j] == false)
						{
							average_travel_time = (nominator_tran1[i][j]+1.0)/denominator_tran1[i][j] + (nominator_tran2[29][j]+1.0)/denominator_tran2[29][j];
							new_Objective = new_Objective + B1 + B2*average_travel_time;
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

