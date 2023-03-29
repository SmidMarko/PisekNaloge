/*
  Private.
*/
vector< pair<int,int> > maze_edges(int nbLines, int nbCols) {
    vector< pair<int,int> > resEdges;
    for(int iLine = 0; iLine < nbLines; iLine++) {
	for(int iCol = 0; iCol < nbCols; iCol++) {
	    int idNode = iLine*nbCols + iCol;
	    int idRightNeigh = idNode + 1;
	    int idBottomNeigh = idNode + nbCols;
	    if(iCol + 1 < nbCols)
		resEdges.push_back(make_pair(idNode,idRightNeigh));
	    if(iLine + 1 < nbLines)
		resEdges.push_back(make_pair(idNode,idBottomNeigh));
	}
    }
    return resEdges;
}

/*
  Private
*/
vector< vector<int> > maze_from_edges(
    int nbLines, int nbCols, vector< pair<int,int> > edges) {
    const int RIGHT = 1, BOTTOM = 2;
    vector< vector<int> > maze(nbLines,vector<int>(nbCols,0));
    for(int iEdge = 0; iEdge < (int)edges.size(); iEdge++) {
	int idNode1 = edges[iEdge].first;
	int idNode2 = edges[iEdge].second;
	if(idNode1 > idNode2)
	    swap(idNode1,idNode2);
	int iLine = idNode1/nbCols;
	int iCol  = idNode1%nbCols;
	if(idNode2 - idNode1 == 1 && nbCols > 1) { // right
	    maze[iLine][iCol] += RIGHT;
	} else { // bottom
	    maze[iLine][iCol] += BOTTOM;
	}
    }
    return maze;
}


void display_maze(vector< vector<int> > maze) {
    int nbLines = (int)maze.size(), nbCols = (int)maze[0].size();
    for(int iLine = 0; iLine < nbLines; iLine++) {
	for(int iCol = 0; iCol < nbCols; iCol++ ) {
	    cout << ".";
	    if(iCol < nbCols-1) {
		if(!(maze[iLine][iCol] & 1))
		    cout << "#";
		else
		    cout << ".";
	    }
	}
	cout << endl;
	if(iLine < nbLines-1) {
	    for(int iCol = 0; iCol < nbCols; iCol++ ) {
		if(!(maze[iLine][iCol] & 2))
		    cout << "#";
		else
		    cout << ".";
		
		if(iCol < nbCols-1)
		    cout << "#";
	    }
	    cout << endl;
	}
    }
}

void display_maze_js(string name, vector< vector<int> > maze) {
    int nbLines = (int)maze.size(), nbCols = (int)maze[0].size();
    cout << "display_maze(\"" + name + "\","
	 << nbLines << "," << nbCols << ",[" << endl;
    for(int iLine = 0; iLine < nbLines; iLine++) {
	cout << "[";
	for(int iCol = 0; iCol < nbCols; iCol++) {
	    cout << maze[iLine][iCol];
	    cout << ",";
	}
	cout << "]," << endl;
    }
    cout << "]);" << endl;
}


void display_block_maze_js(string name, vector< vector<bool> > maze) {
    int nbLines = (int)maze.size(), nbCols = (int)maze[0].size();
    cout << "display_block_maze(\"" + name + "\","
	 << nbLines << "," << nbCols << ",[" << endl;
    for(int iLine = 0; iLine < nbLines; iLine++) {
	cout << "[";
	for(int iCol = 0; iCol < nbCols; iCol++) {
	    cout << maze[iLine][iCol];
	    cout << ",";
	}
	cout << "]," << endl;
    }
    cout << "]);" << endl;
}


vector< vector<int> > rand_maze(int nbLines, int nbCols) {

    vector< pair<int,int> > treeEdges = rand_spanning_tree(
	nbLines*nbCols, maze_edges(nbLines, nbCols));

    return maze_from_edges(nbLines,nbCols,treeEdges);
}


vector< vector<int> > rand_rec_split_maze(int nbLines, int nbCols) {

    vector< pair<int,int> > treeEdges = rand_rec_split_tree(
	nbLines*nbCols, maze_edges(nbLines, nbCols));

    return maze_from_edges(nbLines,nbCols,treeEdges);
}

