#include "phoenix_intro.h"

using namespace std;
using namespace boost::phoenix;
using namespace boost::spirit::qi;

ostream& operator<<(ostream& os, const bf_command_sequence& bfcs) {
  for(int i = 0; i < bfcs.m_repetitions; ++i) {
    os << bfcs.m_command;
  }
}

ostream& operator<<(ostream& os, const bf_clear_cell& cc) {
  os << "[-]";
  return os;
}

ostream& operator<<(ostream& os, const bf_transfer_cell& tc) {
  os << "[" << tc.offset << ", " << tc.quantity << "]";
  return os;
}


void turing_machine::process_command_sequence(const bf_command_sequence& c) {

  //cout << "processing command_sequence: " <<
  //c.m_command << "x" << boost::lexical_cast<std::string>(c.m_repetitions) << endl;
  switch(c.m_command) {
  case '+': alter_data(c.m_repetitions); break;
  case '-': alter_data(-c.m_repetitions); break;
  case '>': move_data_ptr(c.m_repetitions); break;
  case '<': move_data_ptr(-c.m_repetitions); break;
  case '.':
    for(int i = 0; i < c.m_repetitions; ++i)
      output_data();
    break;
  case ',':
    for(int i = 0; i < c.m_repetitions; ++i)
      input_data();
    break;
  default: throw "invalid command";
  };
  
}

void turing_machine::alter_data(char a) {
  m_data[m_data_ptr] += a;
  if(m_data[m_data_ptr] < 0)
    m_data[m_data_ptr] = 0;
}
void turing_machine::move_data_ptr(long i) {
  if(m_data_ptr + i > static_cast<long>(m_data.size())) {
    m_data.resize(m_data.size() + 30000 + i, 0);
  }
  else if((m_data_ptr + i) < 0) {
    m_data.insert(m_data.begin(), 30000 - i, 0);
    m_data_ptr = 30000 - i + m_data_ptr;
  }
  m_data_ptr += i;
}
void turing_machine::output_data() const { cout << m_data[m_data_ptr]; }
void turing_machine::input_data() { cin >> m_data[m_data_ptr]; }
