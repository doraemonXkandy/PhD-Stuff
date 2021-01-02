#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>	 /* time */

using namespace std;

int main()
{
 
	clock_t start = clock();

	srand (time(NULL));
	
	cout << "First number: " << rand()%100 << endl;
  	
  	cout << "Random number: " << rand()%100<< endl;
 
  	cout << "Again the first number: " << rand()%100<< endl;
  	for (int i=1;i<5000;i++)
  		cout << i;
  	srand (time(NULL));
  	cout << "First number: " << rand()%100 << endl;
  	
  	cout << "Random number: " << rand()%100<< endl;
 
  	cout << "Again the first number: " << rand()%100<< endl;

  	 clock_t end = clock();
    double elapsed = double(end - start)/CLOCKS_PER_SEC;
    
   
    
    cout << "Time measured: " << elapsed << "seconds.";
	return 0;
}