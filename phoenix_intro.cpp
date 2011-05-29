#include "phoenix_intro.h"

using namespace std;
using namespace boost::phoenix;
using namespace boost::spirit::qi;

void turing_machine::process_command(char c) {

  switch(c) {
  case '+': alter_data(1); break;
  case '-': alter_data(-1); break;
  case '<': move_data_ptr(-1); break;
  case '>': move_data_ptr(1); break;
  case '.': output_data(); break;
  case ',': input_data(); break;
  default: throw "invalid command";
  };
  
}

void turing_machine::process_command_sequence(const std::vector<char>& v) {
  std::for_each(
		v.begin(),
		v.end(),
		bind(
		     &turing_machine::process_command,
		     this,
		     arg_names::arg1
		     )
		);
}

void turing_machine::alter_data(char a) {
  m_data[m_data_ptr] += a;
  if(m_data[m_data_ptr] < 0)
    m_data[m_data_ptr] = 0;
}
void turing_machine::move_data_ptr(long i) {
  if((m_data_ptr + i) > m_data.size())
    m_data.resize(m_data.size() + 30000, 0);
  m_data_ptr += i;
}
void turing_machine::output_data() const { cout << m_data[m_data_ptr]; }
void turing_machine::input_data() { cin >> m_data[m_data_ptr]; }
