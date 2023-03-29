function initTask(subTask) {
    subTask.gridInfos = {				//podatki za urejevalnik in vizualno okno
		hideControls: {					//gumbi na urejevalniku
			restart: false,
			saveOrLoad: false,			//gumba za shranjevanje in nalaganje kode ukazov
			loadBestAnswer: false,
			speedSlider: false,
			backToFirst: false,
			nextStep: false,
			goToEnd: false,
		},
		introMaxHeight: "33%",				//maksimalna velikost prostora za navodila
		maxListSize: 100, 						//max. dolžina seznama
		scrollbars: true,
		showLabels: false,
		zoom: {
			controls: true,
			scale: 1,
		},
		actionDelay: 200,						//parameter za časovni zamik med izvajanjem ukazov
		blocklyColourTheme: "bwinf",	//izbira seta barv za bloke ukazov
		maxInstructions: 6,
		includeBlocks: {						//dovoljeni ukazi 
			groupByCategory: {
				easy: false,
			},
			generatedBlocks: {
				robot: ["forward","markedCell", "paint"],
			},
			standardBlocks: {
				includeAll: false,
				wholeCategories: [],
				singleBlocks: ["controls_repeat_ext","controls_if"],
				excludedBlocks: [],
			},
		},
		startingExample: { //vnaprej podana koda ukazov
			blockly: '',
		},					
		checkEndEveryTurn: false,		//kako pogosto preverjamo uspešnost rešitve
		checkEndCondition: robotEndConditions.checkMarkersPainted,
			  
		cellSide: 60,							
		itemTypes: {
			green_robot: { img: "zajec.png", category: "robot", side: 80, nbStates: 8, offsetX: -14, zOrder: 8},
			tile1: { num: 4, img: "smet1.png", side: 60, zOrder: 1 },
			tile2: { num: 5, img: "smet2.png", side: 60, zOrder: 1 },
			marker: { num: 7, img: "korencek.png", side: 60, category: "marker", isMarker: true, zOrder: 1 },
			paint: { num: 8, img: "prazno_polje.png", side: 60, category: "paint", isPaint: true, zOrder: 2 },
		}, //isRound, isSquare, isTriangle, isDotted, isFull, isStriped, // hasColor(color) // isTransportable(color, shape) // "paint"(color) //"number"(value)
		ignoreInvalidMoves: false,
	};

	subTask.data = {						//testni primeri
		easy: [
			{
				tiles: [
					[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
				],
				initItems: [
					{ row: 0, col: 0, dir: 0, type: "green_robot" },
					{ row: 0, col: 2, type: "marker" },
					{ row: 0, col: 4, type: "marker" },
					{ row: 0, col: 8, type: "marker" },
					{ row: 0, col: 7, type: "tile1" },
					{ row: 0, col: 11, type: "tile2" },
				],
			},
			{
				tiles: [
					[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
				],
				initItems: [
					{ row: 0, col: 0, dir: 0, type: "green_robot" },
					{ row: 0, col: 1, type: "marker" },
					{ row: 0, col: 3, type: "marker" },
					{ row: 0, col: 5, type: "tile1" },
					{ row: 0, col: 9, type: "marker" },
					{ row: 0, col: 7, type: "tile2" },
					{ row: 0, col: 11, type: "marker" },
				],
			},
			{
				tiles: [
					[1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1],
				],
				initItems: [
					{ row: 0, col: 0, dir: 0, type: "green_robot" },
					{ row: 0, col: 1, type: "tile1" },
					{ row: 0, col: 3, type: "marker" },
					{ row: 0, col: 4, type: "marker" },
					{ row: 0, col: 5, type: "marker" },
					{ row: 0, col: 10, type: "tile2" },
					{ row: 0, col: 8, type: "tile1" },
				],
			},
		],
	};

	initBlocklySubTask(subTask);	//klic programskega orodja Blockly
}

initWrapper(initTask, null, null, true);
//initWrapper(initTask, null, null, true);
