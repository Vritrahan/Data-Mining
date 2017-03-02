#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <algorithm>

using namespace std;


double eDist(vector<double> v1, vector<double> v2) {    //Euclidean distance
    double dist = 0;
    for (int i = 0; i < v2.size(); ++i) {
        dist += (v1[i] - v2[i]) * (v1[i] - v2[i]);
    }

    return dist;
}


vector<vector<double>> calcCentroid(vector<vector<double>> values, vector<vector<int>> group) { //returns centroid vector
    vector<vector<double>> centroid;
    for (int i = 0; i < group.size(); ++i) {
        vector<double> v;
        v.resize(values[0].size());
        for (int j = 0; j < group[i].size(); ++j) {
            for (int k = 0; k < values[group[i][j]].size(); ++k) {
                v[k] += values[group[i][j]][k];
            }
        }
        for (int k = 0; k < v.size(); ++k) {
            v[k] /= group[i].size();
        }
        centroid.push_back(v);
    }
    return centroid;
}


void display(vector<vector<int>> group, vector<vector<double>> centroid) { //display table of clusters, members and centroids
    cout << "\nCluster\tMembers";
    int size = 0;
    for (int i = 0; i < group.size(); ++i) {
        if (group[i].size() > size) {
            size = group[i].size();
        }
    }
    for (int i = 0; i < size; ++i) {
        cout << '\t';
    }
    cout << "Centroid\n";

    for (int i = 0; i < group.size(); ++i) {
        cout << i << '\t';
        for (int j = 0; j < group[i].size(); ++j) {
            cout << group[i][j] << '\t';
        }
        for (int j = 0; j < size - group[i].size(); ++j) {
            cout << '\t';
        }
        //cout << "\t|\t";
        for (int j = 0; j < centroid[i].size(); ++j) {
            cout << centroid[i][j] << '\t';
        }
        cout << endl;
    }
}


int main(int argc, char const *argv[])
{
    if (argc != 3) {
        cout << "Exepcting 2 arguments!\nUsage: a.out [file.csv] [number_of_clusters]\n";
        return 1;
    }

    ifstream file(argv[1]); //the file
    string line;    //each line
    vector<vector<double>> values;  //the scraped values
    //vector<vector<double>> clusters;    //the generated clusters
    vector<int> visited; //marks clustered tuples
    int n = stol(argv[2]);  //no. of clusters

    while (getline(file, line)) {   //scraping the csv
        stringstream lstream(line);
        string word;
        vector<double> v;
        while (getline(lstream, word, ',')) {
            v.push_back(stod(word));
        }
        values.push_back(v);
    }

    cout << "\nTuples:\n";
    for (int i = 0; i < values.size(); ++i) {   //display the values
        for (int j = 0; j < values[i].size(); ++j) {
            cout << values[i][j] << '\t';
        }
        cout << endl;
    }

    double dist = 0;
    vector<vector<int>> group;  //the generated clusters
    int gg1 = 0, gg2 = 1;
    for (int i = 0; i < values.size(); ++i) {   //2 farthest points
        vector<double> v1 = values[i];
        for (int j = i + 1; j < values.size(); ++j) {
            vector<double> v2 = values[j];
            double cdist = eDist(v1, v2);
            if (cdist > dist) { //replace
                dist = cdist;
                gg1 = i;
                gg2 = j;
            }
        }
    }
    vector<int> g1, g2;
    g1.push_back(gg1);
    g2.push_back(gg2);
    group.push_back(g1);
    group.push_back(g2);
    visited.push_back(gg1);
    visited.push_back(gg2);

    if (n > 2) {
        int ggj = 2;
        for (int i = 2; i < n; ++i) {   //remaining groups
            dist = 0;
            for (int j = 0; j < values.size(); ++j) {   //all tuples
                double cdist = 0;
                for (int k = 0; k < group.size(); ++k) {
                    if (group[k][0] == j) {  //repeat
                        //cout << "\nSkip" << j;
                        cdist = numeric_limits<int>::min();
                        break;
                    }
                    cdist += eDist(values[j], values[group[k][0]]); //sum of distance from selected points
                }
                if (cdist > dist) { //replace
                    dist = cdist;
                    ggj = j;
                }
            }
            vector<int> g;
            g.push_back(ggj);
            group.push_back(g);
            visited.push_back(ggj);
        }
    }

    /*for (int i = 0; i < group.size(); ++i) {   //display the values
        for (int j = 0; j < group[i].size(); ++j) {
            cout << group[i][j] << '\t';
        }
        cout << endl;
    }*/

    vector<vector<double>> centroid = calcCentroid(values, group); //get centroids

    cout << "\nClustering...\n\nStep 1:\n";
    display(group, centroid); //display table

    /*for (int i = 0; i < visited.size(); ++i) {   //display the values
        cout << visited[i] << '\t';
    }*/

    for (int i = 0; i < values.size(); ++i) {   //clustering, iterate over each tuple
        dist = numeric_limits<int>::max();
        int gg = -1;
        int flag = 1;
        for (int j = 0; j < visited.size(); ++j) {  //check visited
            if (visited[j] == i) {  //repeat
                //cout << "\nSkip" << i;
                flag = 0;
                break;
            }
        }

        if (flag) { //new item
            for (int j = 0; j < centroid.size(); ++j) { //iterate over each group/centroid
                //cout << "\nCompare" << i;
                double cdist = eDist(values[i], centroid[j]);
                if (cdist < dist) {
                    dist = cdist;
                    gg = j;
                }

            }
            group[gg].push_back(i);
            visited.push_back(i);
            centroid = calcCentroid(values, group);
            cout << "\nStep " << (i + 1) << ":\n";
            display(group, centroid);
        }
    }

    cout << "\nRelocating...\n";
    int count = 1;
    const int MAX_ATTEMPTS = 10;    //max iterations
    while (count <= MAX_ATTEMPTS) { //iterative relocation
        int flag = 1;
        for (int i = 0; i < group.size(); ++i) {    //verification
            for (int j = 0; j < group[i].size(); ++j) {
                double odist = eDist(values[group[i][j]], centroid[i]); //distance with own centroid
                int ggi = -1;
                for (int k = 0; k < centroid.size(); k++) { //distance with other centroids
                    if (k == i) { //same group
                        continue;
                    }
                    double dist = eDist(values[group[i][j]], centroid[k]);
                    if (dist < odist) {
                        odist = dist;
                        ggi = k;
                    }
                }
                if (ggi != -1) { //found better group
                    group[ggi].push_back(group[i][j]);
                    group[i].erase(group[i].begin() + j);
                    j--;
                    flag = 0;
                }
            }
        }
        for (int i = 0; i < group.size(); ++i) {    //sorting
            sort(group[i].begin(), group[i].end());
        }
        centroid = calcCentroid(values, group);
        cout << "\nAttempt " << count << ":\n";
        display(group, centroid);
        if (flag) {
            cout << "\nNo change\n";
            break;
        }
        count++;
    }

    return 0;
}