// if steadiness = 1 => always cut at half, if steadiness = 1, random
vector< vector<int> > rand_rec_axis_split_maze(int nbLines, int nbCols, float steadiness) {
    if(steadiness > 1)
	steadiness = 1;
    if(steadiness < 0)
	steadiness = 0;
    vector< vector<int> > maze(nbLines, vector<int>(nbCols,0));
    typedef pair<int,int> Coords;
    vector< pair<Coords,Coords> > recs;
    recs.push_back(make_pair(make_pair(0,nbLines-1),make_pair(0,nbCols-1)));
    while(!recs.empty()) {
	pair<Coords,Coords> cur = recs.back();
	recs.pop_back();
	int iLineMin = cur.first.first;
	int iLineMax = cur.first.second;
	int iColMin  = cur.second.first;
	int iColMax  = cur.second.second;
	if(iLineMin == iLineMax && iColMin == iColMax)
	    continue;
	
	int iDim = rand()%2;
	if(iLineMin == iLineMax)
	    iDim = 1;
	if(iColMin == iColMax)
	    iDim = 0;

	int cMin, cMax;
	if(iDim == 0) {// horizontal cut axis
	    cMin = iLineMin;
	    cMax = iLineMax;
	}
	else { // vertical cut axis
	    cMin = iColMin;
	    cMax = iColMax;
	}
	int L = cMax-cMin+1;
	int mini = (steadiness*L)/2;
	int maxi = ((2.f-steadiness)*L)/2;
	if(mini < 1)
	    mini = 1;
	if(maxi > L-1)
	    maxi = L-1;
	if(maxi < mini)
	    maxi = mini;
	int cCut = cMin + rand_int(mini,maxi);
	if(cCut < cMin+1)
	    cCut = cMin+1;
	if(cCut > cMax)
	    cCut = cMax;
	
	if(iDim == 0) {// horizontal cut axis
	    maze[cCut-1][rand_int(iColMin,iColMax)] += 2;
	    recs.push_back(make_pair(
			       make_pair(iLineMin,cCut-1),
			       make_pair(iColMin,iColMax)));
	    recs.push_back(make_pair(
			       make_pair(cCut,iLineMax),
			       make_pair(iColMin,iColMax)));

	}
	else { // vertical cut axis
	    maze[rand_int(iLineMin,iLineMax)][cCut-1] += 1;
	    recs.push_back(make_pair(
			       make_pair(iLineMin,iLineMax),
			       make_pair(iColMin,cCut-1)));
	    recs.push_back(make_pair(
			       make_pair(iLineMin,iLineMax),
			       make_pair(cCut,iColMax)));
	}
	
    }
    return maze;
}



vector< vector<int> > rand_kruskal_maze(int nbLines, int nbCols) {

    vector< pair<int,int> > treeEdges = rand_kruskal_tree(
	nbLines*nbCols, maze_edges(nbLines, nbCols));

    return maze_from_edges(nbLines,nbCols,treeEdges);
}

vector< vector<int> > rand_prim_maze(int nbLines, int nbCols,
				    int iLineStart, int iColStart) {

    vector< pair<int,int> > treeEdges = rand_prim_tree(nbLines*nbCols,
       maze_edges(nbLines, nbCols),nbCols*iLineStart+iColStart);

    return maze_from_edges(nbLines,nbCols,treeEdges);
}


vector< vector<int> > rand_dfs_maze(int nbLines, int nbCols,
				    int iLineStart, int iColStart) {

    vector< pair<int,int> > treeEdges = rand_dfs_tree(nbLines*nbCols,
       maze_edges(nbLines, nbCols),nbCols*iLineStart+iColStart);

    return maze_from_edges(nbLines,nbCols,treeEdges);
}

