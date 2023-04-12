#include <Geode/Geode.hpp>
#include <Geode/modify/CCTransitionFade.hpp> //only this transition is available fsr
#include <Geode/cocos/extensions/network/HttpRequest.h>
#include <Geode/cocos/extensions/network/HttpResponse.h>
#include <Geode/cocos/extensions/network/HttpClient.h>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <random>
#include <iterator>
#include <typeinfo>

using namespace geode::prelude;

#define getTS float transitionSpeed = (float) Mod::get()->getSettingValue<double>("transition-speed")
class $modify(CCTransitionFade) {
    static CCTransitionFade* create(float d, CCScene * s) {
        getTS;
        return CCTransitionFade::create(transitionSpeed, s);
    }
};

template<typename Iter, typename RandomGenerator>
Iter select_randomly2(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter select_randomly(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return select_randomly2(start, end, gen);
}

template <typename Out>
void split(const std::string& s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

template <typename vectelemtype>
std::vector<vectelemtype> split(const vectelemtype& s, char delim) {
    std::vector<vectelemtype> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

class PerformRandomLevel : public CCLayer {
protected:
    bool init() {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        auto bg = CCSprite::create("GJ_gradientBG.png");
        auto bgSize = bg->getContentSize();

        bg->setScaleX(winSize.width / bgSize.width);
        bg->setScaleY(winSize.height / bgSize.height);
        bg->setAnchorPoint(CCPoint{ 0,0 });
        bg->setZOrder(-1);

        auto thetext = CCLabelBMFont::create("Getting random level", "bigFont.fnt");
        thetext->setContentSize(thetext->getContentSize() * 1.5f);
        thetext->setPosition(winSize / 2 + CCPoint{ 0,30 });


        this->addChild(bg); this->addChild(thetext);
        LoadingCircle::create()->show();
        return true;
    }
public:
    static PerformRandomLevel* create() {
        auto pret = new PerformRandomLevel();
        if (pret && pret->init()) {
            pret->autorelease();
            return pret;
        };
        CC_SAFE_DELETE(pret);
        return nullptr;
    }
    void pushLayer(float duration, CCLayer* layer) {
        auto scene = CCScene::create();
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(duration, scene));
    }
    void onBtn(CCObject* h) {
        //it might be __pad48 but idk
        auto searchlay = static_cast<LevelSearchLayer*>(static_cast<CCNode*>(h)->getParent()->getParent());
        auto searchObj = searchlay->getSearchObject(SearchType::Search, gd::string(searchlay->m_searchInput->getString()));

        getTS;
        this->pushLayer(transitionSpeed, PerformRandomLevel::create());
        #define ccstr(s) CCString::create(s)
        #define ccint(i) CCInteger::create(i)
        #define ccbool(b) CCInteger::create(int(b))
        #define add(k, v) the->setObject(v,k)
        CCDictionary* the = CCDictionary::create();
        add("secret", ccstr("Wmfd2893gb7"));
        add("gameVersion", ccint(21));
        add("binaryVersion", ccint(35));
        add("gdw", ccint(0));
        add("type", ccint(0));
        add("page", ccint(1));
        add("str", ccstr(searchObj->m_searchQuery));
        add("uncompleted", ccbool(searchObj->m_uncompletedFilter));
        add("onlyCompleted", ccbool(searchObj->m_completedFilter));
        add("featured", ccbool(searchObj->m_featuredFilter));
        add("original", ccbool(searchObj->m_originalFilter));
        add("twoPlayer", ccbool(searchObj->m_twoPlayerFilter));
        add("coins", ccbool(searchObj->m_coinsFilter));
        add("epic", ccbool(searchObj->m_epicFilter));
        if (searchObj->m_starFilter) add("star", ccint(1));
        if (searchObj->m_customSongFilter) {
            add("song", ccint(searchObj->m_songID));
            add("customSong", ccbool(searchObj->m_customSongFilter));
        };
        //i hope this one exist
        CCHttpRequest* httpreq = new (std::nothrow) CCHttpRequest();
        std::string url = Mod::get()->getSettingValue<std::string>("gdps") + "/getGJLevels21.php";
        httpreq->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        httpreq->setUrl(url.c_str());
        auto stringified = this->CCDictDump(the).c_str();
        httpreq->setRequestData(stringified, sizeof(stringified));
        httpreq->setResponseCallback(this, httpresponse_selector(PerformRandomLevel::response));
        CCHttpClient::getInstance()->send(httpreq);
    }
    void response(CCHttpClient* client, CCHttpResponse* response) {
        auto responseData = response->getResponseData();
        std::string egg(responseData->begin(), responseData->end());
        gd::string iforgor(egg);
        
        DS_Dictionary* stringmanager = new DS_Dictionary();
        auto levelObj = GJGameLevel::create();
        
        //levels, authors, songs
        std::vector<gd::string> splitted = split(iforgor, '#');
        

        //split levels response
        std::vector<gd::string> wtf = split(splitted[0], '|');

        //get random level
        gd::string level = *select_randomly(wtf.begin(), wtf.end());
        std::vector<gd::string> mumei = split(level, ':');

        for (int i = 0; i < mumei.size(); i++) {
            if (i % 2 == 0) {
                switch (std::stoi(mumei[i])) {
                case 1:  levelObj->m_levelID = std::stoi(mumei[i + 1]);
                case 2:  levelObj->m_levelName = mumei[i + 1];
                case 3:  levelObj->m_levelDesc = mumei[i + 1];
                case 4:  levelObj->m_levelString = mumei[i + 1];
                case 5:  levelObj->m_levelVersion = std::stoi(mumei[i + 1]);
                case 10: levelObj->m_downloads = std::stoi(mumei[i + 1]);
                case 12: levelObj->m_audioTrack = std::stoi(mumei[i + 1]);
                case 13: levelObj->m_likes = std::stoi(mumei[i + 1]);
                }
            }
        }
        
        getTS;
        this->pushLayer(transitionSpeed, LevelInfoLayer::create(levelObj));
    }
    //helper funcs
    std::string CCDictDump(CCDictionary* dict) {
        std::string mim = "{ # }";
        #define contains(item, arr) std::find(arr.begin(), arr.end(), item) != arr.end()
        //hardcoding time
        CCDictElement* element;
        std::vector<std::string> stritems = { "secret", "str" };
        std::vector<std::string> intitems = { "gameVersion", "binaryVersion", "type", "gdw", "page", "uncompleted", "onlyCompleted", "epic", "coins", "featured", "twoPlayer", "song", "customSong", "original", "star" };
        std::string kv = "";
        CCDICT_FOREACH(dict, element) {
            std::string the = element->getStrKey();
            kv += "\"" + the + "\" :";
            if (contains(the, stritems)) {
                kv += "\"";
                kv += static_cast<CCString*>(element->getObject())->getCString();
                kv += "\"";
            }
            else if (contains(the, intitems)) {
                kv += std::to_string(static_cast<CCInteger*>(element->getObject())->getValue());
            }
            kv +=", ";
        }
        this->replace(mim, "#", kv);
        return mim;
    }
    bool replace(std::string& str, const std::string& from, const std::string& to) {
        size_t start_pos = str.find(from);
        if (start_pos == std::string::npos)
            return false;
        str.replace(start_pos, from.length(), to);
        return true;
    }
};

class $modify(LevelSearchLayer) {
    bool init() {
        if (LevelSearchLayer::init()) {
            auto searchbg = static_cast<CCScale9Sprite*>(this->getChildByID("level-search-bg"));
            searchbg->setContentSize(searchbg->getContentSize() + CCSize{ 30,0 });
            auto searchbtnmenu = this->getChildByID("search-button-menu");
            auto rndbtnmenu = CCMenuItemSpriteExtra::create(CCSprite::create("GJ_gradientBG.png"), this, menu_selector(PerformRandomLevel::onBtn));//temporary sprite
            rnmdbtnmenu->setPosition(searchbtnmenu->getChildByID("search-user-button")->getPosition() + CCPoint{5,0});
            searchbtnmenu->addChild(rndbtnmenu);
            searchbtnmenu->setPosition(searchbtnmenu->getPosition() - CCPoint{30, 0});
            #define move(node, point) node->setPosition(node->getPosition() + point)
            move(searchbtnmenu->getChildByID("level-search-bar-bg"), CCPoint{-30,0});
            move(searchbtnmenu->getChildByID("search-bar"), CCPoint{-30,0});
            return true;
        }
        return false;
    }
};
