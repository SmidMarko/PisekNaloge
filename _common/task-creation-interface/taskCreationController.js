app.controller('taskCreationController', ['$scope', '$rootScope', 'Languages', '$http', '$timeout', function($scope, $rootScope, Languages, $http, $timeout) {
    'use strict';

    ModelsManager.init(models);
    $rootScope.formData = {};

    $scope.loadTaskId = function(taskId, defaultLang) {
    	$rootScope.taskId = taskId;
	    _.forOwn(ModelsManager.getRecords('tm_tasks'), function(tm_task) {
	        if (tm_task.ID == $rootScope.taskId) {
	           $rootScope.tm_task = tm_task;
              tm_task.aSupportedLangProg = tm_task.sSupportedLangProg.split(',')
              $rootScope.formData.path = tm_task.sTextId;
	            return false;
	        }
	     });
	     // apply strings if already present
	     _.forOwn(ModelsManager.getRecords('tm_tasks_strings'), function(tm_tasks_strings) {
	        if (tm_tasks_strings.idTask == $rootScope.taskId) {
	            $rootScope.tm_tasks_strings = tm_tasks_strings;
               if (!tm_tasks_strings.sLanguage) {
                  tm_tasks_strings.sLanguage = defaultLang;
               }
	            return false;
	        }
	    });
       $timeout($scope.$apply);
    }

    $scope.loadTask = function() {
      var path = $rootScope.formData.path;
      if (!path) {
        $scope.error('no path to load!');
        return;
      }
      var url = '/'+path+'/index.html';
      $('#taskIframe').attr('src', url);
      $timeout(function() {
       	TaskProxyManager.getTaskProxy('taskIframe', function(task) {
       		window.task = task;
       		var platform = new Platform(task);
       		TaskProxyManager.setPlatform(task, platform);
   			task.load({metadata:true, grader:true, solution:true, hints:true, editor:true, resources:true}, function() {
   				task.getMetaData(function(metadata) {
   			     	task.getResources(function(resources) {
   			     		var taskId = insertResourcesInModel(resources, metadata);
   			     		$scope.loadTaskId(taskId, 'fr');
   			     	}, function(errormsg) {$scope.error(errormsg);});
   			 	}, function(errormsg) {$scope.error(errormsg);});
   			}, function(errormsg) {$scope.error(errormsg);});
       	}, true);
      }, 1000);
    };

    $scope.saveTask = function() {
    	var params = ModelsManager.curData;
    	params.idToSave = $rootScope.taskId;
      $rootScope.tm_task.sTextId = $rootScope.formData.path;
      $rootScope.tm_task.sSupportedLangProg = $rootScope.tm_task.aSupportedLangProg.join(',')
    	// tm_* can be recursive, do not dump objects and arrays in them
    	function donotdumpobjects(key, value) {
 		   if (key && key.substring(0, 3) !== 'tm_' && ! /^\d+$/.test(key) && value && (typeof value === "object" || typeof value === "array")) {
    		   return undefined;
    	   }
 		   return value;
      }
      var paramsStr = JSON.stringify(params, donotdumpobjects);
   	$http.post('http://127.0.0.1:8091/saveTask', paramsStr, {responseType: 'json'}).success(function(postRes) {
      	if (!postRes || !postRes.success) {
         	$scope.error('error calling python script'+(postRes ? ': '+postRes.error : ''));
         	return;
      	}
         $scope.addAlert('success', postRes.message);
   	}).error(console.error);
    }

      $scope.alerts = [];

     $scope.addAlert = function(type, msg) {
       $scope.alerts.push({type: type, msg: msg});
     };

     $scope.closeAlert = function(index) {
       $scope.alerts.splice(index, 1);
     };

    $scope.error = function(msg) {
      console.error(msg);
      $scope.addAlert('danger', msg);
    };

    $scope.newTask = function() {
    	var noResources = {task: {}, solution: {}};
    	var taskId = insertResourcesInModel(noResources, {});
	   $scope.loadTaskId(taskId, 'fr');
    }


    $scope.newLimit = function() {
      var limit = ModelsManager.createRecord('tm_tasks_limits');
      limit.idTask = $rootScope.taskId;
      limit.sLangProg = '*';
      limit.iMaxTime = 10000;
      limit.iMaxMemory = 16000;
      ModelsManager.insertRecord("tm_tasks_limits", limit);
      $timeout($scope.$apply);
    };

    function getNextHintRank() {
      var res = 0;
      _.forOwn(ModelsManager.getRecords('tm_hints'), function(hint) {
         if (hint.idTask == $rootScope.taskId && hint.iRank > res) {
            res = hint.iRank;
         }
      });
      return res+1;
    }

    $scope.newHint = function() {
      var hint = ModelsManager.createRecord('tm_hints');
      hint.iRank = getNextHintRank();
      hint.idTask = $rootScope.taskId;
      ModelsManager.insertRecord("tm_hints", hint);
      var hintStrings = ModelsManager.createRecord('tm_hints_strings');
      hintStrings.idHint = hint.ID;
      hintStrings.sLanguage = 'fr';
      hintStrings.sContent = '';
      ModelsManager.insertRecord("tm_hints_strings", hintStrings);
      $timeout($scope.$apply);
    };

}]);

angular.element(document).ready(function () {
  angular.module('submission-manager', []);
  angular.bootstrap(document, ['pemTask'], {strictDi: true});
});