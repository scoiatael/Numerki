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

double spline_int_at(const fhb::fun_wrap& f , const pair<double, double>& O, const pair<double, double>& P, const double& x)
{
  double 
    a = O.first,
    b = P.first,
    d = b - a,
    M = O.second,
    N = P.second;
  return (
      -a*x*(6*f.apply(b)-pow(d,2)*M)/6
          +b*x*(6*f.apply(a)-pow(d,2)*N)/6
            -pow(x,2)*(6*f.apply(a)-pow(d,2)*N)/12
              -M*pow((a-x),3)/18
                +N*pow((x-a),4)/24
                  +pow(x,2)*(6*f.apply(b)-pow(d,2)*M)/12)/(d);
}

// M -> (M_(k-1), x_(k-1)), N -> (M_k, x_k), thus area from beg of this interval (x_(k-1), x_k) from beginning to x is equal to difference of value of indef integral at x and that value at x_(k-1)
double area(const fhb::fun_wrap& func, const pair<double, double>& M, const pair<double, double>& N, const double& x)
{
  return spline_int_at(func, M, N, x) - spline_int_at(func, M, N, M.first);
}

double integral_approx_at_points(const fhb::fun_wrap& func, const vector<double>& points, const double& x)
{
  vector< pair<double,double> > spline;
  double integral_ax=0;
  spline_params(func, points, spline);
  //ASSERT(x < points[points.size()-1])
  int i;
  for(i=1; points[i+1] < x;i++)
    integral_ax+=area(func, spline[i-1], spline[i], spline[i].first);
  integral_ax+=area(func, spline[i-1], spline[i], x);
  return integral_ax;
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

void integral_lim_n(const fhb::fun_wrap& function, const double& a, const double& b, const double& x, vector<double>& int_mono, vector<double>& int_dist)
{
  srand(gettime());
  int_mono.clear();
  int_dist.clear();
  for(int n=1; n<100; n++)
  {
    vector<double> monospaced;
    create_monosp_interval(a,b,n,monospaced);
    int_mono.push_back(integral_approx_at_points(function, monospaced, x));
    vector<double>& distorted = monospaced;
    distort_interval(distorted);
    int_dist.push_back(integral_approx_at_points(function, distorted, x));
  }
}

inline double diff(const double& arg1, const double& arg2)
{
  return -log(abs((arg1-arg2)/arg1))/log(10);
}

inline double average(const vector<double>& arg)
{
  double av=0;
  for(unsigned int i=0; i<arg.size(); i++)
    if(isfinite(arg[i]))
      av+=arg[i];
  return av/(arg.size());
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


void do_computations(const string& name, const string& testname0)
{
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
  double a, b;
  load_f >> a >> b;
  if(a>=b)
  {
    cerr << "Failed. Wrong domain given.\n";
    exit(255);
  }
  double x;
  load_f >> x;
  if(a>x || b<x)
  {
    cerr << "Failed. Wrong x given.\n";
    exit(255);
  }
  if(load_f.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  load_f.close();
  /** Obliczenia*/
  vector<double> int_mono, int_dist;
  integral_lim_n(function, a, b, x, int_mono, int_dist);
  double int_true = dfunction.apply(x) - dfunction.apply(a);
  /* */
  fstream do_zapisu;
  fname = string("./wyniki/");
  fname += testname0;
  fname += string("/");
  fname += name;
  fname += ".dw";
  cout << "Writing to " << fname << "..." << endl;
  do_zapisu.open(fname.c_str(), ios_base::out);
  /** Zapis wynikow1*/
  //ASSERT(int_mono.size()==int_dist.size())
  for(int i=1; i < int_mono.size(); i++)
    do_zapisu << i << " " << int_true << " " << int_mono[i] << " " << int_dist[i] << endl;
  /* */
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
  fname += ".dd";
  cout << "Writing to " << fname << "..." << endl;
  do_zapisu.open(fname.c_str(), ios_base::out);
  /** Zapis wynikow1*/
  //ASSERT(int_mono.size()==int_dist.size())
  for(int i=1; i < int_mono.size(); i++)
    do_zapisu << i << " " << diff(int_true, int_mono[i]) << " " << diff(int_true, int_dist[i]) << endl;
  /* */
  if(do_zapisu.fail())
  {
    cerr << "Failed.\n";
    exit(255);
  }
  else
    cout << "Done.\n";
  do_zapisu.close();
  
  fname = string("./wyniki/");
  fname += string("statistics");
  fname += ".tex";
  cout << "Writing to " << fname << "..." << endl;
  do_zapisu.open(fname.c_str(), ios_base::out | ios_base::app);
  /** Zapis wynikow2*/
  do_zapisu << s << " & " << a << " & " << b << " & " << x << " & "
     << int_true << " & " << int_mono[int_mono.size()-1] << " & " 
      << int_dist[int_dist.size()-1] << " & " << diff(int_true, int_mono[int_mono.size()-1]) << " & "
            << diff (int_true, int_dist[int_dist.size()-1]) << "\\\\"
              << endl;
  /**/ 
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
  do_computations("Integrals", fname);
	return 0;
}
