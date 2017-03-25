#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <fstream>
#include <sstream>
#include <iterator>
#include <algorithm>

using namespace std;


void dispCount(map<set<int>, int> count) {  //display item counts

    cout << "\nItem counts:\nItem\tCount\n";

    for (map<set<int> , int>::iterator i = count.begin(); i != count.end(); ++i) {  //display counts
        for (set<int>::iterator j = (i->first).begin(); j != (i->first).end(); ++j) {
            cout << *j;
        }

        cout << '\t' << i->second << endl;
    }
}


int tsupport(multiset<int> item, set<int> key) {    //find transaction-wise support for itemset

    int flag = 1, sup = 0;

    for (set<int>::iterator j = key.begin(); j != key.end(); ++j) {
        if (item.find(*j) == item.end()) {
            flag = 0;
        }
    }

    if (flag) { //all found
        sup++;
    }

    return sup;
}


void join(vector<set<int>> dsqaure, map<set<int>, int> &lattice, map<set<int>, int> &age) {  //join elements to prepare new itemsets

    for (vector<set<int>>::iterator i = dsqaure.begin(); i != dsqaure.end(); ++i) {
        //cout << "in";
        vector<set<int>>::iterator it = i;
        for (vector<set<int>>::iterator j = ++it; j != dsqaure.end(); ++j) {
            //dispCount(lattice);
            set<int> s1(i->begin(), --(i->end())), s2(j->begin(), --(j->end()));
            int n1 = *(--(i->end())), n2 = *(--(j->end()));
            //cout << n1 << n2 << endl;
            if (s1 == s2 && n1 != n2) {
                //cout << "in";
                //cout << n1 << n2 << endl;
                set<int> s = *i;
                s.insert(n2);

                /*for (set<int>::iterator k = s.begin(); k != s.end(); ++k) {
                    cout << *k;
                }
                cout << endl;*/

                lattice[s] = 0;
                age[s] = 0;
            }
        }
    }
}


int main(int argc, char const *argv[]) {

    if (argc != 4) {
        cout << "Exepcting 2 arguments!\nUsage: a.out [file.csv] [min_support] [step]\n";
        return 1;
    }

    ifstream infile(argv[1]);   //the file
    int minsup = stol(argv[2]), m = stol(argv[3]); //minimum support value, step value
    string line;
    vector<multiset<int>> items;    //the scraped values

    while (getline(infile, line)) { //scraping the csv file
        stringstream stream(line);
        string word;
        multiset<int> s;
        while (getline(stream, word, ',')) {
            s.insert(stol(word));
        }
        items.push_back(s);
    }

    for (vector<multiset<int>>::iterator i = items.begin(); i != items.end(); ++i) {    //display the values
        for (multiset<int>::iterator j = i->begin(); j != i->end(); ++j) {
            cout << *j << '\t';
        }
        cout << endl;
    }

    map<set<int>, int> lattice; //the lattice of all itemsets
    vector<set<int>> dsqaure;   //dashed squares
    vector<set<int>> frequent;  //solid squares
    map<set<int>, int> age;     //age of every alive itemset

    for (vector<multiset<int>>::iterator i = items.begin(); i != items.end(); ++i) {    //generate 1-itemsets
        for (multiset<int>::iterator j = i->begin(); j != i->end(); ++j) {
            set<int> tmp;
            tmp.insert(*j);
            lattice[tmp] = 0;
            age[tmp] = 0;
        }
    }

    //dispCount(lattice);
    //dispCount(age);

    vector<multiset<int>>::iterator i = items.begin();
    while (!lattice.empty()) {  //DIC starts
        for (int h = 0; h < m; ++h) {   //move step-wise throough transactions
            for (map<set<int>, int>::iterator j = lattice.begin(); j != lattice.end(); ++j) {   //update frequency
                j->second += tsupport(*i , j->first);
            }

            ++i;
            if(i == items.end()){
                i = items.begin();
            }

            //dispCount(lattice);
            //dispCount(age);
        }

        for (map<set<int>, int>::iterator j = age.begin(); j != age.end(); ++j) {   //update age
            j->second += m;
        }

        //printf("Here1\n");
        for (map<set<int>, int>::iterator i = lattice.begin(); i != lattice.end(); ++i) {   //mark dashed square
            if (i->second >= minsup) {
                set<int> tmp = i->first;
                if (find(dsqaure.begin(), dsqaure.end(), tmp) == dsqaure.end()) {
                    dsqaure.push_back(tmp);
                }
            }
        }
        join(dsqaure, lattice, age);
        /*for (vector<set<int>>::iterator i = dsqaure.begin(); i != dsqaure.end(); ++i) {    //display the values
            for (set<int>::iterator j = i->begin(); j != i->end(); ++j) {
                cout << *j << '\t';
            }
            cout << endl;
        }*/

        //printf("Here2\n");
        for (map<set<int>, int>::iterator i = age.begin(); i != age.end(); ) {   //mark solid by removing
            //printf("In\n");
            int flag = 1;
            if (i->second >= items.size()) {
                //cout << "in";
                flag = 0;
                lattice.erase(i->first);
                //lattice.erase(remove(lattice.begin(), lattice.end(), i->first));
                if (find(dsqaure.begin(), dsqaure.end(), i->first) != dsqaure.end()) {  //mark solid square
                    //cout << "in";
                    //dsqaure.erase(i->first);
                    dsqaure.erase(remove(dsqaure.begin(), dsqaure.end(), i->first));
                    set<int> tmp = i->first;
                    frequent.push_back(tmp);
                }
                i = age.erase(i);
            }
            if (flag) {
                ++i;
            }


            //dispCount(lattice);
            //dispCount(age);
        }

        //dispCount(lattice);
        //dispCount(age);
    }

    cout << "\nFrequent:\n";
    for (vector<set<int>>::iterator i = frequent.begin(); i != frequent.end(); ++i) {    //generate count
        for (set<int>::iterator j = i->begin(); j != i->end(); ++j) {
            cout << *j;
        }
        cout << endl;
    }

    return 0;
}
