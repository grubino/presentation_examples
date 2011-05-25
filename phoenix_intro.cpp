#include "phoenix_intro.h"

using namespace std;
using namespace boost::phoenix;
using namespace boost::spirit::qi;

void bf_grammar::process_command(char c) {

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

void bf_grammar::alter_data(char a) { m_data[m_data_ptr] += a; }
void bf_grammar::move_data_ptr(unsigned long i) {
  if((m_data_ptr + i) >= m_data.size())
    m_data.resize(m_data.size() + 30000 + i);
  m_data_ptr += i;
}
void bf_grammar::output_data() const { cout << m_data[m_data_ptr]; }
void bf_grammar::input_data() { cin >> m_data[m_data_ptr]; }

void bf_grammar::process_block(const std::string& s) {

  std::string::const_iterator start_(s.begin());
  if(!parse(start_, s.end()))
    throw "invalid program";
  
}

