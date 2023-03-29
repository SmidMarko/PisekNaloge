/*
  Return an integer uniformly drawn from range [mini,maxi]
*/
int rand_int(int mini, int maxi) {
    return rand()%(maxi-mini+1) + mini;
}

float rand_float(float mini, float maxi) {
    float x = (float)rand() / (float)RAND_MAX;
    return mini + x*(maxi-mini);
}

bool rand_bernoulli(float successProba) {
    return (float)rand() < successProba*RAND_MAX;
}

/*
  If s > 0, return the vector [a,a+s,...,a+ks] with a+ks <= b < a+(k+1)s
  If s < 0, return the vector [a,a+s,...,a+ks] with a+ks >= b > a+(k+1)s
  If s == 0, return an empty vector.
*/
template<typename T>
vector<T> range(T a, T b, T s = 1) {
    if(s == 0)
	return vector<T>();
    vector<T> res;
    T cur = a;
    if(s > 0) {
	while(cur <= b) {
	    res.push_back(cur);
	    cur += s;
	}
    } else {
	while(cur >= b) {
	    res.push_back(cur);
	    cur +=s;
	}
    }
    return res;
}


/*
  Return the result of std::random_shuffle on the input vector (which
  is given by value)
*/
template<typename T>
vector<T> rand_perm(vector<T> v) {
    std::random_shuffle(v.begin(),v.end());
    return v;
}
