/**
 * A C++ WAND top-k retrieval implementation by Andrew Zhang.
 * 7/30/2018
 */
#include <vector>
#include <unordered_map>
#include <string>   
#include <utility>
#include <queue>
#include <algorithm>
using namespace std;

#ifndef WAND_H_
#define WAND_H_

class WAND {
    public:
        WAND(unordered_map<string, vector<pair<int, int>>> & postingList);
        void init(vector<string> & query_terms, int k, int F, vector<string> & must_include);
        void sortTerms();
        int findPivotTerm();   
        void nextID(int change_index, int docID); //advance the term iterators i.e. currentInvertedIndex
        int next();
        void insertHeap(int docID, int score);
        int fullScore(int docID);
        vector<pair<int,int>> DoQuery(vector<string> & query_terms, int k, int F = 1, vector<string> must_include = vector<string>());
        int getFullEvaluationCount();

    private:
        //Holds the upper bound score of each posting list. hash map->[term, UB]
        unordered_map<string, int> UB;
        /**
         * Holds the current DIDs in each of the posting lists in InvertedIndex.
         * Each term has iterator to traverse the vector of pairs of [DID, score] in InvertedIndex.
         */
        unordered_map<string, vector<pair<int, int>>::iterator> currentInvertedIndex;  //iterator going through [DID, score]
        /**
         * The InvertedIndex to query from. maps terms to vector of pairs of [DID, score]
         */
        unordered_map<string, vector<pair<int, int>>> InvertedIndex; //[DID, score]
        /**
         * Stores the current DID for each term from currentInvertedIndex and sorts the terms 
         * based on their current DID. Vector of pairs of [DID, term]. Used to find pivot term.
         */ 
        vector<pair<int, string>> sortedTerms; //[DID, term]
        /**
         * The heap that contains the top-k documents. 
         * Minimum priority queue with vector as its container.
         * Stores pairs of [score, DID], min heap based on score of documents.
         */
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> top_k;  //[score, DID]
        
        //previous valid pivot ID.
        int curDoc;

        int threshold = -1;
        
        //the last DID, a really big number. Stop when the pivot ID is this.
        int lastID = 2000000000;
        //Number of results to return. The k in top-k.
        int result_num;
        //Vary this to vary the speed of query at the expense of accuracy.
        int thresholdFactor;
        int fullEvaluationCount = 0;

};

#endif  

