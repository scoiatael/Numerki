#include <cstring>
#include <cmath>
#include <iostream>
#include <cstdlib>
#include <cstdio>
using namespace std;

#define uint unsigned int
namespace fhb
{
  union ptrtof
  {
      double (*ptrtof1)(double);
      double (*ptrtof2)(double, double);
      double* val;
  };

  struct function
  {
      unsigned char base;
      /*
          bool:       bit in base:
          fof1arg;   - 0th
          var;       - 1st
          num;       - 2nd
          rev        - 3rd
          neg        - 4th
          */
          ptrtof ptr1;
          function* arg1;
          function* arg2;

      bool isrev() const
      {
          return (bool)((base>>3)%2);
      }
      bool isneg() const
      {
          return (bool)((base>>4)%2);
      }
      bool isfof1arg() const
      {
          return (bool)(base%2);
      }
      bool isvar() const
      {
          return (bool)((base>>1)%2);
      }
      bool isnum() const
      {
          return (bool)((base>>2)%2);
      }
      double getval() const
      {
          if(isnum())
              return (*(ptr1.val));
          return sqrt(-1);
      }
      void setneg(bool arg)
      {
          if(arg)
              base|=16;
          else
              if(isneg())
                  base-=16;
      }
      void setrev(bool arg)
      {
          if(arg)
              base|=8;
          else
              if(isrev())
                  base-=8;
      }

      void setfof1arg(bool arg)
      {
          if(arg)
              base|=1;
          else
              if(isfof1arg())
                  base-=1;
      }
      void setvar(bool arg)
      {
          if(arg)
              base|=2;
          else
              if(isvar())
                  base-=2;
      }
      void setnum(bool arg)
      {
          if(arg)
              base|=4;
          else
              if(isnum())
                  base-=4;
      }
      void setval(double arg)
      {
          if(isnum())
          {
              if(ptr1.val==NULL)
              {
                  ptr1.val = new(double);
              }
              (*(ptr1.val)) = arg;
          }
      }
      function()
      {
          base=0;
          ptr1.ptrtof1=NULL;
          arg1=NULL;
          arg2=NULL;
      }
      void del_w_offspr()
      {
          if(!isnum() && !isvar())
          {
              arg1->del_w_offspr();
              if(!isfof1arg())
                  arg2->del_w_offspr();
          }
          if(isnum())
              delete(ptr1.val);
          delete(this);
      }
      double apply(double x) const
      {
          double y=0;
          if(isnum() || isvar() || isfof1arg())
          {
              if(isnum())
                  y = getval();
              if(isvar())
                  y = x;
              if(isfof1arg())
                  y = (*(ptr1.ptrtof1))(arg1->apply(x));
          }
          else
              y = (*(ptr1.ptrtof2))(arg1->apply(x), arg2->apply(x));
          if(isneg())
              y=-y;
          if(isrev())
              y=1/y;
          return y;
      }
  };

  void wypisz(function* fname);

  function* atofunc(string s, uint& a);

  struct fun_wrap
  {
    private: 
      function* base;
    public:
      fun_wrap(string S)
      {
        uint a=0;
        base = atofunc(S,a);
      }
      ~fun_wrap()
      {
        base->del_w_offspr();
      }
      double apply(const double& x) const
      {
        return base->apply(x);
      }
      void wypisz()
      {
        fhb::wypisz(base);
      }
  };

  double abs_val(double x)
  {
      if(x<0)
          return -x;
      return x;
  }
  int nrbyint(double x)
  {
      return (int)(x+0.5);
  }
  bool ischar(char A)
  {
      if(((A >= 'a') && (A <= 'z')) || ((A >= 'A') && (A <= 'Z')))
          return true;
      return false;
  }
  bool isdigit(char A)
  {
      if(A>='0' && A<='9')
          return true;
      return false;
  }
  double mult(double x, double y)
  {
      return x*y;
  }
  double div(double x, double y)
  {
      return x/y;
  }
  double sum(double x, double y)
  {
      return x+y;
  }
  double dif(double x, double y)
  {
      return x-y;
  }
  double mpix(double arg)
  {
    return M_PI;
  }

