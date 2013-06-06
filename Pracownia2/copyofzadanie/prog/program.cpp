#include <iostream>
#include <fstream>
#include <cmath>
#include <sys/time.h>
#include <cstdlib>
#include "fhb.cpp"
#include <algorithm>
#include <vector>
using namespace std;

bool low_enough(const double& delta)
{
  const double EPSILON = pow(0.1, 12);
  return abs(delta) <= EPSILON;
}

inline void iterate(double (&func)(const fhb::fun_wrap&, vector<double>&), const fhb::fun_wrap& func_inter, vector<double>& points)
{
  const unsigned int MAX = pow(10, 2);
  unsigned int i=0;
  do
  {
    points.push_back(func(func_inter, points));
    i++;
  }
  while(!(low_enough(points.back() - points[points.size()-2]) 
      && low_enough(func_inter.apply(points.back()))) 
        && i<MAX) ;
}

double odwr_interpol_kw(const fhb::fun_wrap& func, vector<double>& points)
{
  double x[3],y[3];
  for(int i=1; i<4;i++)
  {
    x[i-1]=points[points.size()-i];
    y[i-1]=func.apply(x[i-1]);
  }
  return x[2]*y[1]*y[0]/((y[2]-y[1])*(y[2]-y[0])) + x[1]*y[2]*y[0]/((y[1]-y[2])*(y[1]-y[0])) + x[0]*y[2]*y[1]/((y[0]-y[2])*(y[0]-y[1]));
}

void odwr_interpol_kw_wrap(const fhb::fun_wrap& func, vector<double>& start_points, fhb::fun_wrap& dfunc)
{
  if(start_points.size() < 3)
  {
    cerr << "Blad: Za malo punktow startowych, aby zastosowac odwrotna interpolacje kwadratowa.\n";
    exit(255);
  }
  iterate(odwr_interpol_kw, func, start_points);
}

fhb::fun_wrap* dfuncpointer;
double met_sty(const fhb::fun_wrap& func, vector<double>& points)
{
  double x = points.back();
  return x - (func.apply(x)/dfuncpointer->apply(x));
}

void met_sty_wrap(const fhb::fun_wrap& func, vector<double>& start_points, fhb::fun_wrap& dfunc)
{
  if(start_points.size() < 1)
  {
    cerr << "Blad: Za malo punktow startowych, aby zastosowac metode Newtona.\n";
    exit(255);
  }
  dfuncpointer = &dfunc;
  iterate(met_sty, func, start_points);
}

double met_siecz(const fhb::fun_wrap& func, vector<double>& points)
{
  double x[2], y[2];
  for(int i=1; i<3;i++)
  {
   x[i-1]=points[points.size()-i];
   y[i-1]=func.apply(x[i-1]);
  }
  return x[0] - (y[0]*(x[0]-x[1]))/(y[0]-y[1]) ;  
}

void met_siecz_wrap(const fhb::fun_wrap& func, vector<double>& start_points, fhb::fun_wrap& dfunc)
{
  if(start_points.size() < 2)
  {
    cerr << "Blad: Za malo punktow startowych, aby zastosowac metode siecznych.\n";
    exit(255);
  }
  iterate(met_siecz, func, start_points);
}

inline double rate_of_conv_aux(const double& arg1, const double& arg2, const double& arg3)
{
  if(arg1!=0 && arg2!=0 && arg3!=0)
  {
    double aux = abs(arg2/arg3);
    if(aux!=1)
      return (log(abs(arg1/arg2))/log(aux));
  }
  return 2;
}

inline double max(const double& arg1, const double& arg2)
{
  if(arg1<arg2)
    return arg2;
  return arg1;
}

vector<double> rate_of_conv(const vector<double>& points, const double& l)
{
  vector<double> temp;
  for(int i=max(2,points.size()-4); i<points.size();i++)
  {
    temp.push_back(rate_of_conv_aux(points[i]-l, points[i-1]-l, points[i-2]-l));
  }
  return temp;
}

