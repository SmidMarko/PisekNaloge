function initTask(subTask) {
   var cellSide = 60;

   subTask.gridInfos = {
      hideSaveOrLoad: false,
      cellSide: cellSide,
      actionDelay: 200,
      itemTypes: {
         tabornik: { img: "green_robot.png", category: "robot", side: 60, nbStates: 8, offsetX: 0 },
         obstacle: { num: 2, img: "obstacle.png", side: cellSide, isObstacle: true },
         green: { num: 3, img: "object.png", side: cellSide, color: "vert" },
         pill1: { num: 4, img: "object.png", side: cellSide, category: "pill", isObstacle: false, isCollectible: true, zOrder: 1, isTransportable: true },
         pill2: { num: 5, img: "object.png", side: cellSide, category: "pill", isObstacle: false, isCollectible: true, zOrder: 1, isTransportable: true },
         pill3: { num: 6, img: "object.png", side: cellSide, category: "pill", isObstacle: false, isCollectible: true, zOrder: 1, isTransportable: true },
         pill4: { num: 7, img: "object.png", side: cellSide, category: "pill", isObstacle: false, isCollectible: true, zOrder: 1, isTransportable: true },
         pill5: { num: 8, img: "object.png", side: cellSide, category: "pill", isObstacle: false, isCollectible: true, zOrder: 1, isTransportable: true }
         
      },
      maxInstructions: 15,
      includeBlocks: {
         groupByCategory: true,
         generatedBlocks: {
            robot: ["right", "left", "forward"]
         },
         standardBlocks: {
            includeAll: false,
            wholeCategories: {
				easy: ["loops"],
			},
            singleBlocks: [],
         }
      },
	  backgroundColor: "#96fc88",
	  blocklyColourTheme: "bwinf",
      ignoreInvalidMoves: false,
      groupByCategory: false,
      includedAll: false,
      includedCategories: [ ],
      includedBlocks: [],
      checkEndEveryTurn: true,
      checkEndCondition: robotEndConditions.checkPickedAllCollectibles
   };

   subTask.data = {
      easy: [
         {
            tiles: [
			       [2, 2, 2, 2, 2, 2, 2, 2],
			       [2, 1, 1, 1, 5, 1, 1, 2],
			       [2, 1, 4, 2, 2, 2, 6, 2],
			       [2, 1, 8, 2, 2, 2, 6, 2],
			       [2, 1, 1, 2, 2, 2, 1, 2],
			       [2, 1, 8, 1, 1, 1, 1, 2],
			       [2, 1, 1, 1, 1, 1, 1, 2],
			       [2, 2, 2, 2, 2, 2, 2, 2]
               ],
            initItems: [
                  { row: 5, col: 6, dir: 2, type: "tabornik" }
               ]
         }
      ],
   };

   initBlocklySubTask(subTask);
}

initWrapper(initTask, null, null, true);
   