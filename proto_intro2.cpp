#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<boost/proto/proto.hpp>
#include<boost/foreach.hpp>

using namespace boost;
using namespace std;

template <int I> struct placeholder {};


struct calculator_context
  : proto::callable_context< calculator_context const > {

  std::vector<double> args;

  typedef double result_type;

  template <int I>
  double operator()(proto::tag::terminal, placeholder<I>) const {
    return this->args[I];
  }

};


template <class Expr> 
struct calculator;


struct calculator_domain
  : proto::domain< proto::generator<calculator> > {};


template <class Expr>
struct calculator
  : proto::extends<Expr, calculator<Expr>, calculator_domain> {

  typedef proto::extends<Expr, calculator<Expr>, calculator_domain> base_type;
  typedef double result_type;

  calculator(const Expr& expr = Expr()) : base_type(expr) {}

  double operator()(double a1 = 0, double a2 = 0) const {

    calculator_context ctx;
    
    ctx.args.push_back(a1);
    ctx.args.push_back(a2);

    return proto::eval(*this, ctx);
    
  }
  
};

const calculator<proto::terminal<placeholder<0> >::type> _1;
const calculator<proto::terminal<placeholder<1> >::type> _2;

int main() {

  list<double> a;
  list<double> b;

  a.push_back(10.4);
  a.push_back(4.3);
  a.push_back(22.334);
  a.push_back(384.1);
  a.push_back(8.232);

  b.push_back(0.23);
  b.push_back(9.93);
  b.push_back(1.23);
  b.push_back(10.01);
  b.push_back(55.65);

  std::transform(
		 a.begin()
		 , a.end()
		 , b.begin()
		 , a.begin()
		 , (_1 + _2)
		 );

  BOOST_FOREACH(double d, a) {
    cout << d << endl;
  }
  
  return 0;
  
}
