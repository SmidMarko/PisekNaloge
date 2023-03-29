/*
  Rand uniformly a spanning tree of the complete graph on vertex
  set {0,1,..,n-1}. The result is given as a list of edges.
*/
vector< pair<int,int> > rand_tree(int nbNodes) {
    // we use Prufer encoding
    vector<int> pruferCode;
    for(int i = 0; i < nbNodes-2; i++)
	pruferCode.push_back(rand_int(0,nbNodes-1));
    pruferCode.push_back(nbNodes-1);

    vector< pair<int,int> > resEdges;
    vector<int> nbChildren(nbNodes,0);
    for(int i = 0; i < nbNodes-1; i++) {
	nbChildren[pruferCode[i]]++;
    }
    int posInCode = 0;
    for(int idNode = 0; idNode < nbNodes-1; idNode++) {
	int idCur = idNode;
	while(idCur <= idNode && nbChildren[idCur] == 0) {
	    int idFather = pruferCode[posInCode];
	    posInCode++;
	    resEdges.push_back(make_pair(idCur,idFather));
	    nbChildren[idFather]--;
	    idCur = idFather;
	}
    }
    return resEdges;
}


/*
  Rand uniformly a spanning tree of the graph whose number of vertices
  and edge list are given in input (we assume that the graph is connected
  and that the vertices are numbered  from 0 to nbNodes-1).
  The result is given as a list of edges.
  Remark : quite slow for big graphs.
*/
vector< pair<int,int> > rand_spanning_tree(
    int nbNodes, vector< pair<int,int> > edges) {
    // we use Wilson's algorithm
    if(nbNodes <= 1)
	return vector< pair<int,int> >();

    vector< vector<int> > neigh(nbNodes, vector<int>());
    for(int iEdge = 0; iEdge < (int)edges.size(); iEdge++) {
	neigh[edges[iEdge].first].push_back(edges[iEdge].second);
	neigh[edges[iEdge].second].push_back(edges[iEdge].first);
    }

    vector<bool> marked(nbNodes,false);
    marked[rand_int(0,nbNodes-1)] = true;
    int nbMarkedNodes = 1;
    vector<int> suiv(nbNodes);
    vector< pair<int,int> > resEdges;

    while(nbMarkedNodes < nbNodes) {
	int idStart = rand_int(0,nbNodes-1);
	int idCur = idStart;
	while(!marked[idCur]) {
	    int idNeigh = neigh[idCur][rand()%neigh[idCur].size()];
	    suiv[idCur] = idNeigh;
	    idCur = idNeigh;
	}
	idCur = idStart;
	while(!marked[idCur]) {
	    marked[idCur] = true;
	    nbMarkedNodes++;
	    resEdges.push_back(make_pair(idCur,suiv[idCur]));
	    idCur = suiv[idCur];
	}
    }

    return resEdges;
}


/*
  Return the spanning tree obtained by a randomized DFS on the graph defined
  by its number of vertices and its edge list, starting the search from a given
  start vertex (we assume that the graph is connected and that the vertices are
  numbered from 0 to nbNodes-1). The result is given as a list of edges.
*/
vector< pair<int,int> > rand_dfs_tree(
    int nbNodes, vector< pair<int,int> > edges, int idStart) {
    
    vector< vector<int> > neigh(nbNodes, vector<int>());
    for(int iEdge = 0; iEdge < (int)edges.size(); iEdge++) {
	neigh[edges[iEdge].first].push_back(edges[iEdge].second);
	neigh[edges[iEdge].second].push_back(edges[iEdge].first);
    }
    for(int idNode = 0; idNode < nbNodes; idNode++)
	random_shuffle(neigh[idNode].begin(),neigh[idNode].end());

    vector< pair<int,int> > resEdges;
    vector<int> idNodeStack;
    vector<int> iNeighStack;
    vector<bool> marked(nbNodes,false);
    idNodeStack.push_back(idStart);
    iNeighStack.push_back(0);
    marked[idStart] = true;

    while(!idNodeStack.empty()) {
	int idNodeCur = idNodeStack.back();
	int iNeighCur = iNeighStack.back();
	if(iNeighCur == (int)neigh[idNodeCur].size()) {
	    idNodeStack.pop_back();
	    iNeighStack.pop_back();
	    continue;
	}
	int idNeigh = neigh[idNodeCur][iNeighCur];
	iNeighStack.back()++;
	if(!marked[idNeigh]) {
	    marked[idNeigh] = true;
	    idNodeStack.push_back(idNeigh);
	    iNeighStack.push_back(0);
	    resEdges.push_back(make_pair(idNodeCur,idNeigh));
	}
    }
    return resEdges;
}


