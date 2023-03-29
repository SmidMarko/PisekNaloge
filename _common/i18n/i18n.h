#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "jsonxx.h"

using namespace std;
using namespace jsonxx;

namespace i18n {
Object __translations;

void setI18nOptions() {
    ifstream translationsFile;
    translationsFile.open("en/translations.json");
    __translations.parse(translationsFile);
}

string translate(string origStr) {
    if(__translations.has<String>(origStr.c_str())) {
        return __translations.get<String>(origStr);
    } else {
        return origStr;
    }
}

string _(string origStr) {
    return translate(origStr);
}

}