vector< vector<int> > rand_bfs_maze(int nbLines, int nbCols,
				    int iLineStart, int iColStart) {

    vector< pair<int,int> > treeEdges = rand_bfs_tree(nbLines*nbCols,
       maze_edges(nbLines, nbCols),nbCols*iLineStart+iColStart);

    return maze_from_edges(nbLines,nbCols,treeEdges);
}


vector< vector<int> > rand_dijkstra_maze(int nbLines, int nbCols,
				    int iLineStart, int iColStart) {

    vector< pair<int,int> > treeEdges = rand_dijkstra_tree(nbLines*nbCols,
       maze_edges(nbLines, nbCols),nbCols*iLineStart+iColStart);

    return maze_from_edges(nbLines,nbCols,treeEdges);
}

vector< vector<int> > rand_hunt_and_kill_maze(int nbLines, int nbCols,
				    int iLineStart, int iColStart) {

    vector< pair<int,int> > treeEdges =	rand_hunt_and_kill_tree(nbLines*nbCols,
		       maze_edges(nbLines, nbCols),nbCols*iLineStart+iColStart);

    return maze_from_edges(nbLines,nbCols,treeEdges);
}


vector< vector<int> > rand_binary_maze(int nbLines, int nbCols) {
    vector< vector<int> > maze(nbLines, vector<int>(nbCols,0));
    for(int iLine = 0; iLine < nbLines; iLine++) {
	for(int iCol = 0; iCol < nbCols; iCol++) {
	    maze[iLine][iCol] = rand_int(1,2);
	    if(iLine == nbLines-1)
		maze[iLine][iCol] = 1;
	    if(iCol == nbCols-1)
		maze[iLine][iCol] = 2;
	    if(iLine == nbLines-1 && iCol == nbCols-1)
		maze[iLine][iCol] = 0;
	}
    }
    return maze;
}

vector< vector<int> > rand_eller_maze(
    int nbLines, int nbCols, float mergeProba) {
    vector< vector<int> > maze(nbLines, vector<int>(nbCols,0));

    vector<int> idGroup = range(0,nbCols-1);
    vector< vector<int> > elementOfGroup(nbCols,vector<int>());
    for(int iCol = 0; iCol < nbCols; iCol++)
	elementOfGroup[iCol].push_back(iCol);
    
    for(int iLine = 0; iLine < nbLines; iLine++) {
	// give groupes to the free cells
	int idGroupCur = 0;
	for(int iCol = 0; iCol < nbCols; iCol++) {
	    if(idGroup[iCol] == -1) {
		while(!elementOfGroup[idGroupCur].empty())
		    idGroupCur++;
		idGroup[iCol] = idGroupCur;
		elementOfGroup[idGroupCur].push_back(iCol);
	    }
	}

	// merge
	for(int iCol = 0; iCol < nbCols-1; iCol++) {
	    if(rand_bernoulli(mergeProba) || iLine == nbLines-1) {
		int iGroup1 = idGroup[iCol], iGroup2 = idGroup[iCol+1];
		if(iGroup1 == iGroup2)
		    continue;
		maze[iLine][iCol] += 1;
		if(elementOfGroup[iGroup1].size() > elementOfGroup[iGroup2].size())
		    swap(iGroup1,iGroup2);
		vector<int> & g1 = elementOfGroup[iGroup1];
		vector<int> & g2 = elementOfGroup[iGroup2];
		for(int i = 0; i < (int)g1.size(); i++) {
		    int iCol = g1[i];
		    g2.push_back(iCol);
		    idGroup[iCol] = iGroup2;
		}
		g1.clear();
	    }
	}
	
	//prepare groups of next line
	if(iLine+1 < nbLines) {
	    vector<int> newIdGroup(nbCols,-1);
	    // grow at least one cell of each group downwards
	    for(int iGroup = 0; iGroup < nbCols; iGroup++) {
		if(!elementOfGroup[iGroup].empty()) {
		    int iRand = rand_int(0,(int)elementOfGroup[iGroup].size()-1);
		    int iCol = elementOfGroup[iGroup][iRand];
		    newIdGroup[iCol] = iGroup;
		    maze[iLine][iCol] += 2;
		}
	    }
	    // grow others randomly
	    for(int iCol = 0; iCol < nbCols; iCol++) {
		if(newIdGroup[iCol] == -1 && rand_bernoulli(mergeProba)) {
		    newIdGroup[iCol] = idGroup[iCol];
		    maze[iLine][iCol] += 2;
		}
	    }
	    idGroup = newIdGroup;
	    elementOfGroup = vector< vector<int> >(nbCols, vector<int>());
	    for(int iCol = 0; iCol < nbCols; iCol++) 
		if(idGroup[iCol] != -1)
		    elementOfGroup[idGroup[iCol]].push_back(iCol);
	}
    }
    return maze;
}

