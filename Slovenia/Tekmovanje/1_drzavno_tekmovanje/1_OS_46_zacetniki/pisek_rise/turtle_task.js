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
		zoom: {
			controls: true,
			scale: 1,
		},
		actionDelay: 200,						//parameter za časovni zamik med izvajanjem ukazov
		blocklyColourTheme: "bwinf",	//izbira seta barv za bloke ukazov
		maxInstructions: {												//omejitev števila ukazov
			easy: 16, 
		},
		includeBlocks: {																	//dovoljeni ukazi 
			groupByCategory: {
				easy: false, 
			},
			generatedBlocks: {
				turtle: {
					easy: ["moveamountvalue", "turnleft", "turnright", "penup", "pendown"],
				},
			},
			standardBlocks: {
				includeAll: {
					easy: false, 
				},
				wholeCategories: [],
				singleBlocks: [],
				singleBlocks: ["controls_repeat_ext"],
			},
			variables:{
				easy: [],
			},
		},
		startingExample: '',																//vnaprej podana koda ukazov
		checkEndEveryTurn: false,													//kako pogosto preverjamo uspešnost rešitve
		checkEndCondition: turtleEndConditions.checkImagesEqual,	//funkcija za preverjanje uspešnosti rešitve
     		
		overlayFileName: "sand.png",
		turtleImageFile: "pisek2.png",
		turtleStepSize: {
			easy: 0.3,
		},	
		coords : { //max x:300, y:300
			easy: {x: 15, y: 200},
		},
	};

	subTask.data = {					//testni primeri
		easy: [
			{
				drawSolution: function(turtle) {
					turtle.move(20); 
					turtle.turn(-90);
					for (var i = 0; i < 4; i++) {
						turtle.stop_painting();
						turtle.move(10);
						turtle.start_painting();
						turtle.move(10);		
					}
					turtle.stop_painting();
					turtle.move(10);
					turtle.turn(90);
					turtle.start_painting();
					turtle.move(20);
					
				},
			},
		],
	};

	initBlocklySubTask(subTask);	//klic programskega orodja Blockly
}

//initWrapper(initTask, ["easy"], null, true);
initWrapper(initTask, null, null, true);
