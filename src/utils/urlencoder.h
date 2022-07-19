#ifndef _URLENCODER_H_
#define _URLENCODER_H_

#include <Arduino.h>

class URLEncoder {
public:
    static String urlencode(String str);
};

#endif //_URLENCODER_H_