vector< vector<int> > rand_sidewinder_maze(
    int nbLines, int nbCols, float verticalProba) {

    vector< vector<int> > maze(nbLines, vector<int>(nbCols,0));
    for(int iCol = 0; iCol < nbCols-1; iCol++) {
	maze[0][iCol] += 1;
    }
    for(int iLine = 1; iLine < nbLines; iLine++) {
	int iBegCol = 0;
	for(int iCol = 0; iCol < nbCols-1; iCol++) {
	    maze[iLine][iCol] += 1;
	    if(rand_bernoulli(verticalProba)) {
		maze[iLine][iCol] -= 1;
		maze[iLine-1][rand_int(iBegCol,iCol)] += 2;
		iBegCol = iCol+1;
	    }
	}
	maze[iLine-1][rand_int(iBegCol,nbCols-1)] += 2;
    }
    return maze;
}


vector< vector<int> > snake_maze(int nbLines, int nbCols) {
    vector< vector<int> > maze(nbLines, vector<int>(nbCols,0));
    for(int iLine = 0; iLine < nbLines-1; iLine++) 
	for(int iCol = 0; iCol < nbCols; iCol++) 
	    maze[iLine][iCol] += 2;
    for(int iCol = 0; iCol < nbCols-1; iCol+=2) 
	maze[nbLines-1][iCol] += 1;
    for(int iCol = 1; iCol < nbCols-1; iCol+=2) 
	maze[0][iCol] += 1;
    return maze;
}

vector< vector<int> > spiral_maze(int nbLines, int nbCols) {
    vector< vector<int> > maze(nbLines, vector<int>(nbCols,0));
    vector< vector<bool> > visited(nbLines, vector<bool>(nbCols,false));
    int iLineCur = 0, iColCur = 0;
    visited[iLineCur][iColCur] = true;
    for(int iter = 0; iter < nbLines+nbCols; iter++) {
	while(iColCur+1 < nbCols && !visited[iLineCur][iColCur+1]) {
	    maze[iLineCur][iColCur] += 1;
	    iColCur++;
	    visited[iLineCur][iColCur] = true;
	}
	while(iLineCur+1 < nbLines && !visited[iLineCur+1][iColCur]) {
	    maze[iLineCur][iColCur] += 2;
	    iLineCur++;
	    visited[iLineCur][iColCur] = true;
	}
	while(iColCur-1 >= 0 && !visited[iLineCur][iColCur-1]) {
	    maze[iLineCur][iColCur-1] += 1;
	    iColCur--;
	    visited[iLineCur][iColCur] = true;
	}
	while(iLineCur-1 >= 0 && !visited[iLineCur-1][iColCur]) {
	    maze[iLineCur-1][iColCur] += 2;
	    iLineCur--;
	    visited[iLineCur][iColCur] = true;
	}

    }
    return maze;
}


/////////////////////
// Modifying walls //
/////////////////////

vector< vector<int> > add_walls_to_maze(vector< vector< int> > maze, int nbWalls) {
    int nbLines = (int)maze.size(), nbCols = (int)maze[0].size();
    
    vector< pair<pair<int,int>,int> > missing;
    for(int iLine = 0; iLine < nbLines; iLine++) {
	for(int iCol = 0; iCol < nbCols; iCol++) {
	    if(iCol+1 < nbCols && (maze[iLine][iCol]&1))
		missing.push_back(make_pair(make_pair(iLine,iCol),1));
	    if(iLine+1 < nbLines && (maze[iLine][iCol]&2))
		missing.push_back(make_pair(make_pair(iLine,iCol),2));
	}
    }
    random_shuffle(missing.begin(),missing.end());

    while(nbWalls > 0 && !missing.empty()) {
	pair< pair<int,int>, int> cur = missing.back();
	missing.pop_back();
	maze[cur.first.first][cur.first.second] -= cur.second;
	nbWalls--;
    }
    return maze;
}

