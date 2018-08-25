#include "wand.h"
using namespace std;

/**
 * Initializes the WAND object. 
 * Initializes the InvertedIndex to query from and also the UB
 * of each posting list.
 */
WAND::WAND(unordered_map<string, vector<pair<int, int>>> & postingList) {
    InvertedIndex = postingList;

    //Initialize UB for each term.
    for (auto & pair : postingList) {
        int maxScore = 0;
        for (auto & document : pair.second) {
            if (document.second > maxScore) {
                maxScore = document.second;
            }
        }
        UB[pair.first] = maxScore;
    }
}

/**
 * Initializes the WAND iterator, sets all the variables to default values,
 * clears currentInvertedIndex, sortedTerms, top_k heap. Initializes 
 * currentInvertedIndex iterators to first document in the InvertedIndex.
 */
void WAND::init(vector<string> & query_terms, int k, int F, vector<string> & must_include) {
    curDoc = -1;
    threshold = 0;
    thresholdFactor = F;
    fullEvaluationCount = 0;
    result_num = k;
    top_k = priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>>();
    currentInvertedIndex.clear();
    sortedTerms.clear();
    for (auto & term : query_terms) {
        currentInvertedIndex[term] = InvertedIndex[term].begin();
    }

    if (!must_include.empty()) {
        for (auto & term : must_include) {
            for (auto & document : InvertedIndex[term]) {
                document.second += 500;
            }
            UB[term] += 500;
            threshold += UB[term];
        }
    }
}

//Returns the full evaluation count for this particular WAND run.
int WAND::getFullEvaluationCount() {
    return fullEvaluationCount;
}   

/**
 * Sorts the current DIDs in the increasing order. 
 * Sorts [DID, term] pairs in the sortedTerms vector.
 */
void WAND::sortTerms() {
    //If sortedTerms is empty initialize it. I believe this can go in init method.
    if (sortedTerms.size() == 0) {
        for (auto & elem : currentInvertedIndex) {
            sortedTerms.push_back(make_pair(elem.second->first, elem.first));  //[DID, term]
        }
    }
    sort(sortedTerms.begin(), sortedTerms.end()); //sort based on DID.
}   

/**
 * Finds pivot term by summing the UB of terms in the sortedTerms order.
 * Returns the index of the pivot term in sortedTerms.
 * Returns -1 if pivot term was not found.
 */
int WAND::findPivotTerm() {
    int UBscore = 0;
    for (size_t i = 0; i < sortedTerms.size(); i++) {
        UBscore += UB[sortedTerms[i].second];
        if (UBscore >= threshold) {
            return i;
        }
    }
    return -1;
}

/**
 * Advances the iterator of a term to >= docID.
 * change_index is the index of the term in sortedTerms.
 */
void WAND::nextID(int change_index, int docID) {
    string change_term = sortedTerms[change_index].second;
    while (currentInvertedIndex[change_term]->first < docID) {
        currentInvertedIndex[change_term]++;
    }   
    //Update the DID of the change_term in sortedTerms list.
    sortedTerms[change_index].first = currentInvertedIndex[change_term]->first;
}

/**
 * WAND iterator that returns the next valid candidate to be in top-k.
 * Returns next document that has UB(d, q) >= threshold.
 */
int WAND::next() {
    while (true) {
        //sorts the term lists based on current DID in the lists.
        sortTerms();
        //Finds the pivot term in sortedTerms and returns the index.
        int pivot_index = findPivotTerm();

        if (pivot_index == -1) {
            return -1;
        }
        string pivot_term = sortedTerms[pivot_index].second;
        int pivot_DID = currentInvertedIndex[pivot_term]->first; //or sortedTerms[pivot_index].first
        
        if (pivot_DID == lastID) {
            return -1;
        }
        
        if (pivot_DID <= curDoc) {
            int change_index = 0;
            nextID(change_index, curDoc + 1);
        }
        else {
            int first_DID = sortedTerms[0].first;
            if (first_DID == pivot_DID) {
                curDoc = pivot_DID;
                return curDoc;
            }
            else {
                //Advance all of the iterators above pivot term to >= pivotID
                for (int i = 0; i < pivot_index; i++) {
                    nextID(i, pivot_DID);
                }
            }
        }
    }
}

/**
 * Inserts the docID and its score as pair into the heap.
 * If heap is not full, insert it without comparing with 
 * minimum in top-k. If heap is full, compare with minimum and 
 * update threshold if inserted.
 */
void WAND::insertHeap(int docID, int score) {
    if (top_k.size() < result_num) {
        top_k.push(make_pair(score, docID));

        //once the heap is full, update threshold from -1.
        if (top_k.size() == result_num) {
            threshold = thresholdFactor * top_k.top().first;
        }
    }
    else {
        if (score > top_k.top().first) {
            top_k.pop();
            top_k.push(make_pair(score, docID));
            threshold = thresholdFactor * top_k.top().first;
        }
    }
}

/**
 * Computes full score of a document.
 * Makes use of the fact that the first couple DID of sortedTerms are the 
 * same.
 */
int WAND::fullScore(int docID) {
    int score = 0;
    fullEvaluationCount++; //Keep track of full evaluation count.
    for (auto & p : sortedTerms) {
        if (p.first != docID) {
            break;
        }
        score += currentInvertedIndex[p.second]->second;
    }
    return score;
}

/**
 * The overall process. Initializes the query and calls
 * next() repeatedly to get candidates to be in top-k and then 
 * computes their full score to see if they belong in top-k.
 * You can specify must include terms in the resulting documents
 * You can also specify the threshold factor F.
 */
vector<pair<int, int>> WAND::DoQuery(vector<string> & query_terms, int k, int F, vector<string> must_include) {
    init(query_terms, k, F, must_include);

    while (true) {
        int candidate_DID = next();
        if (candidate_DID == -1) {
            break;
        }

        int full_doc_score = fullScore(candidate_DID);
        insertHeap(candidate_DID, full_doc_score);
    }

    if (!must_include.empty()) {
        for (auto & term : must_include) {
            for(auto & document : InvertedIndex[term]) {
                document.second -= 500;
            }
            UB[term] -= 500;
        }
    }

    //Returns the top-k documents as vector of pairs of [DID, score].
    vector<pair<int, int>> resultList;
    while (!top_k.empty()) {
        resultList.push_back(make_pair(top_k.top().second, top_k.top().first));
        top_k.pop();
    }
    return resultList;
}
