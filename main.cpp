
#include"phoenix_intro.h"

using namespace std;
using namespace boost::spirit::qi;

int main(int argc, char** argv) {

  std::string command_string;
  cin >> command_string;

  bf_grammar bfg;

  bfg.process_block(command_string);

  return 0;
  
}
