#!/usr/bin/env python3

from os import path
from bottle import route, run, error, get, post, request, static_file, redirect, response
import json
import os

# from 
def enable_cors(fn):
    def _enable_cors(*args, **kwargs):
        # set CORS headers
        response.headers['Access-Control-Allow-Origin'] = '*'
        response.headers['Access-Control-Allow-Methods'] = 'GET, POST, OPTIONS'
        response.headers['Access-Control-Allow-Headers'] = 'Origin, Accept, Content-Type, X-Requested-With, X-CSRF-Token'
        if request.method != 'OPTIONS':
            # actual request; reply with the actual response
            return fn(*args, **kwargs)

    return _enable_cors

taskTemplate = """<!DOCTYPE html><html>
  <head>
    <meta charset="utf-8">
    <title>Sample Task</title>
    <link class="task" type="text/css" rel="stylesheet" href="{commonRelPath}/modules/pemFioi/progTask.css">
    <link class="task" type="text/css" rel="stylesheet" href="{commonRelPath}/modules/ext/bootstrap/css/bootstrap.min.css">
    <script class="remove" src="{commonRelPath}/modules/ext/requirejs/require.js"></script>
    <script type="text/javascript">
      var modulesPath ='{commonRelPath}/modules';
      var taskPlatformPath ='{commonRelPath}/task-platform';
    </script>
    <script type="text/javascript" src="{commonRelPath}/modules/pemFioi/progTaskConfig-1.0.js"></script>
    <script type="text/javascript" class="remove">
      // general metadata conforming the PEM API Documentation for getMetaData
      var PEMTaskMetaData = {{
        // title is automatically fetched from the <title> markup
        id : '{path}',
        license : 'MIT',
        authors : ["France IOI"],
        language : "fr",
        version: "0.1",
        baseUrl : "",
        testMode: {testMode},
        hasUserTests: {userTests},
        evalOutputScript: {evalOutputScript},
        supportedLanguages: {supportedLanguages},
        limits : {limits}
      }};
      // Metadata specific to the FranceIOI tasks
      var FIOITaskMetaData = {{

      }};
    </script>
  </head>
<body ng-controller="taskController">
    <h1>{title}</h1>
    {task}
    {hints}
    {solution}
  </body>
</html>"""

@error(404)
def error404(error):
    print('404')
    return 'Nothing here, sorry'

def getLimits(tm_limits, taskId):
    limits = {}
    for limitId, limit in tm_limits.items():
        if limit['idTask'] == taskId:
            limits[limit['sLangProg']] = {'time': limit['iMaxTime'], 'memory': limit['iMaxMemory']}
    return json.dumps(limits)

def getTaskStr(tm_tasks_strings, taskId, sLang):
    res = {'task': None, 'solution': None, 'title': None}
    resStr = None
    for stringsId, strings in tm_tasks_strings.items():
        if strings['idTask'] == taskId: # and strings['sLanguage'] == sLang:
            resStr = strings
            break
    if not resStr:
        return res
    res['task'] = '<div id="task">'+resStr['sStatement']+'</div>'
    res['title'] = resStr['sTitle']
    res['solution'] = '<div id="solution">'+resStr['sSolution']+'</div>'
    return res

def getHintsStr(tm_hints, tm_hints_strings, taskId, sLang):
    # TODO
    hints = {}
    resTab = {}
    res = ''
    print(tm_hints)
    print(tm_hints_strings)
    for hintId, hint in tm_hints.items():
        if hint['idTask'] == taskId:
            hints[hint['ID']] = hint['iRank']
    print(hints)
    for hintstringId, hintstring in tm_hints_strings.items():
        if hintstring['idHint'] in hints: #and hintstring['sLanguage'] == sLang:
            iRank = hints[hintstring['idHint']]
            resTab[iRank] = hintstring['sContent']
    for iRank, hintstr in sorted(resTab.items()):
        res += '\n<div class="hint">'+hintstr+'</div>\n'
    return res

def getTask(tm_tasks, askedTaskId):
    resTask = None
    for taskId, task in tm_tasks.items():
        if taskId == askedTaskId:
            resTask = task
            break
    if not resTask or 'sTextId' not in resTask:
        return None
    return resTask

def getBool(tm_task, attr):
    if attr not in tm_task:
        return 'false'
    if tm_task[attr] == True or tm_task[attr] == 1 or tm_task[attr] == '1':
        return 'true'
    return 'false'

def getEvalOutputScript(tm_task):
    if 'sEvalResultOutputScript' not in tm_task:
        return json.dumps('')
    return json.dumps(tm_task['sEvalResultOutputScript'])

@post('/saveTask', method=['OPTIONS', 'GET', 'POST'])
@enable_cors
def saveTask():
    if not request.json:
        return {'success':False, 'error': 'could not decrypt'}
    taskId = request.json['idToSave']
    task = getTask(request.json['tm_tasks'], taskId)
    if not task:
        return {'success':False, 'error': 'cannot find path'}
    taskPath = task['sTextId'].strip(' \t\n\r/')
    limits = getLimits(request.json['tm_tasks_limits'], taskId)
    commonRelPath = ''
    for i in range(taskPath.count('/')+1): commonRelPath += '../'
    commonRelPath += '_common'
    supportedLanguages = json.dumps(task["sSupportedLangProg"].split(',')) # TODO
    taskStrings = getTaskStr(request.json['tm_tasks_strings'], taskId, 'fr')
    hintsString = getHintsStr(request.json['tm_hints'], request.json['tm_hints_strings'], taskId, 'fr')
    testMode = getBool(task, 'bTestMode')
    userTests = getBool(task, 'bUserTests')
    evalOutputScript = getEvalOutputScript(task)
    formatArgs = {
        'commonRelPath': commonRelPath,
        'supportedLanguages': supportedLanguages,
        'path': taskPath,
        'title': taskStrings['title'],
        'task': taskStrings['task'],
        'solution': taskStrings['solution'],
        'hints': hintsString,
        'limits': limits,
        'testMode': testMode,
        'userTests': userTests,
        'evalOutputScript': evalOutputScript
        }
    formattedIndex = taskTemplate.format(**formatArgs)
    print(formattedIndex)
    outputFileDir = path.abspath(path.join(path.dirname(__file__), '../../', taskPath))
    if not os.path.exists(outputFileDir):
        os.makedirs(outputFileDir)
    outputFilePath = path.join(outputFileDir, 'index.html')
    with open(outputFilePath, "w") as outputFile:
        outputFile.write(formattedIndex)
    return {'success':True, 'message': 'output written to '+outputFilePath}

@route('/')
def main():
    redirect("/_common/task-creation-interface/index.html")

@route('/<filename:path>')
def server_static(filename):
    rootDir = path.abspath(path.join(path.dirname(__file__), '../../'))
    return static_file(filename, root=rootDir)

run(host='localhost', port=8091, debug=True)