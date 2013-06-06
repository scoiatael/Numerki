#include <cmath>
#include <iostream>
using namespace std;

int main()
{
	cout << "i: " << "    " << "f(i): " << endl;
	for (int i=0; i< 100; i++)
		cout << i << "    " << sin(i) << endl;
}