/*
  Return the spanning tree obtained by a randomized BFS on the graph defined
  by its number of vertices and its edge list, starting the search from a given
  start vertex (we assume that the graph is connected and that the vertices are
  numbered from 0 to nbNodes-1). The result is given as a list of edges.
*/
vector< pair<int,int> > rand_bfs_tree(
    int nbNodes, vector< pair<int,int> > edges, int idStart) {
    
    vector< vector<int> > neigh(nbNodes, vector<int>());
    for(int iEdge = 0; iEdge < (int)edges.size(); iEdge++) {
	neigh[edges[iEdge].first].push_back(edges[iEdge].second);
	neigh[edges[iEdge].second].push_back(edges[iEdge].first);
    }
    for(int idNode = 0; idNode < nbNodes; idNode++)
	random_shuffle(neigh[idNode].begin(),neigh[idNode].end());

    vector< pair<int,int> > resEdges;
    queue<int> idNodeQueue;
    vector<bool> marked(nbNodes,false);
    idNodeQueue.push(idStart);
    marked[idStart] = true;
    while(!idNodeQueue.empty()) {
	int idNodeCur = idNodeQueue.front();
	idNodeQueue.pop();
	for(int iNeigh = 0; iNeigh < (int)neigh[idNodeCur].size(); iNeigh++) {
	    int idNeigh = neigh[idNodeCur][iNeigh];
	    if(!marked[idNeigh]) {
		marked[idNeigh] = true;
		resEdges.push_back(make_pair(idNodeCur,idNeigh));
		idNodeQueue.push(idNeigh);
	    }
	}
    }
    return resEdges;
}


/*
  Return the spanning tree obtained by a randomized Dijkstra on the graph defined
  by its number of vertices and its edge list, starting the search from a given
  start vertex (we assume that the graph is connected and that the vertices are
  numbered from 0 to nbNodes-1). The result is given as a list of edges.
*/
vector< pair<int,int> > rand_dijkstra_tree(
    int nbNodes, vector< pair<int,int> > edges, int idStart) {
    
    vector< vector<int> > neigh(nbNodes, vector<int>());
    for(int iEdge = 0; iEdge < (int)edges.size(); iEdge++) {
	neigh[edges[iEdge].first].push_back(edges[iEdge].second);
	neigh[edges[iEdge].second].push_back(edges[iEdge].first);
    }
    for(int idNode = 0; idNode < nbNodes; idNode++)
	random_shuffle(neigh[idNode].begin(),neigh[idNode].end());

    vector< pair<int,int> > resEdges;

    const int INVALID = -1;
    priority_queue< pair< float, pair<int,int> > > pq;
    vector< bool > visited(nbNodes,false);
    pq.push(make_pair(0.f, make_pair(idStart,INVALID)));
    while(!pq.empty()) {
	pair< float, pair<int,int> > cur = pq.top();
	pq.pop();
	int idNode = cur.second.first;
	if(visited[idNode])
	    continue;
	visited[idNode] = true;
	float curDist = -cur.first;
	int idFather = cur.second.second;
	if(idFather!=INVALID)
	    resEdges.push_back(make_pair(idFather,idNode));
	for(int iNeigh = 0; iNeigh < (int)neigh[idNode].size(); iNeigh++) {
	    int idNeigh = neigh[idNode][iNeigh];
	    float newDist = curDist+rand_float(0.f,1.f);
	    pq.push(make_pair(-newDist, make_pair(idNeigh,idNode)));	    
	}
    }
    
    return resEdges;
}


