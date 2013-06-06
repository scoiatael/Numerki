#include <iostream>
#include <cmath>
using namespace std;


double ci(const int& n)
{
	if(n==2)
		return 0;
	else
		return sqrt((1+ci(n-1))/2);
}

double si(const int& n)
{
	if(n==2)
		return 1.0;
	else
		return sqrt((1-ci(n-1))/2);
}

double P(const int& n)
{
	return pow(2, (n-1)) * si(n);
}


double si2(const int& n)
{
	if(n==2)
		return 1.0;
	else
		return si2(n-1) / sqrt(2+2*ci(n-1));
}


double P2(const int& n)
{
	return pow(2, (n-1)) * si2(n);
}

int main()
{
	cout.precision(10);
	for(int i=2; i<100; i++)
	{
		double pi = P(i), pi2 = P2(i);
		cout << i << ": " << pi << " " << pi2 << " " << floor(abs(log10(abs(pi2 - M_PI)))) << endl;
	}
	return 0; 
}
