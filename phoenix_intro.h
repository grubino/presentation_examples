#ifndef __PHOENIX_INTRO_H__
#define __PHOENIX_INTRO_H__

//#define BOOST_SPIRIT_DEBUG

#include <iostream>
#include <iterator>
#include <string>
#include <deque>
#include <vector>
#include <stack>

#include <boost/lexical_cast.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/variant/recursive_variant.hpp>
#include <boost/foreach.hpp>

using namespace boost::spirit::qi;
using namespace boost::phoenix;
using namespace std;

typedef vector<char> bf_command_string;

struct bf_command_sequence {

  bf_command_sequence() {}
  bf_command_sequence(char c, int i)
  : m_command(c)
  , m_repetitions(i) {}

  char m_command;
  int m_repetitions;

  friend ostream& operator<<(
			     ostream& os,
			     const bf_command_sequence& bfcs
			     );
  
};
ostream& operator<<(ostream& os, const bf_command_sequence& bfcs);



struct bf_clear_cell {};
ostream& operator<<(ostream& os, const bf_clear_cell& cc);



struct bf_transfer_cell {

  bf_transfer_cell() {}
  bf_transfer_cell(long x, int y)
  : offset(x)
  , quantity(y) {}
  
  long offset;
  int quantity;
  
};
ostream& operator<<(ostream& os, const bf_transfer_cell& tc);



typedef boost::variant<
bf_clear_cell
, bf_transfer_cell
  > bf_known_command;



typedef boost::variant<
bf_known_command
, bf_command_sequence
  > bf_command_variant;



struct bf_statement;

typedef boost::variant<
boost::recursive_wrapper<bf_statement>
, std::vector<bf_command_variant>
  > bf_expression;



struct bf_statement {
  std::vector<bf_expression> stmt;
};

BOOST_FUSION_ADAPT_STRUCT(
			  bf_statement,
			  (std::vector<bf_expression>, stmt)
			  )


struct turing_machine {

  turing_machine()
  : m_data(30000, 0),
    m_data_ptr(0) {}

  void process_command_sequence(const bf_command_sequence& c);
  void process_command(const bf_clear_cell&) {
    m_data[m_data_ptr] = 0;
  }
  void process_command(const bf_transfer_cell& tc) {
    char temp = m_data[m_data_ptr];
    move_data_ptr(tc.offset);
    alter_data(tc.quantity * temp);
    move_data_ptr(-tc.offset);
    process_command(bf_clear_cell());
  }

  bool is_zero() const { return (m_data[m_data_ptr] == 0); }
  void alter_data(char a);
  void move_data_ptr(long i);
  void output_data() const;
  void input_data();

  void print_state() {
    cout << "data pointer position: " << m_data_ptr << endl;
    cout << "data pointer content: " << static_cast<int>(m_data[m_data_ptr]) << endl;
  }

  std::deque<char> m_data;
  long m_data_ptr;

};

/*****************************************************************************
 * turing_machine_visitor
 * ======================
 * this structure acts as a dispatch for different command types from the
 * brainf*ck input.
 *****************************************************************************/

struct turing_machine_visitor : boost::static_visitor<> {

  turing_machine_visitor(turing_machine& tm)
    : m_tm(tm) {}

  void operator()(const bf_known_command& kc) const {
    boost::apply_visitor(*this, kc);
  }
  
  void operator()(const bf_clear_cell& cc) const {
    m_tm.process_command(cc);
  }

  void operator()(const bf_transfer_cell& tc) const {
    m_tm.process_command(tc);
  }
  
  void operator()(bf_command_sequence& cs) const {
    m_tm.process_command_sequence(cs);
  }
  
  void operator()(const std::vector<bf_command_variant>& v) const {
    BOOST_FOREACH(bf_command_variant cv, v) {
      boost::apply_visitor(*this, cv);
    }
  }

  void operator()(const bf_statement& s) const {
    while(!m_tm.is_zero()) {
      BOOST_FOREACH(bf_expression bf_e, s.stmt) {
	boost::apply_visitor(*this, bf_e);
      }
    }
  }
  
  turing_machine& m_tm;
  
};

/*****************************************************************************************
 * bf_grammar
 * ==========
 * the grammar parses the input into optimized structures so that we don't have to deal
 * directly with the brainf*ck input.
 *****************************************************************************************/

