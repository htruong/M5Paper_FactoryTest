#ifndef _FRAME_FEEDCONTENT_H_
#define _FRAME_FEEDCONTENT_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"
#include <SD.h>

struct rssItem_t {
    String name;
    String url;
};

class Frame_FeedContent : public Frame_Base
{
public:
    Frame_FeedContent(String url);
    ~Frame_FeedContent();
    void listFeeds();
    int init(epdgui_args_vector_t &args);

private:
    bool downloadFeed();
    std::vector<struct rssItem_t> _feed;
    std::vector<EPDGUI_Button*> _key_feed;
    String _url;
};

#endif //_FRAME_FEEDCONTENT_H_