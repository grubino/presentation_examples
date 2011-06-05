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

struct bf_command_sequence {

  bf_command_sequence() {}
  bf_command_sequence(const char* c, int i)
  : m_command(*c)
  , m_repetitions(i) {}

  char m_command;
  int m_repetitions;

  friend ostream& operator<<(
			     ostream& os,
			     const bf_command_sequence& bfcs
			     );
  
};
ostream& operator<<(ostream& os, const bf_command_sequence& bfcs);

typedef std::vector<char> bf_command_string;
ostream& operator<<(ostream& os, const bf_command_string& bfcs);

typedef boost::variant<bf_command_sequence, bf_command_string> bf_command_variant;

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


struct turing_machine : grammar<std::vector<char>::iterator> {

  turing_machine()
    : turing_machine::base_type(start),
    m_data(30000, 0),
    m_data_ptr(0) {

    using boost::spirit::qi::string;
    
    start =
      eps >>
      clear_cell
      | transfer_cell
      ;
    clear_cell = string("[-]")[ref(m_data)[ref(m_data_ptr)] = val(0)];
    transfer_cell =
      transfer_cell_left
      | transfer_cell_right
      ;
    transfer_cell_right =
      char_('[') >>
      char_('-') >>
      +(char_('>')[_a += val(1)]) >>
      char_('+') >>
      repeat(_a)[char_('<')] >>
      char_(']')[ref(m_data)[ref(m_data_ptr) + _a] +=
		 ref(m_data)[ref(m_data_ptr)]
		 , ref(m_data)[ref(m_data_ptr)] = val(0)]
      ;
    transfer_cell_left =
      char_('[') >>
      char_('-') >>
      +(char_('<')[_a += val(1)]) >>
      char_('+') >>
      repeat(_a)[char_('>')] >>
      char_(']')[ref(m_data)[ref(m_data_ptr) - _a] +=
		 ref(m_data)[ref(m_data_ptr)]
		 , ref(m_data)[ref(m_data_ptr)] = val(0)]
      ;
    BOOST_SPIRIT_DEBUG_NODE(clear_cell);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell_left);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell_right);

  }

  void process_command_sequence(const bf_command_sequence& c);
  void process_command_string(bf_command_string& cs) {
    bf_command_string::iterator cs_begin = cs.begin();
    ::parse(cs_begin, cs.end(), *this);
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

  rule<bf_command_string::iterator> start;
  rule<bf_command_string::iterator> clear_cell;
  rule<bf_command_string::iterator> transfer_cell;
  rule<bf_command_string::iterator, locals<int> > transfer_cell_left;
  rule<bf_command_string::iterator, locals<int> > transfer_cell_right;

};

struct turing_machine_visitor : boost::static_visitor<> {

  turing_machine_visitor(turing_machine& tm)
    : m_tm(tm) {}

  void operator()(bf_command_string& cs) const {
    m_tm.process_command_string(cs);
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

struct bf_grammar
  : grammar<bf_command_string::iterator, vector<bf_expression>()> {

  bf_grammar()
    : bf_grammar::base_type(start) {

    start = +expression;
    expression %= command_sequence | statement;
    statement %= lit('[') >> +expression >> lit(']');
    command_sequence %= +command_group;
    command_group %= command_string | command_token;
    command_token =
      eps[_a = val(0)] >> 
      +(
	char_("+")[
		   _a += val(1)
		   ]
	)[
	  _val = construct<bf_command_sequence>(
						val("+")
						, _a
						)
	  ]
      | +(
	  char_("-")[
		     _a += val(1)
		     ]
	  )[
	    _val = construct<bf_command_sequence>(
						  val("-")
						  , _a
						  )
	    ]
      | +(
	  char_(">")[
		     _a += val(1)
		     ]
	  )[
	    _val = construct<bf_command_sequence>(
						  val(">")
						  , _a
						  )
	    ]
      | +(
	  char_("<")[
		     _a += val(1)
		     ]
	  )[
	    _val = construct<bf_command_sequence>(
						  val("<")
						  , _a
						  )
	    ]
      | +(
	  char_(".")[
		     _a += val(1)
		     ]
	  )[
	    _val = construct<bf_command_sequence>(
						  val(".")
						  , _a
						  )
	    ]
      | +(
	  char_(",")[
		     _a += val(1)
		     ]
	  )[
	    _val = construct<bf_command_sequence>(
						  val(",")
						  , _a
						  )
	    ]
      ;
    command_string =
      eps >>
      clear_cell
      | transfer_cell
      ;
    clear_cell =
      eps[clear(_val)] >>
      char_('[')[push_back(_val, _1)] >>
      char_('-')[push_back(_val, _1)] >>
      char_(']')[push_back(_val, _1)]
      ;
    transfer_cell %=
      transfer_cell_left
      | transfer_cell_right
      ;
    transfer_cell_right =
      eps[clear(_val)] >>
      char_('[')[push_back(_val, _1)] >>
      char_('-')[push_back(_val, _1)] >>
      +(char_('>')[_a += val(1), push_back(_val, _1)]) >>
      char_('+')[push_back(_val, _1)] >>
      repeat(_a)[char_('<')[push_back(_val, _1)]] >>
      char_(']')[push_back(_val, _1)]
      ;
    transfer_cell_left =
      eps[clear(_val)] >>
      char_('[')[push_back(_val, _1)] >>
      char_('-')[push_back(_val, _1)] >>
      +(char_('<')[_a += val(1), push_back(_val, _1)]) >>
      char_('+')[push_back(_val, _1)] >>
      repeat(_a)[char_('>')[push_back(_val, _1)]] >>
      char_(']')[push_back(_val, _1)]
      ;
    
    BOOST_SPIRIT_DEBUG_NODE(expression);
    BOOST_SPIRIT_DEBUG_NODE(statement);
    BOOST_SPIRIT_DEBUG_NODE(command_sequence);
    BOOST_SPIRIT_DEBUG_NODE(command_group);
    BOOST_SPIRIT_DEBUG_NODE(command_token);
    BOOST_SPIRIT_DEBUG_NODE(command_string);
    BOOST_SPIRIT_DEBUG_NODE(clear_cell);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell_left);
    BOOST_SPIRIT_DEBUG_NODE(transfer_cell_right);

  }
  
  rule<bf_command_string::iterator, vector<bf_expression>()> start;
  rule<bf_command_string::iterator, bf_expression()> expression;
  rule<bf_command_string::iterator, bf_statement()> statement;
  rule<bf_command_string::iterator, vector<bf_command_variant>()> command_sequence;
  rule<bf_command_string::iterator, bf_command_variant()> command_group;
  rule<bf_command_string::iterator, bf_command_sequence(), locals<int> > command_token;
  rule<bf_command_string::iterator, bf_command_string()> command_string;
  rule<bf_command_string::iterator, bf_command_string()> clear_cell;
  rule<bf_command_string::iterator, bf_command_string()> transfer_cell;
  rule<bf_command_string::iterator, bf_command_string(), locals<int> > transfer_cell_left;
  rule<bf_command_string::iterator, bf_command_string(), locals<int> > transfer_cell_right;
};


#endif
