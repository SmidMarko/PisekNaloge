/*
  Return a vector of length 2*nbPairs, where each integer in [0,nbPairs) appears
  twice, defining a balanced list of parentheses uniformly drawn.
*/
vector<int> rand_parentheses(int nbPairs) {
    // we use the cyclic lemma
    vector<int> signs = vector<int>(nbPairs,1);
    vector<int> minus(nbPairs+1,-1);
    signs.insert(signs.end(),minus.begin(),minus.end());
    random_shuffle(signs.begin(),signs.end());

    vector<int> cum(2*nbPairs+1);
    partial_sum(signs.begin(),signs.end(),cum.begin());
    int iMin = min_element(cum.begin(),cum.end()) - cum.begin();
    rotate(signs.begin(),signs.begin()+iMin+1,signs.end());
    signs.pop_back();

    vector<int> res;
    vector<int> idStack;
    int cur = 0;
    for(int i = 0; i < 2*nbPairs; i++) {
	if(signs[i] == 1) {
	    idStack.push_back(cur);
	    cur++;
	    res.push_back(idStack.back());
	} else {
	    res.push_back(idStack.back());
	    idStack.pop_back();
	}
    }
    return res;
}
