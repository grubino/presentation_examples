#include <iostream>
#include <iterator>
#include <string>
#include <deque>
#include <vector>
#include <stack>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

struct bf_statement;

typedef boost::variant<
boost::recursive_wrapper<bf_statement>
, std::vector<char>
  > bf_expression;

struct bf_statement {
  std::vector<bf_expression> stmt;
};

BOOST_FUSION_ADAPT_STRUCT(
			  bf_statement,
			  (std::vector<bf_expression>, stmt)
			  )

using namespace boost::spirit::qi;
using namespace boost::phoenix;
using namespace std;

struct turing_machine {

  turing_machine()
  : m_data(30000, 0),
    m_data_ptr(0) {}

  void process_command(char c);
  void process_command_sequence(const std::vector<char>& v);

  bool is_zero() const { return (m_data[m_data_ptr] == 0); }
  void alter_data(char a);
  void move_data_ptr(long i);
  void output_data() const;
  void input_data();

  std::deque<char> m_data;
  long m_data_ptr;

};

struct turing_machine_visitor : boost::static_visitor<> {

  turing_machine_visitor(turing_machine& tm)
    : m_tm(tm) {}

  void operator()(const std::vector<char>& v) const { m_tm.process_command_sequence(v); }

  void operator()(const bf_statement& s) const {
    while(!m_tm.is_zero()) {
      BOOST_FOREACH(bf_expression bf_e, s.stmt) {
	boost::apply_visitor(*this, bf_e);
      }
    }
  }
  
  turing_machine& m_tm;
  
};

struct bf_grammar
  : grammar<std::vector<char>::iterator, vector<bf_expression>()> {

  bf_grammar()
    : bf_grammar::base_type(start) {

    start = +expression;
    expression %= command_sequence | statement;
    statement %= lit('[') >> +expression >> lit(']');
    command_sequence %= +command_token;
    command_token %= char_("+-><.,");
    
    expression.name("expression");
    statement.name("statement");
    command_sequence.name("command_sequence");
    command_token.name("command_token");
    
    //debug(expression);
    //debug(statement);
    //debug(command_sequence);
    //debug(command_token);
    
  }
  
  rule<std::vector<char>::iterator, vector<bf_expression>()> start;
  rule<std::vector<char>::iterator, bf_expression()> expression;
  rule<std::vector<char>::iterator, bf_statement()> statement;
  rule<std::vector<char>::iterator, vector<char>()> command_sequence;
  rule<std::vector<char>::iterator, char()> command_token;

};
