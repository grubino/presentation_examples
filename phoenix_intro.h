#include<iostream>
#include<iterator>
#include<string>
#include<deque>
#include<stack>

#include<boost/spirit/include/qi.hpp>
#include<boost/spirit/include/phoenix.hpp>

class turing_machine {

public:
  turing_machine();

  void process_command(char c);
  void process_block(const std::string& s);
  bool is_zero() { return (m_data[m_data_ptr] == 0); }

private:

  void alter_data(char a);
  void move_data_ptr(unsigned long i);
  void output_data();
  void input_data();

  std::deque<char> m_data;
  unsigned long m_data_ptr;
  
};


struct bf_grammar;