inline int gettime()
{
  timeval timer;
  gettimeofday(&timer, NULL);
  return 1000000*(timer.tv_sec)+(timer.tv_usec);
}

inline double diff(const double& arg1, const double& arg2)
{
  return abs((arg1-arg2)/arg1);
}

inline double average(const vector<double>& arg)
{
  double av=0;
  for(unsigned int i=0; i<arg.size(); i++)
    if(isfinite(arg[i]))
      av+=arg[i];
  return av/(arg.size());
}


void do_computations(void (*func)(const fhb::fun_wrap&, vector<double>&, fhb::fun_wrap&),const string& name, const string& testname0)
{
  fstream load_f;
  string fname = string("./testy/");
  fname += testname0;
  fname += ".din";
  cout << "Loading from " << fname << "..." << endl;
  load_f.open(fname.c_str(), ios_base::in);
  vector<double> points;
  for(int i=0; i<3; i++)
  {
    double pt;
    load_f >> pt;
    points.push_back(pt);
  }
  string s;
  load_f >> s;
  fhb::fun_wrap function(s);
  load_f >> s;
  fhb::fun_wrap dfunction(s);
  float x_0;
  load_f >> s;
  {
    fhb::fun_wrap tempfunction(s);
    x_0 = tempfunction.apply(0);
    if(abs(x_0- tempfunction.apply(1)) > 0.1)
    {
      cerr << "Wrong x0 given. (";
      cerr << x_0 << " != " << tempfunction.apply(1) << ")\n";
      exit(255);
    }
  }
  if(load_f.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  load_f.close();
  int start = gettime();
  (*func)(function, points, dfunction);
  int end = gettime();
  fstream do_zapisu;
  fname = string("./wyniki/");
  fname += testname0;
  fname += string("/");
  fname += name;
  fname += ".dw";
  cout << "Writing to " << fname << "..." << endl;
  do_zapisu.open(fname.c_str(), ios_base::out);
  for(int i=0; i<points.size(); i++)
//    do_zapisu << i << " " << -log(diff(x_0,points[i])) << endl;
      do_zapisu << i << " " << points[i] << endl;
  if(do_zapisu.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  do_zapisu.close();
  fname = string("./wyniki/");
  fname += testname0;
  fname += string("/");
  fname += name;
  fname += ".dr";
  cout << "Writing to " << fname << "..." << endl;
  do_zapisu.open(fname.c_str(), ios_base::out);
  vector<double> rates = rate_of_conv(points, x_0);
  for(int i=0; i<rates.size(); i++)
    do_zapisu << i << " " << rates[i] << endl;
  if(do_zapisu.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  do_zapisu.close();
  fname = string("./wyniki/");
  fname += testname0;
  fname += string("/");
  fname += name;
  fname += ".dt";
  cout << "Writing to " << fname << "..." << endl;
  do_zapisu.open(fname.c_str(), ios_base::out);
  do_zapisu.precision(3);
  do_zapisu << end - start << endl;
  do_zapisu << points.size() << endl;
  do_zapisu << average(rates) << endl;
  do_zapisu << points[points.size()-1] << endl;
  if(do_zapisu.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  do_zapisu.close();
   /*DEBUG
  fname = string("./wyniki/");
  fname += testname0;
  fname += string("/");
  fname += name;
  fname += ".dp";
  cout << "Writing to " << fname << "..." << endl;
  sort(points.begin(), points.end());
  do_zapisu.open(fname.c_str(), ios_base::out);
  for(int i=0; i<points.size(); i++)
    do_zapisu << points[i] << " " << function.apply(points[i]) << " " << dfunction.apply(points[i]) << endl;
  if(do_zapisu.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  do_zapisu.close();
  // END DEBUG */
}

int main()
{
  string fname;
  cin >> fname;
  do_computations(met_siecz_wrap, "met_siecz", fname);
  do_computations(met_sty_wrap, "met_sty", fname);
  do_computations(odwr_interpol_kw_wrap, "odwr_interpol_kw", fname);
	return 0;
}
