#include <iostream>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include <cstdlib>
using namespace std;

const double EPS = 0.0000000000001L;
const int BIG_NUM = 10000000;
const double MAX = 20;

int sgn(const double& num)
{
	if(num<0)
		return -1;
	if(num == 0)
		return 0;
	return 1;
}

bool low_enough(const double& num)
{
	return num < EPS;
}

double my_exp(double x)
{
	int sgnx = sgn(x);
	x *= sgnx;
	double acc = 0, next_iter = 1;
	int n;
	for (n = 0; (! low_enough(next_iter)) && n < BIG_NUM; n++,  next_iter*= x/n)
		acc += next_iter;
	cout << log10(n) << " "; 
	return pow(acc, sgnx);
}

double better_exp2(double x)
{
	int sgnx = sgn(x);
	x *= sgnx;
	int end_mark = floor(x);
	double acc = 1, n;
	while (end_mark > 0)
	{
		if (end_mark % 2 == 0)
		{
			acc *= acc;
			end_mark /= 2;
		}
		else
		{
			acc *= M_E;
			end_mark -= 1;
		}
			
	}
	return pow(acc * my_exp(x - (double)floor(x)),sgnx);
}

double better_exp1(double x)
{
	
	if(abs(x) >= M_LN2/2)
	{
		double z = x/M_LN2, m = floor(z + sgn(floor(x))/2), w = z - m, u = w * M_LN2; 	
		return pow(2,m) * my_exp(u);
	}
	return my_exp(x);
}

void print_w_format(const double* table, const int& n)
{
	for(int i=0; i< n; i++)
		cout << table[i] << " ";
	cout << endl;
}

double diff(const double& num1, const double& num_prec)
{
	double wynik =  (abs(log10(abs((num1-num_prec)/num_prec))));
	if (wynik == __builtin_inf())
		return MAX;
	return wynik;
}

void print_exp(const double& num)
{
	cout << num << " ";
	double val = my_exp(num), true_val = exp(num), better_val1 = better_exp1(num);
	double table[2] = {diff(val,true_val), diff( better_val1, true_val)};
	print_w_format(table,2); 
	
}

int main()
{
	cout.precision(20);
	char op;
	cin >> op;
	switch(op)
	{
		case('b'):
		{
			for(double i = -600; i <= 600; i+=10)
				print_exp(i);
			break;
		}
		case('m'):
		{	
			for(double i = -10; i <= 10; i+=0.25)
				print_exp(i);
			break;
		}
		case('s'):
		{
			for(double i = -2; i <= 2; i+=0.05)
				print_exp(i);
			break;
		}
		case('e'):
		{	
			ofstream myfile, myfile2;
			myfile.open("datar.dat");
			myfile2.open("datan.dat");
			timeval start,end;
			double e, delta;
			for(int i = -600; i < 600; i+=15)
			{
				double arg = (double) i + ((double) (rand()%1000))/100;
				myfile << arg << " & ";
				myfile2 << arg << " ";
				gettimeofday(&start, NULL);
				for(int i=0; i<1000; i++) e = my_exp(arg);
				gettimeofday(&end, NULL);
				delta = 1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);
				myfile << delta << " & ";
				myfile2 << delta << " ";
				gettimeofday(&start, NULL);
				for(int i=0; i<1000; i++) e = better_exp1(arg);
				gettimeofday(&end, NULL);
				delta = 1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);
				myfile << delta << " & ";
				myfile2 << delta << " ";
				gettimeofday(&start, NULL);
				for(int i=0; i<1000; i++) e = exp(arg);
				gettimeofday(&end, NULL);
				delta = 1000000*(end.tv_sec-start.tv_sec)+(end.tv_usec-start.tv_usec);
				myfile << delta << " \\\\\n";
				myfile2 << delta << "\n";
		}
			break;
		}
	}

	return 0;
}
