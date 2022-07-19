#ifndef _FRAME_FEEDSLISTING_H_
#define _FRAME_FEEDSLISTING_H_

#include "frame_base.h"
#include "../epdgui/epdgui.h"
#include <SD.h>

struct rssFeed_t {
    String name;
    String url;
};

class Frame_FeedsListing : public Frame_Base
{
public:
    Frame_FeedsListing(String path);
    ~Frame_FeedsListing();
    void listFeeds(fs::FS &fs, const char *filename);
    int init(epdgui_args_vector_t &args);

private:
    std::vector<EPDGUI_Button*> _key_feed;
    String _path;
};

#endif //_FRAME_FEEDSLISTING_H_