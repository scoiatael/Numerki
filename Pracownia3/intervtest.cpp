#include <vector>
#include <cstdlib>
#include <string>
#include <algorithm>
#include <iostream>
#include <sys/time.h>
using namespace std;

inline int gettime()
{
  timeval timer;
  gettimeofday(&timer, NULL);
  return 1000000*(timer.tv_sec)+(timer.tv_usec);
}

void create_monosp_interval(const double& a, const double&b, const double& n, vector<double>& arg)
{
  arg.clear();
  for(int i=0; i<=n; i++)
    arg.push_back(a+i/n*(b-a));
}

void distort_interval(vector<double>& arg)
{
  double a=arg[0],
         b=arg[arg.size()-1],
         n=arg.size();
  for(int i=1; i<n-1; i++)
    arg[i]+=0.001*(double)(rand()%(int)((b-a)/n*1000))*(double)(rand()%3-1);
  sort(arg.begin(), arg.end());
}

void printvect(const vector<double>& arg)
{
  for(int i=0; i<arg.size(); i++)
    cout << arg[i] << " ";
  cout << endl;
}

int main(int argc, char** argv)
{
  srand(gettime());
  double a = atof(argv[1]),
         b = atof(argv[2]),
         n = atof(argv[3]);
  vector<double> arg;
  create_monosp_interval(a,b,n,arg);
  printvect(arg);
  distort_interval(arg);
  printvect(arg);
}
