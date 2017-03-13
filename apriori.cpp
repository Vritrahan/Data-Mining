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


void prune(map<set<int>, int> &count, vector<set<int>> freq, int minsup) {  //prune items with less support

    if (!freq.empty()) {

        for (map<set<int> , int>::iterator i = count.begin(); i != count.end(); ) {

            vector<set<int>> tmp;
            for (set<int>::iterator j = (i->first).begin(); j != (i->first).end(); ++j) {
                set<int> temp = i->first;
                temp.erase(*j);
                tmp.push_back(temp);
            }

            int flag = 1;
            for (vector<set<int>>::iterator j = tmp.begin(); j != tmp.end(); ++j) {

                if (find(freq.begin(), freq.end(), *j) == freq.end()) {
                    i = count.erase(i);
                    flag = 0;
                    break;
                }
            }
            if (flag) {
                ++i;
            }
        }
    }

    for (map<set<int> , int>::iterator i = count.begin(); i != count.end(); ) {
        if (i->second < minsup) {
            i = count.erase(i);
        }
        else {
            ++i;
        }
    }
}


int support(vector<multiset<int>> items, set<int> key) {    //find support for itemset

    int flag, sup = 0;

    for (vector<multiset<int>>::iterator i = items.begin(); i != items.end(); ++i) {

        flag = 1;

        for (set<int>::iterator j = key.begin(); j != key.end(); ++j) {
            if (i->find(*j) == i->end()) {
                flag = 0;
            }
        }

        if (flag) { //all found
            sup++;
        }
    }

    return sup;
}


map<set<int>, int> join(map<set<int>, int> count, vector<multiset<int>> items) {  //join elements to prepare new count map

    map<set<int>, int> newcount;

    for (map<set<int>, int>::iterator i = count.begin(); i != count.end(); ++i) {
        map<set<int>, int>::iterator it = i;
        for (map<set<int>, int>::iterator j = ++it; j != count.end(); ++j) {
            //dispCount(newcount);
            set<int> s1((i->first).begin(), --(i->first).end()), s2((j->first).begin(), --(j->first).end());
            int n1 = *(--(i->first).end()), n2 = *(--(j->first).end());
            //cout << n1 << n2 << endl;
            if (s1 == s2 && n1 != n2) {
                //cout << "in";
                set<int> s = i->first;
                s.insert(n2);
                newcount[s] = support(items, s);
            }
        }
    }
    return newcount;
}


void add(vector<set<int>> &freq, map<set<int>, int> count) { //insert frequent itemsets of Ck

    for (map<set<int>, int>::iterator i = count.begin(); i != count.end(); ++i) {
        freq.push_back(i->first);
    }
}


int main(int argc, char const *argv[]) {

    if (argc != 3) {
        cout << "Exepcting 2 arguments!\nUsage: a.out [file.csv] [min_support]\n";
        return 1;
    }

    ifstream infile(argv[1]);   //the file
    int minsup = stol(argv[2]); //minimum support value
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

    map<set<int>, int> count;   //item count map
    for (vector<multiset<int>>::iterator i = items.begin(); i != items.end(); ++i) {    //generate count
        for (multiset<int>::iterator j = i->begin(); j != i->end(); ++j) {
            set<int> tmp;
            tmp.insert(*j);
            count[tmp]++;
        }
    }

    vector<set<int>> frequent;  //frequent itemsets

    cout << "\nC";
    dispCount(count);   //display counts

    prune(count, frequent, minsup);
    add(frequent, count);   //add to frequent itemsets

    cout << "\nL";
    dispCount(count); //display counts

    while (!count.empty()) { //join when n-1 elements are same
        count = join(count, items);
        cout << "\nC";
        dispCount(count);
        prune(count, frequent, minsup);
        add(frequent, count);
        cout << "\nL";
        dispCount(count);
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