/*
  Return the spanning tree obtained by a randomized Prim-like search on the
  graph defined  by its number of vertices and its edge list, starting the search
  from a given  start vertex (we assume that the graph is connected and that
  the vertices are numbered from 0 to nbNodes-1). The result is given as a list
  of edges.
*/
vector< pair<int,int> > rand_prim_tree(
    int nbNodes, vector< pair<int,int> > edges, int idStart) {
    
    vector< vector<int> > neigh(nbNodes, vector<int>());
    for(int iEdge = 0; iEdge < (int)edges.size(); iEdge++) {
	neigh[edges[iEdge].first].push_back(edges[iEdge].second);
	neigh[edges[iEdge].second].push_back(edges[iEdge].first);
    }
    for(int idNode = 0; idNode < nbNodes; idNode++)
	random_shuffle(neigh[idNode].begin(),neigh[idNode].end());

    const int INVALID = -1;
    vector< pair<int,int> > resEdges;
    vector< pair<int,int> > idActiveAndIdFatherNodes;
    vector<bool> marked(nbNodes,false);
    idActiveAndIdFatherNodes.push_back(make_pair(idStart,INVALID));
    while(!idActiveAndIdFatherNodes.empty()) {
	int iRand = rand_int(0, (int)idActiveAndIdFatherNodes.size()-1);
	swap(idActiveAndIdFatherNodes[iRand],idActiveAndIdFatherNodes.back());
	int idNode = idActiveAndIdFatherNodes.back().first;
	int idFather = idActiveAndIdFatherNodes.back().second;
	idActiveAndIdFatherNodes.pop_back();

	if(marked[idNode])
	    continue;
	marked[idNode] = true;
	if(idFather != INVALID)
	    resEdges.push_back(make_pair(idFather,idNode));
	
	for(int iNeigh = 0; iNeigh < (int)neigh[idNode].size(); iNeigh++) {
	    int idNeigh = neigh[idNode][iNeigh];
	    if(!marked[idNeigh]) 
	        idActiveAndIdFatherNodes.push_back(make_pair(idNeigh,idNode));
	}
    }
    return resEdges;
}



/*
  Return the spanning tree obtained by a hunt and kill process on the
  graph defined  by its number of vertices and its edge list, starting the search
  from a given  start vertex (we assume that the graph is connected and that
  the vertices are numbered from 0 to nbNodes-1). The result is given as a list
  of edges.
*/
vector< pair<int,int> > rand_hunt_and_kill_tree(
    int nbNodes, vector< pair<int,int> > edges, int idStart) {
    
    vector< vector<int> > neigh(nbNodes, vector<int>());
    for(int iEdge = 0; iEdge < (int)edges.size(); iEdge++) {
	neigh[edges[iEdge].first].push_back(edges[iEdge].second);
	neigh[edges[iEdge].second].push_back(edges[iEdge].first);
    }
    for(int idNode = 0; idNode < nbNodes; idNode++)
	random_shuffle(neigh[idNode].begin(),neigh[idNode].end());

    const int INVALID = -1;
    vector< pair<int,int> > resEdges;
    vector< pair<int,int> > idActiveAndIdFatherNodes;
    vector<bool> marked(nbNodes,false);
    idActiveAndIdFatherNodes.push_back(make_pair(idStart,INVALID));
    while(!idActiveAndIdFatherNodes.empty()) {
	int iRand = rand_int(0, (int)idActiveAndIdFatherNodes.size()-1);
	swap(idActiveAndIdFatherNodes[iRand],idActiveAndIdFatherNodes.back());
	int idNode = idActiveAndIdFatherNodes.back().first;
	int idFather = idActiveAndIdFatherNodes.back().second;
	idActiveAndIdFatherNodes.pop_back();

	if(marked[idNode])
	    continue;
	marked[idNode] = true;
	if(idFather != INVALID)
	    resEdges.push_back(make_pair(idFather,idNode));

	while(true) {
	    marked[idNode] = true;
	    int idNext = idNode;
	    for(int iNeigh = 0; iNeigh < (int)neigh[idNode].size(); iNeigh++) {
		int idNeigh = neigh[idNode][iNeigh];
		if(!marked[idNeigh]) {
		    idActiveAndIdFatherNodes.push_back(make_pair(idNeigh,idNode));
		    idNext = idNode;
		}
	    }
	    if(idNext == idNode)
		break;
	    resEdges.push_back(make_pair(idNode,idNext));
	    idNode = idNext;
	}
	/*
	for(int iNeigh = 0; iNeigh < (int)neigh[idNode].size(); iNeigh++) {
	    int idNeigh = neigh[idNode][iNeigh];
	    if(!marked[idNeigh]) 
	        idActiveAndIdFatherNodes.push_back(make_pair(idNeigh,idNode));
	}
	*/
    }
    return resEdges;
}


