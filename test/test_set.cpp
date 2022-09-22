#include <set>

using namespace std;

bool comp_( int a, int b){
    return a<b;
}

int main(){

    bool(*c)(int,int) = comp_;
    set<int,decltype(comp_)> hash(*comp_);
    return 0;
}