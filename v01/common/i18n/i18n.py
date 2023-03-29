#!/usr/bin/env python3

# Library to handle i18n translations

import json, os

__initialized = False
__translations = {}

def setI18nOptions(basePath='./', directPath=None, language=None, defaultLanguage='en'):
    global __initialized, __translations

    if language == None:
        language = os.environ.get('TASKGRADER_LOCALE', 'en')

    if language != defaultLanguage and not __initialized:
        setI18nOptions(basePath=basePath, directPath=directPath, language=defaultLanguage)

    __initialized = True

    if directPath == None:
        path = os.path.join(basePath, language, 'translations.json')
    else:
        try:
            path = directPath % language
        except:
            path = directPath

    if os.path.isfile(path):
        try:
            newTranslations = json.load(open(path, 'r'))
        except:
            return 'Error: file `%s` contains invalid JSON data.' % path

        if not isinstance(newTranslations, dict):
            return "Error: file `%s` doesn't contain a JSON object." % path

        __translations = newTranslations
        return True
    else:
        return 'Error: file `%s` not found.' % path


def translate(token):
    global __initialized, __translations

    if not __initialized:
        setI18nOptions()

    try:
        translation = __translations.get(token, token)
    except:
        return token

    if isinstance(translation, str):
        return translation
    else:
        try:
            # Python 2 compatibility
            if isinstance(translation, basestring):
                return translation
        except:
            pass
        return token

def _(token):
    return translate(token)
