#include<iostream>
#include<vector>
#include<list>
#include<algorithm>
#include<boost/proto/proto.hpp>
#include<boost/foreach.hpp>

using namespace boost;
using namespace std;

template <int I> struct placeholder {};


const proto::terminal<placeholder<0> >::type _1 = {{}};
const proto::terminal<placeholder<1> >::type _2 = {{}};

struct calculator_context
  : proto::callable_context< calculator_context const > {

  std::vector<double> args;

  typedef double result_type;

  template <int I>
  double operator()(proto::tag::terminal, placeholder<I>) const {
    return this->args[I];
  }

};


int main() {

  calculator_context ctx;
  ctx.args.push_back(10);
  ctx.args.push_back(20);
  
  double d = proto::eval((_1 + _2), ctx);
    
  cout << d << endl;
  
  return 0;
  
}
