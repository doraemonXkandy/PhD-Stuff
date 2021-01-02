#include <iostream>
#include <stdlib.h>     /* srand, rand */
#include <time.h>


using namespace std;

int main()
{
  srand (time(NULL));
  cout << "First number: " << rand()%100 << endl; 
  cout << "Second number: " << rand()%100<< endl;
  cout << "Third number: " << rand()%100<< endl;


	return 0;
}