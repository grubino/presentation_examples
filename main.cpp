
#include"phoenix_intro.h"

using namespace std;

int main(int argc, char** argv) {

  turing_machine tm;
  string command_string;
  
  cin >> command_string;

  tm.process_block(command_string);

  return 0;
  
}
