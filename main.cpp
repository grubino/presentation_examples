
#include"phoenix_intro.h"

#include<fstream>
#include<iomanip>
#include<iterator>

using namespace std;
using namespace boost::spirit::qi;

int main(int argc, char** argv) {

  if(argc != 2)
    return 1;
  
  ifstream program_file(argv[1]);
  program_file >> skipws;

  std::vector<char> program;

  std::copy(
	    istream_iterator<char>(program_file),
	    istream_iterator<char>(),
	    back_inserter(program)
	    );

  bf_grammar bfg;
  std::vector<char>::iterator it_(program.begin());
  std::vector<bf_expression> ast;
  
  if(parse(it_, program.end(), bfg, ast)) {
    
    turing_machine tm;
    BOOST_FOREACH(bf_expression bf_e, ast) {
      boost::apply_visitor(turing_machine_visitor(tm), bf_e);
    }
    
  }

  
  return 0;
  
}
