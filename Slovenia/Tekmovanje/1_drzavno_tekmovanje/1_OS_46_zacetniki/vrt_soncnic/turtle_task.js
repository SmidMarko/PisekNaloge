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
		actionDelay: 20,						//parameter za časovni zamik med izvajanjem ukazov
		blocklyColourTheme: "bwinf",	//izbira seta barv za bloke ukazov
		maxInstructions: {												//omejitev števila ukazov
			easy: 0, 
		},
		includeBlocks: {																	//dovoljeni ukazi 
			groupByCategory: {
				easy: false, 
			},
			generatedBlocks: {
				turtle: ["moveamountvalue", "turnleftamountvalue","turnrightamountvalue","penup","pendown","sunflower"]
			 },
			standardBlocks: {
				includeAll: {
					easy: false, 
				},
				wholeCategories: [],
				singleBlocks: ["controls_repeat"],
			}
		},
		startingExample: {
			blockly: '<xml xmlns="http://www.w3.org/1999/xhtml"><block type="robot_start" id="_HuWA~Mp=ouylCUHrfAj" deletable="false" movable="false" editable="false" x="0" y="0"><next><block type="controls_repeat" id="lVkz+Qd0R7[XNEeB!We?"><field name="TIMES">10</field><statement name="DO"><block type="penup" id="6hK6voP4j;C9l9A+)6jy"><next><block type="sunflower" id="pa0U3p/m_H=`:c/Ml?m{"><next><block type="moveamountvalue" id="pWdFiB6Gk.]hW0PIMR#y"><field name="PARAM_0">25</field><next><block type="moveamountvalue" id="slo=6XT7PVnzH3*=h4G{"><field name="PARAM_0">25</field><next><block type="pendown" id="kW9E;Pim_ASk*g.2YZvT"><next><block type="turnleftamountvalue" id="M(xH5Zk3T5uLm:/Y6T5]"><field name="PARAM_0">30</field></block></next></block></next></block></next></block></next></block></next></block></statement></block></next></block><additional>{}</additional></xml>',
		},
		checkEndEveryTurn: false,													//kako pogosto preverjamo uspešnost rešitve
		checkEndCondition: turtleEndConditions.checkImagesEqual,	//funkcija za preverjanje uspešnosti rešitve
     		
		overlayFileName: "sand.png",
		turtleImageFile: "pisek2.png",
		turtleStepSize: {
			easy: 0.125,
		},	
		coords : { //max x:300, y:300
			easy: {x: 150, y: 150},
		},
	};

	subTask.data = {					//testni primeri
		easy: [
			{
				drawSolution: function(turtle) { 
					for (var i = 0; i < 12; i++) {
						turtle.stop_painting();
						turtle.move(25);
						turtle.start_painting();
						turtle.move(50);
						turtle.stop_painting();
						turtle.move(25);
						
						turtle.drawSunflower();
						
						turtle.move(-100);
						turtle.turn(30);
					}
				},
			},
		],
	};

	initBlocklySubTask(subTask);	//klic programskega orodja Blockly
}

//initWrapper(initTask, ["easy"], null, true);
initWrapper(initTask, null, null, true);
