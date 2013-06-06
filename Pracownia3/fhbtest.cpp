#include <iostream>
#include "fhb.cpp"
#include <fstream>
using namespace std;

int main()
{
  string fname;
  cin >> fname;
  fstream f;
  f.open(fname.c_str(), ios_base::in);
  getline(f, fname);
  f >> fname;
  fhb::fun_wrap funkcja(fname);
  while(true)
  {
    float a;
    cin >> a;
    cout << funkcja.apply(a) << endl;
  }
}
