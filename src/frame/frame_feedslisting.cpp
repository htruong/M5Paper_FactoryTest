#include "frame_feedslisting.h"
#include "frame_feedcontent.h"

#define MAX_BTN_NUM     12

void key_feedslisting_feed_cb(epdgui_args_vector_t &args)
{
    Frame_Base *frame = new Frame_FeedContent(((EPDGUI_Button*)(args[0]))->GetCustomString());
    EPDGUI_PushFrame(frame);
    *((int*)(args[1])) = 0;
    log_d("%s", ((EPDGUI_Button*)(args[0]))->GetCustomString().c_str());
}

void key_feedslisting_exit_cb(epdgui_args_vector_t &args)
{
    EPDGUI_PopFrame(true);
    *((int*)(args[0])) = 0;
}

Frame_FeedsListing::Frame_FeedsListing(String path)
{
    _frame_name = "Frame_FeedsListing";
    _path = path;

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
        exitbtn("Home");
    }
    _canvas_title->drawString("Feeds Index", 540 - 15, 34);

    _key_exit->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, (void *)(&_is_run));
    _key_exit->Bind(EPDGUI_Button::EVENT_RELEASED, &key_feedslisting_exit_cb);
}

void Frame_FeedsListing::listFeeds(fs::FS &fs, const char *filename)
{
    std::vector<struct rssFeed_t> sites;

    // TODO: Read this list from a text file or something

    struct rssFeed_t hn;
    hn.name = "Hacker News Frontpage";
    hn.url = "https://hnrss.org/frontpage";
    sites.push_back(hn);

    hn.name = "Huan Truong's blog";
    hn.url = "http://www.tnhh.net/feed.xml";
    sites.push_back(hn);

    hn.name = "VnExpress";
    hn.url = "https://vnexpress.net/rss/tin-moi-nhat.rss";
    sites.push_back(hn);

    hn.name = "Hackaday";
    hn.url = "http://feeds.feedburner.com/hackaday";
    sites.push_back(hn);

    for(int n = 0; n < sites.size(); n++)
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
        btn->CanvasNormal()->drawString(sites[n].name, 47 + 13, 35);
        btn->SetCustomString(sites[n].url);
        btn->CanvasNormal()->setTextDatum(CR_DATUM);
        btn->CanvasNormal()->pushImage(15, 14, 32, 32, ImageResource_item_icon_file_floder_32x32);
        btn->CanvasNormal()->pushImage(532 - 15 - 32, 14, 32, 32, ImageResource_item_icon_arrow_r_32x32);
        *(btn->CanvasPressed()) = *(btn->CanvasNormal());
        btn->CanvasPressed()->ReverseColor();

        btn->AddArgs(EPDGUI_Button::EVENT_RELEASED, 0, btn);
        btn->AddArgs(EPDGUI_Button::EVENT_RELEASED, 1, (void*)(&_is_run));
        btn->Bind(EPDGUI_Button::EVENT_RELEASED, key_feedslisting_feed_cb);
    }
}

Frame_FeedsListing::~Frame_FeedsListing(void)
{
    for(int i = 0; i < _key_feed.size(); i++)
    {
        delete _key_feed[i];
    }
}

int Frame_FeedsListing::init(epdgui_args_vector_t &args)
{
    _is_run = 1;

    if(_key_feed.size() == 0)
    {
        listFeeds(SD, _path.c_str());
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