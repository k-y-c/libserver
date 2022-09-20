#include "Thread.hpp"
#include "Mutex.hpp"
#include <iostream>
#include "Log.hpp"
#include <time.h>
#include <unistd.h>

using namespace std;


void func(int cnt){
    cout<< "test : " << cnt << endl;
}


int main(){
    nanolog::initialize(nanolog::GuaranteedLogger(), ".", "nanolog", 1);
    LOG_INFO << "main";
    ThreadPool pool(4);
    pool.start();
    // sleep(15);
    for(int i  = 0;i<10;++i){
    //     // cout << "run"<< endl;
        // LOG_INFO << "main" << i;
        // Thread th(bind(func,i));
        // th.start();
        // th.join();
        pool.run(bind(func,i));
    }

    
    return 0;
}