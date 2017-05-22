#include <iostream>
#include <deque>
#include <atomic>
#include <mutex>
#include <thread>
#include <map>
#include <fstream>
#include <condition_variable>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cstring>


using namespace std;

deque<vector<string>> d;
map<string, int> m;
condition_variable cv;
mutex mux;
atomic <bool> done = {false};





int producer(const string& filename) {
    fstream fin(filename); //full path to the file
    if (!fin.is_open()) {
        cout << "error reading from file";
        return  0;
    }
    string line;
    vector<string> lines;
    int counter = 0;
    int fullBlock = 50;
    while(getline(fin, line))
    {
        lines.push_back(line);
        if (counter == fullBlock)
        {
            {
                lock_guard<mutex> ll(mux);
                d.push_back(lines);
            }
            cv.notify_one();
            lines.clear();
            counter = 0;
        } else
        {
            counter++;
        }

    }
    if (lines.size() != 0)
    {
        {
            lock_guard<mutex> ll(mux);
            d.push_back(lines);
        }
        cv.notify_one();
    }
    cv.notify_all(); //notify all consumers that words have finished
    done = true;
    return 0;
}

int consumer()
{
    while(true)
    {
        unique_lock<std::mutex> lk(mux);
        if (!d.empty()) {

            vector<string> v {d.front()};
            d.pop_front();
            lk.unlock();
            string word;
            for(int i = 0; i < v.size(); i++) {
                //cout<<v[i]<<endl;
                //cout<<v[i].size()<<endl;
                istringstream iss(v[i]);
                while(iss >> word){

                    //cout << word << endl;
                    char chars[] = ".,:;$%&?*()@!{}[]-^ ";
                    for (unsigned int a = 0; a < strlen(chars); ++a){
                        word.erase (std::remove(word.begin(), word.end(), chars[a]), word.end());
                    }
                    cout << word << endl;
                }
                if(word !="") {
                    lock_guard<mutex> lg(mux);
                    ++m[word];
                }
            }
        } else {
            if(done)
                break;
            else
                cv.wait(lk);
        }
    }
    return 0;

}

int main() {
    string name = "data.txt";
    thread thr1 = thread(producer, cref(name));
    thread thr2 = thread(consumer);
    thr1.join();
    thr2.join();
}