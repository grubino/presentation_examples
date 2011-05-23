#include<iostream>
#include<iterator>
#include<list>

using namespace std;

int main(void) {

  list<int> a(10, 0);
  list<int>::iterator it(a.begin());

  for(int i = 0; i < 15; ++i) {
    ++it;
    cout << *it << " " << a.size() << " " << endl;
  }

  if(it == a.end())
    a.resize(a.size() + 30, 0);

  cout << *it << " " << a.size() << " " << endl;

}
