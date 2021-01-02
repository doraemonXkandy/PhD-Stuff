#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>	 /* time */

using namespace std;

int main()
{
 
	clock_t start = clock();

  for (int i=1;i<10000;i++)
    cout << i;
  
  clock_t end = clock();
  double elapsed = double(end - start)/CLOCKS_PER_SEC;
  cout << "Time measured: " << elapsed << "seconds.";

	return 0;
}