  function* atonum(string S, uint &cur_pos)
  {
      function* new_arg = new(function);
      if(isdigit(S[cur_pos]))
        {
          new_arg->setnum(true);
          double buffer=0;
          sscanf(S.c_str()+cur_pos,"%lf", &buffer);    
          while(isdigit(S[cur_pos]))
              cur_pos++;
          if(S[cur_pos]=='.')
          {
              cur_pos++;
              while(isdigit(S[cur_pos]))
                  cur_pos++;
          }
          new_arg->setval(buffer);
          return new_arg;
      }
      if(S[cur_pos]=='x')
      {
          new_arg->setvar(true);
          cur_pos++;
          return new_arg;
      }
      string fname;
      fname.clear();
      while(S[cur_pos] != '(' && ischar(S[cur_pos]) && S[cur_pos]!='x')
      {

          fname.push_back(S[cur_pos]);
          cur_pos++;
      }
      //ADD NEW FUNCTIONS HERE:
      //Checks if it "knows" this %function. If yes, stores pointer to it in ::function new_arg.
      if(!strcmp(fname.c_str(), "sin"))
      {
          new_arg->ptr1.ptrtof1 = &sin;
          new_arg->setfof1arg(true);
      }
      if(!strcmp(fname.c_str(), "cos"))
      {
          new_arg->ptr1.ptrtof1 = &cos;
          new_arg->setfof1arg(true);
      }
      if(!strcmp(fname.c_str(), "sqrt"))
      {
          new_arg->ptr1.ptrtof1 = &sqrt;
          new_arg->setfof1arg(true);
      }
      if(!strcmp(fname.c_str(), "ln"))
      {
          new_arg->ptr1.ptrtof1 = &log;
          new_arg->setfof1arg(true);
      }
      if(!strcmp(fname.c_str(), "mpi"))
      {
          new_arg->ptr1.ptrtof1 = &mpix;
          new_arg->setfof1arg(true);
      }
  //If it doesn't know this function return error.
      if(new_arg->ptr1.ptrtof1 == NULL)
      {
          std::cerr << "Error, found unknown function: " << fname << endl;
          delete(new_arg);
          exit(255);
      }
      new_arg->arg1 = atofunc(S, cur_pos);
      return new_arg;
  }


  function* atoelem(string S, uint &cur_pos)
  {
      function* new_arg = new(function);
      bool rev=false;
      if(S[cur_pos]=='/')
      {
          cur_pos++;
          rev=true;
      }
      if(S[cur_pos]=='*')
          cur_pos++;
      if(S[cur_pos]=='(')
         new_arg->arg1=atofunc(S, cur_pos);
      else
          new_arg->arg1=atonum(S, cur_pos);
      new_arg->arg1->setrev(rev);
      if(S[cur_pos]=='*' || S[cur_pos]=='/')
      {
          new_arg->ptr1.ptrtof2=&mult;
          new_arg->arg2=atoelem(S, cur_pos);
          return new_arg;
      }
      else
      {
          function* new_argx = new_arg->arg1;
          delete(new_arg);
          return new_argx;
      }
  }

  function* atofact(string S, uint &cur_pos)
  {
      function* new_arg = new(function);
      bool neg=false;
      if(S[cur_pos]=='-')
      {
          cur_pos++;
          neg=true;
      }
      if(S[cur_pos]=='+')
          cur_pos++;
      new_arg->arg1=atoelem(S, cur_pos);
      new_arg->arg1->setneg(neg);
      if(S[cur_pos]=='+' || S[cur_pos]=='-')
      {
          new_arg->ptr1.ptrtof2=&sum;
          new_arg->arg2=atofact(S, cur_pos);
          return new_arg;
      }
      else
      {
          function* new_argx = new_arg->arg1;
          delete(new_arg);
          return new_argx;

      }
  }

  function* atofunc(string S, uint &cur_pos)
  {
      function* new_arg;
      if(S[cur_pos]=='(')
          cur_pos++;
      new_arg=atofact(S, cur_pos);
      if(S[cur_pos]==')')
          cur_pos++;
      return new_arg;
  }



  ///DEBUG
  void wypisz(function* fname)
  {
      if(fname->isnum())
      {
          cout << fname->getval();
          return;
      }
      if(fname->isvar())
      {
          cout << "x";
          return;
      }
      if(fname->isfof1arg())
      {
          cout<<"func1(";
          wypisz(fname->arg1);
          cout<<") ";
          return;
      }
      cout<<"func2(";
      wypisz(fname->arg1);
      cout<<", ";
      wypisz(fname->arg2);
      cout<<") ";
      cout << endl;
      return;
  }
}