struct bf_grammar
  : grammar<bf_command_string::iterator, vector<bf_expression>()> {

  bf_grammar()
    : bf_grammar::base_type(start) {

    start = +expression;
    expression %= command_sequence | statement;
    statement %= lit('[') >> +expression >> lit(']');
    command_sequence %= +command_group;
    command_group %= known_command_sequence | command_token;

    known_command_sequence %=
      clear_cell
      | transfer_cell
      ;
    command_token =
      eps[_a = val(0)] >> 
      +(increment[_a += val(1)])
      [_val = construct<bf_command_sequence>(
					     val('+')
					     , _a
					     )]
      | +(decrement[_a += val(1)])
      [_val = construct<bf_command_sequence>(
					     val('-')
					     , _a
					     )]
      | +(move_right[_a += val(1)])
      [_val = construct<bf_command_sequence>(
					     val('>')
					     , _a
					     )]
      | +(move_left[_a += val(1)])
      [_val = construct<bf_command_sequence>(
					     val('<')
					     , _a
					     )]
      | +(put_output[_a += val(1)])
      [_val = construct<bf_command_sequence>(
					     val('.')
					     , _a
					     )]
      | +(get_input[_a += val(1)])
      [_val = construct<bf_command_sequence>(
					     val(',')
					     , _a
					     )]
      ;
    
    clear_cell = boost::spirit::qi::string("[-]")[_val = construct<bf_clear_cell>()]
      ;
    transfer_cell %=
      transfer_cell_left
      | transfer_cell_right
      ;
    
    transfer_cell_right =
      eps[_a = val(0), _b = val(0)] >>
      open_bracket >>
      decrement >>
      +(move_right[_a += val(1)]) >>
      +(
	increment[_b += val(1)]
	| decrement[_b -= val(1)]
	) >>
      repeat(_a)[move_left] >>
	close_bracket[_val = construct<bf_transfer_cell>(_a, _b)]
      ;
    transfer_cell_left =
      eps[_a = val(0), _b = val(0)] >>
      open_bracket >>
      decrement >>
      +(move_left[_a += val(1)]) >>
      +(
	increment[_b += val(1)]
	| decrement[_b -= val(1)]
	) >>
      repeat(_a)[move_right] >>
	close_bracket[_val = construct<bf_transfer_cell>(-_a, _b)]
      ;
    
    increment %= char_('+');
    decrement %= char_('-');
    move_right %= char_('>');
    move_left %= char_('<');
    open_bracket %= char_('[');
    close_bracket %= char_(']');
    put_output %= char_('.');
    get_input %= char_(',');

    
    BOOST_SPIRIT_DEBUG_NODE(expression);
    BOOST_SPIRIT_DEBUG_NODE(statement);
    BOOST_SPIRIT_DEBUG_NODE(command_sequence);
    BOOST_SPIRIT_DEBUG_NODE(command_group);
    BOOST_SPIRIT_DEBUG_NODE(command_token);
    BOOST_SPIRIT_DEBUG_NODE(clear_cell);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell_left);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell_right);
    BOOST_SPIRIT_DEBUG_NODE(increment);
    BOOST_SPIRIT_DEBUG_NODE(decrement);
    BOOST_SPIRIT_DEBUG_NODE(move_right);
    BOOST_SPIRIT_DEBUG_NODE(move_left);
    BOOST_SPIRIT_DEBUG_NODE(get_input);
    BOOST_SPIRIT_DEBUG_NODE(put_output);

  }
  
  rule<bf_command_string::iterator, vector<bf_expression>()> start;
  rule<bf_command_string::iterator, bf_expression()> expression;
  rule<bf_command_string::iterator, bf_statement()> statement;
  rule<bf_command_string::iterator, vector<bf_command_variant>()> command_sequence;
  rule<bf_command_string::iterator, bf_command_variant()> command_group;
  rule<bf_command_string::iterator, bf_command_sequence(), locals<int> > command_token;
  rule<bf_command_string::iterator, bf_known_command()> known_command_sequence;
  rule<bf_command_string::iterator, bf_clear_cell()> clear_cell;
  rule<bf_command_string::iterator, bf_transfer_cell()> transfer_cell;
  rule<bf_command_string::iterator, bf_transfer_cell(), locals<long, int> > transfer_cell_left;
  rule<bf_command_string::iterator, bf_transfer_cell(), locals<long, int> > transfer_cell_right;
  
  rule<bf_command_string::iterator, char()> increment;
  rule<bf_command_string::iterator, char()> decrement;
  rule<bf_command_string::iterator, char()> move_left;
  rule<bf_command_string::iterator, char()> move_right;
  rule<bf_command_string::iterator, char()> open_bracket;
  rule<bf_command_string::iterator, char()> close_bracket;
  rule<bf_command_string::iterator, char()> get_input;
  rule<bf_command_string::iterator, char()> put_output;

};


#endif