vector< vector<int> > remove_walls_from_maze(vector< vector< int> > maze, int nbWalls) {
    int nbLines = (int)maze.size(), nbCols = (int)maze[0].size();
    
    vector< pair<pair<int,int>,int> > walls;
    for(int iLine = 0; iLine < nbLines; iLine++) {
	for(int iCol = 0; iCol < nbCols; iCol++) {
	    if(iCol+1 < nbCols && (maze[iLine][iCol]&1)==0)
		walls.push_back(make_pair(make_pair(iLine,iCol),1));
	    if(iLine+1 < nbLines && (maze[iLine][iCol]&2)==0)
		walls.push_back(make_pair(make_pair(iLine,iCol),2));
	}
    }
    random_shuffle(walls.begin(),walls.end());

    while(nbWalls > 0 && !walls.empty()) {
	pair< pair<int,int>, int> cur = walls.back();
	walls.pop_back();
	maze[cur.first.first][cur.first.second] += cur.second;
	nbWalls--;
    }
    return maze;
}

//////////////
// Unicycle //
//////////////

vector< vector<int> > unicycle_from_acyclic_maze(vector< vector<int> > maze) {
    int nbLines = 2*(int)maze.size(), nbCols = 2*(int)maze[0].size();
    vector< vector<int> > resMaze(nbLines, vector<int>(nbCols,0));
    for(int iLine = 0; iLine < nbLines; iLine++) {
	for(int iCol = 0; iCol < nbCols; iCol++) {
	    if(iCol+1 < nbCols) {
		bool rightWall = false;
		if(iCol%2==1 && (maze[iLine/2][iCol/2]&1)==0)
		    rightWall = true;
		if(iCol%2==0 && iLine > 0 && (maze[(iLine-1)/2][iCol/2]&2)!=0)
		    rightWall = true;
		if(!rightWall)
		    resMaze[iLine][iCol] += 1;
	    }
	    if(iLine+1 < nbLines) {
		bool  bottomWall = false;
		if(iLine%2==1 && (maze[iLine/2][iCol/2]&2)==0)
		    bottomWall = true;
		if(iLine%2==0 && iCol > 0 && (maze[iLine/2][(iCol-1)/2]&1)!=0)
		    bottomWall = true;
		if(!bottomWall)
		    resMaze[iLine][iCol] += 2;
	    }
	}
    }
    return resMaze;
}


///////////////////////////
// Convert to block maze //
///////////////////////////

/*
  If the initial dimension was (nbLines,nbCols), then the new one
  is (2*nbLines-1,2*nbCols-1). True values correspond to free cells,
  false ones to forbidden cells.
*/
vector< vector<bool> > convert_to_block_maze(vector< vector<int> > maze) {
    int nbLines = (int)maze.size(), nbCols = (int)maze[0].size();
    vector< vector<bool> > res;
    for(int iLine = 0; iLine < nbLines; iLine++) {
	res.push_back(vector<bool>());
	for(int iCol = 0; iCol < nbCols; iCol++ ) {
	    res.back().push_back(true);
	    if(iCol < nbCols-1) {
		if(!(maze[iLine][iCol] & 1))
		    res.back().push_back(false);
		else
		    res.back().push_back(true);
	    }
	}
	res.push_back(vector<bool>());
	if(iLine < nbLines-1) {
	    for(int iCol = 0; iCol < nbCols; iCol++ ) {
		if(!(maze[iLine][iCol] & 2))
		    res.back().push_back(false);
		else
		    res.back().push_back(true);
		if(iCol < nbCols-1)
		    res.back().push_back(false);
	    }
	}
    }
    return res;
}
