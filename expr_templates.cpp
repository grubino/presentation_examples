#include<iostream>

using namespace std;

// op tags - these represent the parent nodes of the expression tree-lets
//
//   op_tag
//     |
//    / \
// expr expr
//
// here, expr can be a terminal node or a nonterminal expression
struct plus_tag {
  static double apply(double l, double h) { return l + h; }
};
struct minus_tag {
  static double apply(double l, double h) { return l - h; }
};
struct multiply_tag {
  static double apply(double l, double h) { return l * h; }
};
struct divide_tag {
  static double apply(double l, double h) { return l / h; }
};

// binary expression node
template <class L, class OpTag, class R>
struct expression {
  
  expression(const L& lhs, const R& rhs)
  : m_lhs(lhs), m_rhs(rhs) {}

  double operator()(double d) const { return OpTag::apply(m_lhs(d), m_rhs(d)); }
  
  const L& m_lhs;
  const R& m_rhs;
  
};

// type tags
struct variable {
  double operator()(double a) const { return a; }
};

struct constant {
  constant(double val) : m_val(val) {}
  double operator()(double) const { return m_val; }
  double m_val;
};

template <class L, class R>
expression<L, plus_tag, R>
operator+(const L& l, const R& r) {
  return expression<L, plus_tag, R>(l, r);
}

template <class L, class R>
expression<L, minus_tag, R>
operator-(const L& l, const R& r) {
  return expression<L, minus_tag, R>(l, r);
}

template <class L, class R>
expression<L, multiply_tag, R>
operator*(const L& l, const R& r) {
  return expression<L, multiply_tag, R>(l, r);
}

template <class L, class R>
expression<L, divide_tag, R>
operator/(const L& l, const R& r) {
  return expression<L, divide_tag, R>(l, r);
}

int main(int argc, char** argv) {

  variable a, b, c;
  constant m(1), n(2), p(10);
  
  double x = ((a * b + p) * c + m * n)(5);

  cout << x << endl;
  
}
