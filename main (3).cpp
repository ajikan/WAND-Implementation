#include "wand.h"
#include <iostream>
#include <chrono>
using namespace std;

int main() {
    unordered_map<string, vector<pair<int, int>>> postingList;
    postingList["t0"] = {{1, 1}, {2, 5}, {3, 1}, {4, 3}, {5, 2}, {6, 11}, {7, 4}, {9, 10}, {10, 3}, {11, 4}, {12, 2}, {13, 2}, {14, 4}, {15, 6}, {16, 5}, {17, 9}, {18, 6}, {19, 2}, {20, 5}, {21, 5}, {22, 3}, {23, 3}, {24, 3}, {25, 6}, {27, 9}, {28, 4}, {29, 11}, {30, 8}, {31, 2}, {32, 4}, {33, 5}, {34, 4}, {35, 2}, {36, 5}, {37, 6}, {38, 4}, {39, 9}, {40, 2}, {2000000000, -1}};
    postingList["t1"] = {{1, 1}, {3, 2}, {6, 10}, {5, 6}, {8, 9}, {9, 8}, {10, 3}, {12, 3}, {13, 4}, {14, 5}, {16, 7}, {18, 4}, {19, 7}, {20, 5}, {21, 3}, {22, 1}, {23, 4}, {25, 6}, {26, 4}, {27, 8}, {28, 3}, {30, 9}, {31, 2}, {32, 4}, {33, 2}, {34, 5}, {35, 4}, {36, 7}, {37, 8}, {38, 9}, {40, 3}, {2000000000, -1}};
    postingList["t2"] = {{1, 2}, {2, 1}, {3, 1}, {4, 2}, {6, 10}, {7, 12}, {12, 9}, {13, 10}, {14, 11}, {15, 7}, {16, 5}, {17, 3}, {18, 7}, {19, 4}, {20, 4}, {22, 3}, {24, 3}, {25, 8}, {26, 5}, {27, 8}, {31, 2}, {32, 2}, {34, 5}, {35, 6}, {36, 7}, {37, 5}, {38, 9}, {39, 9}, {40, 8}, {2000000000, -1}};
    postingList["t3"] = {{1, 4}, {2, 2}, {13, 10}, {14, 11}, {15, 7}, {17, 3}, {18, 7}, {19, 4}, {20, 4}, {21, 3}, {29, 9}, {30, 10}, {31, 8}, {32, 3}, {33, 5}, {34, 7}, {37, 3}, {38, 3}, {39, 9}, {40, 8}, {2000000000, -1}};

    WAND w(postingList);
    vector<string> query_term = {"t0", "t1", "t2", "t3"};
    vector<string> must_include = {"t0", "t1", "t2"};
    vector<pair<int, int>> result;
    auto begin = chrono::steady_clock::now();
    result = w.DoQuery(query_term, 3, 0, must_include);
    auto end = chrono::steady_clock::now();
    for (auto & p : result) {
        cout<< p.first << ", " << p.second <<endl;
    }
    cout << "Number of full evaluations(F=0): " << w.getFullEvaluationCount() << endl;
    cout << "Time(nanoseconds): " << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() << endl;

    begin = chrono::steady_clock::now();
    result = w.DoQuery(query_term, 3, 1, must_include);
    end = chrono::steady_clock::now();
    for (auto & p : result) {
        cout<< p.first << ", " << p.second <<endl;
    }
    cout << "Number of full evaluations(F=1): " << w.getFullEvaluationCount() << endl;
    cout << "Time(nanoseconds): " << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() << endl;

    begin = chrono::steady_clock::now();
    result = w.DoQuery(query_term, 3, 2, must_include);
    end = chrono::steady_clock::now();
    for (auto & p : result) {
        cout<< p.first << ", " << p.second <<endl;
    }
    cout << "Number of full evaluations(F=2): " << w.getFullEvaluationCount() << endl;
    cout << "Time(nanoseconds): " << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() << endl;

    begin = chrono::steady_clock::now();
    result = w.DoQuery(query_term, 3, 3, must_include);
    end = chrono::steady_clock::now();
    for (auto & p : result) {
        cout<< p.first << ", " << p.second <<endl;
    }
    cout << "Number of full evaluations(F=3): " << w.getFullEvaluationCount() << endl;
    cout << "Time(nanoseconds): " << chrono::duration_cast<chrono::nanoseconds>(end - begin).count() << endl;
}