/*
  Return the spanning tree obtained through a randomized Kruskal-like process on the
  graph defined  by its number of vertices and its edge list (we assume that the
  graph is connected and that the vertices are numbered from 0 to nbNodes-1).
  The result is given as a list of edges.
*/
vector< pair<int,int> > rand_kruskal_tree(
    int nbNodes, vector< pair<int,int> > edges) {

    vector< pair<int,int> > resEdges;
    vector<int> idGroup = range(0,nbNodes-1);
    vector< vector<int> > elementOfGroup(nbNodes,vector<int>());
    for(int idNode = 0; idNode < nbNodes; idNode++)
	elementOfGroup[idNode].push_back(idNode);
    
    int maxGroupSize = 1;
    while(maxGroupSize < nbNodes && !edges.empty()) {
	int iEdge = rand_int(0,(int)edges.size()-1);
	swap(edges[iEdge],edges.back());
	pair<int,int> edge = edges.back();
	edges.pop_back();
	
	int iGroup1 = idGroup[edge.first], iGroup2 = idGroup[edge.second];
	if(iGroup1 == iGroup2)
	    continue;
	
	resEdges.push_back(edge);
	if(elementOfGroup[iGroup1].size() > elementOfGroup[iGroup2].size())
	    swap(iGroup1,iGroup2);
	vector<int> & g1 = elementOfGroup[iGroup1];
	vector<int> & g2 = elementOfGroup[iGroup2];
	for(int i = 0; i < (int)g1.size(); i++) {
	    int idNode = g1[i];
	    g2.push_back(idNode);
	    idGroup[idNode] = iGroup2;
	}
	g1.clear();
	if((int)g2.size() > maxGroupSize)
	    maxGroupSize = (int)g2.size();
    }

    return resEdges;
}


/*
  Return the spanning tree obtained through a recursive split process on the
  graph defined  by its number of vertices and its edge list (we assume that the
  graph is connected and that the vertices are numbered from 0 to nbNodes-1).
  We split the graph into two connected parts (using two  competitive BFS from
  two random seeds), then we choose randomly an edge between the two  parts and
  we process each of the two parts recursively.
  The result is given as a list of edges.
*/

vector< pair<int,int> > rand_rec_split_tree(
    int nbNodes, vector< pair<int,int> > edges) {

    vector< vector<int> > neigh(nbNodes, vector<int>());
    for(int iEdge = 0; iEdge < (int)edges.size(); iEdge++) {
	neigh[edges[iEdge].first].push_back(edges[iEdge].second);
	neigh[edges[iEdge].second].push_back(edges[iEdge].first);
    }
    for(int idNode = 0; idNode < nbNodes; idNode++)
	random_shuffle(neigh[idNode].begin(),neigh[idNode].end());
    
    vector< pair<int,int> > resEdges;
    vector< vector<int> > parts(1, range(0,nbNodes-1));
    vector<int> idPart(nbNodes,0);
    while(!parts.empty()) {
	vector<int> idNodes = parts.back();
	parts.pop_back();
	if(idNodes.size() <= 1)
	    continue;
	int iRand1 = rand_int(0,(int)idNodes.size()-1);
	int iRand2;
	do { iRand2 = rand_int(0,(int)idNodes.size()-1); } while (iRand1 == iRand2);
	int idNode1 = idNodes[iRand1], idNode2 = idNodes[iRand2];

        // we use two concurrent BFS to split into two connected subgraphs
	vector<int> newPart1, newPart2;
	for(int iNode = 0; iNode < (int)idNodes.size(); iNode++)
	    idPart[idNodes[iNode]] = -1;
	idPart[idNode1] = 1;
	idPart[idNode2] = 2;
	queue<int> nodeQueue;
	nodeQueue.push(idNode1);
	nodeQueue.push(idNode2);
	while(!nodeQueue.empty()) {
	    int idNode = nodeQueue.front();
	    nodeQueue.pop();
	    if(idPart[idNode] == 1)
		newPart1.push_back(idNode);
	    else
		newPart2.push_back(idNode);

	    for(int iNeigh = 0; iNeigh < (int)neigh[idNode].size(); iNeigh++) {
		int idNeigh = neigh[idNode][iNeigh];
		if(idPart[idNeigh] == -1) {
		    idPart[idNeigh] = idPart[idNode];
		    nodeQueue.push(idNeigh);
		}
	    }
	}
	
	// Then we remove all the edges between the two parts
	// and choose randomly one of them
	vector< pair<int,int> > transverseEdges;
	for(int iNode = 0; iNode < (int)idNodes.size(); iNode++) {
	    int idNode = idNodes[iNode];
	    for(int iNeigh = 0; iNeigh < (int)neigh[idNode].size(); iNeigh++) {
		int idNeigh = neigh[idNode][iNeigh];
		if(idPart[idNeigh] != idPart[idNode]) {
		    transverseEdges.push_back(make_pair(idNode,idNeigh));
		    swap(neigh[idNode][iNeigh], neigh[idNode].back());
		    neigh[idNode].pop_back();
		}
	    }
	}
	int iTransverseEdge = rand_int(0,(int)transverseEdges.size()-1);
	resEdges.push_back(transverseEdges[iTransverseEdge]);

	parts.push_back(newPart1);
	parts.push_back(newPart2);
    }
    
    return resEdges;
}
