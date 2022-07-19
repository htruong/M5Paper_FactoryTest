#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <MD5Builder.h>
#include "frame_feedcontent.h"
#include "frame_urlreader.h"
#include "../utils/urlencoder.h"


#define MAX_BTN_NUM     12

void key_feedcontent_feed_cb(epdgui_args_vector_t &args)
{
    Frame_Base *frame = new Frame_urlReader(((EPDGUI_Button*)(args[0]))->GetCustomString());
    EPDGUI_PushFrame(frame);
    *((int*)(args[1])) = 0;
    log_d("%s", ((EPDGUI_Button*)(args[0]))->GetCustomString().c_str());
}

void key_feedcontent_exit_cb(epdgui_args_vector_t &args)
{
    EPDGUI_PopFrame(true);
    *((int*)(args[0])) = 0;
}

Frame_FeedContent::Frame_FeedContent(String url)
{
    _frame_name = "Frame_FeedContent";
    _url = url;

    uint8_t language = GetLanguage();
    _canvas_title->setTextDatum(CR_DATUM);
    if (language == LANGUAGE_JA)
    {
        exitbtn("ホーム");
    }
    else if (language == LANGUAGE_ZH)
    {
        exitbtn("主页");
    }
    else
    {
        exitbtn("Back");
    }

    _canvas_title->drawString("Items Index", 540 - 15, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &key_feedcontent_exit_cb);
}

bool Frame_FeedContent::downloadFeed() {
    if (WiFi.status() != WL_CONNECTED) {
        log_e("URL %s is not cached and you're not connected to the internet, gave up.", _url.c_str());
        return false;
    }

    HTTPClient http;
    http.begin("http://article-proxy.tnhh.net/feed/?url=" + URLEncoder::urlencode(_url));
    int httpCode = http.GET();


    if (httpCode == HTTP_CODE_OK) {
        DynamicJsonDocument doc(4096);
        StaticJsonDocument<200> filter;
        filter["items"][0]["title"] = true;
        filter["items"][0]["url"] = true;
        DeserializationError error = deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter));
        if (error != DeserializationError::Ok) {
            log_e("URL %s cache error: %s.", _url.c_str(), error.c_str());
            return false;
        }
        
        log_d("URL %s cached successfully.", _url.c_str());

/*
        char buf [1000];
        serializeJson(doc["items"], buf, 10000);
        log_d("JSON: %s", buf);
*/

        JsonArray RSSItems = doc["items"].as<JsonArray>();

        int item_count = 0;
        log_d("Begin %llu, end %llu", RSSItems.begin(), RSSItems.end());
        for (JsonArray::iterator it=RSSItems.begin(); it!=RSSItems.end(); ++it) {
            log_d("Item %llu: %s at %s", it, (*it)["title"].as<char*>(), (*it)["url"].as<char*>());
            struct rssItem_t item;
            item.name = String((*it)["title"].as<char*>());
            item.url = String((*it)["url"].as<char*>());
            _feed.push_back(item);
        }
    } else {
        log_e("URL %s caching failed.", _url.c_str());
        return false;
    }
    

    return true;
}

void Frame_FeedContent::listFeeds()
{
    if (! downloadFeed()) {
        log_e ("Cannot download feed %s.", _url);
        return;
    }

    for(int n = 0; n < _feed.size(); n++)
    {
        if(_key_feed.size() > MAX_BTN_NUM)
        {
            break;
        }
        EPDGUI_Button *btn = new EPDGUI_Button(4, 100 + _key_feed.size() * 60, 532, 61);
        _key_feed.push_back(btn);

        btn->CanvasNormal()->fillCanvas(0);
        btn->CanvasNormal()->drawRect(0, 0, 532, 61, 15);
        btn->CanvasNormal()->setTextSize(26);
        btn->CanvasNormal()->setTextDatum(CL_DATUM);
        btn->CanvasNormal()->setTextColor(15);
        btn->CanvasNormal()->drawString(_feed[n].name, 47 + 13, 35);
        btn->SetCustomString(_feed[n].url);
        btn->CanvasNormal()->setTextDatum(CR_DATUM);
        btn->CanvasNormal()->pushImage(15, 14, 32, 32, ImageResource_item_icon_file_floder_32x32);
        btn->CanvasNormal()->pushImage(532 - 15 - 32, 14, 32, 32, ImageResource_item_icon_arrow_r_32x32);
        *(btn->CanvasPressed()) = *(btn->CanvasNormal());
        btn->CanvasPressed()->ReverseColor();

        btn->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, btn);
        btn->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void*)(&_is_run));
        btn->Bind(EPDGUI_Button::EVENT_RELEASED, key_feedcontent_feed_cb);
    }
}

Frame_FeedContent::~Frame_FeedContent(void)
{
    for(int i = 0; i < _key_feed.size(); i++)
    {
        delete _key_feed[i];
    }
}

int Frame_FeedContent::init(epdgui_args_vector_t &args)
{
    _is_run = 1;

    if(_key_feed.size() == 0)
    {
        listFeeds();
    }
    
    M5.EPD.WriteFullGram4bpp(GetWallpaper());
    _canvas_title->pushCanvas(0, 8, UPDATE_MODE_NONE);
    EPDGUI_AddObject(_key_exit);

    for(int i = 0; i < _key_feed.size(); i++)
    {
        EPDGUI_AddObject(_key_feed[i]);
    }

    return 3;
}