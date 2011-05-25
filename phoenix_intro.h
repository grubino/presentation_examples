#include<iostream>
#include<iterator>
#include<string>
#include<deque>
#include<stack>

#include<boost/spirit/include/qi.hpp>
#include<boost/spirit/include/phoenix.hpp>

using namespace boost::spirit::qi;
using namespace boost::phoenix;
using namespace std;

struct bf_grammar
  : grammar<std::string::iterator> {

  bf_grammar()
    : bf_grammar::base_type(start),
    m_data(0, 30000),
    m_data_ptr(0) {

    start = +expression(true);
    expression = (
		  command[
			  if_(_r1)[
				   bind(
					&bf_grammar::process_block,
					this,
					_1
					)
				   ]
			  ] |
		  statement(_r1)
		  );
    statement = (lit('[') >> (+expression(bind(&bf_grammar::is_zero, this)) >> lit(']')));
    command = +(char_("+-<>.,"));
    
  }
  
  rule<string::iterator> start;
  rule<string::iterator> expression;
  rule<string::iterator> statement;
  rule<string::iterator> command;
  
  void process_command(char c);
  void process_block(const std::string& s);
  bool is_zero() const { return (m_data[m_data_ptr] == 0); }
  void alter_data(char a);
  void move_data_ptr(unsigned long i);
  void output_data() const;
  void input_data();

  std::deque<char> m_data;
  unsigned long m_data_ptr;
  
};


