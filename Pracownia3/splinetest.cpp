#include <iostream>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include "fhb.cpp"
using namespace std;

void spline_params(const fhb::fun_wrap& func, const vector<double>& points, vector<pair<double, double> >& params)
{
  int n(points.size()-1);
  vector<double> d(n+1), delta(n+1),h(n+1);
  for(int i=1; i<points.size();i++)
  {
//    cout << "p1: " << points[i-1] << " p2: " << points[i] << endl; 
    h[i] = points[i] - points[i-1];
 //   cout << "h: " << h[i-1] << endl;
  }
  for(int i=1; i<h.size()-1; i++)
  {
//    cout << "h1: " << h[i] << " h2: " << h[i+1] << endl;
    delta[i] = (h[i]/(h[i]+h[i+1]));
//    cout << "delta: " << delta[i] << endl;
  }
  vector<double> daux(n+1);
  for(int i=1;i<=n; i++)
    daux[i] = ((func.apply(points[i])-func.apply(points[i-1]))/(h[i]));
  for(int i=1;i<daux.size();i++)
    d[i] = ((daux[i+1]-daux[i])/(points[i+1]-points[i-1]));
  vector<double> q(n+1),p(n+1),u(n+1);
  q[0] = 0;
  u[0] = 0;
  for(int i=1; i<n;i++)
  {
//    cout << "delta: " << delta[i] << " q1: " << q[i-1] << endl;
    p[i] = delta[i]*q[i-1]+2;
//    cout << "p: " << p[i] << endl;
    q[i] = (delta[i]-1)/p[i];
//    cout << "d: " << d[i]<< endl; 
    u[i] = (d[i]-delta[i]*u[i-1])/p[i];
//    cout << "q: " << q[i] << " u: " << u[i] << endl << "--\n";
  }
  params.resize(n+1);
  params[0]   = pair<double, double>(points[0],   0);
  params[n]   = pair<double, double>(points[n],   0);
  params[n-1] = pair<double, double>(points[n-1], u[n-1]);

  for(int i=n-2; i>0; i--)
    params[i]=pair<double, double>(points[i], u[i]+q[i]*params[i+1].second);
}

double spline_at(const fhb::fun_wrap& f , const pair<double, double>& O, const pair<double, double>& P, const double& x)
{
  double 
    a = O.first,
    b = P.first,
    d = b - a,
    M = O.second,
    N = P.second;
 // cout << "a: " << a << " b: " << b << " d: " << d << " M: " << M << " N: " << N << endl;
  return (
          M*pow((a-x),2)/6
          +N*pow((x-b),2)/6
          +(f.apply(a)-M*pow(d,2)/6)*(b-x)
          +(f.apply(b)-N*pow(d,2)/6)*(x-a)
          )/(d);
}


void create_monosp_interval(const double& a, const double&b, const double& n, vector<double>& arg)
{
  arg.clear();
  for(int i=0; i<=n; i++)
    arg.push_back(a+i/n*(b-a));
}

inline int gettime()
{
  timeval timer;
  gettimeofday(&timer, NULL);
  return 1000000*(timer.tv_sec)+(timer.tv_usec);
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

void do_computations(const string& name, const string& testname0)
{
  srand(gettime());
  fstream load_f;
  string fname = string("./testy/");
  fname += testname0;
  fname += ".din";
  cout << "Loading from " << fname << "..." << endl;
  load_f.open(fname.c_str(), ios_base::in);
  string s,ds;
  load_f >> s;
  fhb::fun_wrap function(s);
  load_f >> ds;
  fhb::fun_wrap dfunction(ds);
 // function.wypisz();
 // dfunction.wypisz();
  double a, b;
  load_f >> a >> b;
  if(a>=b)
  {
    cerr << "Failed. Wrong domain given.\n";
    exit(255);
  }
  double x;
  load_f >> x;
  if(load_f.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  load_f.close();
  /** Obliczenia*/
  vector<pair<double,double> > spline_mparams, spline_dparams;
  vector<double> points;
  int n = 100;
  create_monosp_interval(a,b,n,points);
  spline_params(function, points, spline_mparams);
  distort_interval(points);
  spline_params(function, points, spline_dparams);
  /* */
  fstream do_zapisu;
  fname = string("./wyniki/");
  fname += testname0;
  fname += string("/");
  fname += name;
  fname += ".db";
  cout << "Writing to " << fname << "..." << endl;
  do_zapisu.open(fname.c_str(), ios_base::out);
  /** Zapis wynikow1*/
  for(int i=0, k=1, l=1; i <= 4*n; i++)
  {
    double p = a + i*(b-a)/(4*n);
    if(p > spline_mparams[k].first)
      k++;
    if(p > spline_dparams[l].first)
      l++;
    do_zapisu << p << " " << spline_at(function, spline_mparams[k-1], spline_mparams[k], p) 
      << " " << spline_at(function, spline_dparams[l-1], spline_dparams[l], p) 
        << " " << function.apply(p) <<  endl;
  }
  /* */
  if(do_zapisu.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  do_zapisu.close();
}

int main()
{
  string fname;
  cin >> fname;
  do_computations("Approximations", fname);
	return 0;
}
