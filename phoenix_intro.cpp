#include "phoenix_intro.h"

using namespace std;
using namespace boost::phoenix;
using namespace boost::spirit::qi;

turing_machine::turing_machine()
  : m_data(0, 30000),
    m_data_ptr(0) {}

void turing_machine::process_command(char c) {

  switch(c) {
  case '+': alter_data(1); break;
  case '-': alter_data(-1); break;
  case '<': move_data_ptr(1); break;
  case '>': move_data_ptr(-1); break;
  case '.': output_data(); break;
  case ',': input_data(); break;
  default: throw "invalid command";
  };
  
}

void turing_machine::alter_data(char a) { m_data[m_data_ptr] += a; }
void turing_machine::move_data_ptr(unsigned long i) {
  if((m_data_ptr + i) >= m_data.size())
    m_data.resize(m_data.size() + 30000 + i);
  m_data_ptr += i;
}
void turing_machine::output_data() { cout << m_data[m_data_ptr]; }
void turing_machine::input_data() { cin >> m_data[m_data_ptr]; }

void turing_machine::process_block(const std::string& s) {
  std::for_each(
		s.begin(),
		s.end(),
		bind(
		     &turing_machine::process_command,
		     this,
		     arg_names::arg1
		     )
		);
}

struct bf_grammar : grammar<string::iterator, void()> {

  bf_grammar(turing_machine& tm)
  : bf_grammar::base_type(start),
    m_tm(tm) {

    start = +command;
    command = +(char_("+-<>.,")[bind(&turing_machine::process_command, &m_tm, _1)]);
    
  }
  
  rule<string::iterator, void()> start;
  rule<string::iterator, void()> command;
  
  turing_machine& m_tm;
  
};


