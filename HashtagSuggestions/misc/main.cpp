#include <Geode/Geode.hpp>
#include <Geode/modify/CCTransitionFade.hpp> //only this transition is available fsr
#include <Geode/cocos/extensions/network/HttpRequest.h>
#include <Geode/cocos/extensions/network/HttpResponse.h>
#include <Geode/cocos/extensions/network/HttpClient.h>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <Geode/modify/MoreOptionsLayer.hpp>
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

template <typename vectelemtype>
std::vector<std::string> split(vectelemtype& s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss(s);
    std::string word;
    while (!ss.eof()) {
        std::getline(ss, word, delim);
        elems.push_back(word);
    }
    return elems;
}

class PerformRandomLevel : public CCLayer {
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
    void onBtn(CCObject* h) {
        //it might be __pad48 but idk
        auto searchlay = static_cast<LevelSearchLayer*>(static_cast<CCNode*>(h)->getParent()->getParent());
        auto searchObj = searchlay->getSearchObject(SearchType::Search, gd::string(searchlay->m_searchInput->getString()));

        geode::log::info("we passes this point");
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
        CCHttpRequest* httpreq = new CCHttpRequest();
        std::string url = Mod::get()->getSettingValue<std::string>("gdps") + "/getGJLevels21.php";
        auto stringified = this->CCDictDump(the);
        std::vector<gd::string> header = { "Content-Type: application/x-www-form-urlencoded", "Content-Length: "+std::to_string(stringified.length())};
        httpreq->setRequestType(CCHttpRequest::HttpRequestType::kHttpPost);
        httpreq->setUrl(url.c_str());
        httpreq->setHeaders(header);
        httpreq->setRequestData(stringified.c_str(), stringified.length());
        httpreq->setResponseCallback(this, httpresponse_selector(PerformRandomLevel::response));
        geode::log::info(stringified);
        CCHttpClient::getInstance()->send(httpreq);
    }
    void response(CCHttpClient* client, CCHttpResponse* response) {
        auto responseData = response->getResponseData();
        std::string egg(responseData->begin(), responseData->end());
        
        DS_Dictionary* stringmanager = new DS_Dictionary();
        auto levelObj = GJGameLevel::create();
        
        //levels, authors, songs
        std::vector<std::string> splitted = split(egg, '#');
        

        //split levels response
        std::vector<std::string> wtf = split(splitted[0], '|');

        //get random level
        std::string level = *select_randomly(wtf.begin(), wtf.end());
        std::vector<std::string> mumei = split(level, ':');

        geode::log::info(egg);
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
        auto scene = CCScene::create();           
        auto layer = LevelInfoLayer::create(levelObj);
        scene->addChild(layer);
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(transitionSpeed, scene)); 
    }
    //helper funcs
    std::string CCDictDump(CCDictionary* dict) {
        #define contains(item, arr) std::find(arr.begin(), arr.end(), item) != arr.end()
        //hardcoding time
        CCDictElement* element;
        std::vector<std::string> stritems = { "secret", "str" };
        std::vector<std::string> intitems = { "gameVersion", "binaryVersion", "type", "gdw", "page", "uncompleted", "onlyCompleted", "epic", "coins", "featured", "twoPlayer", "song", "customSong", "original", "star" };
        std::string kv = "";

        int h = 0;
        int elems = dict->allKeys()->count();
        CCDICT_FOREACH(dict, element) {
            std::string the = element->getStrKey();
            kv += the + "=";
            if (contains(the, stritems)) {
                kv += static_cast<CCString*>(element->getObject())->getCString();
            }
            else if (contains(the, intitems)) {
                kv += std::to_string(static_cast<CCInteger*>(element->getObject())->getValue());
            }
            if (h != elems-1) kv +="&";
            h++;
        }
        return kv;
    }
};

class $modify(LevelSearchLayer) {
    bool init() {
        if (LevelSearchLayer::init()) {
            auto searchbg = static_cast<CCScale9Sprite*>(this->getChildByID("level-search-bg"));
            searchbg->setContentSize(searchbg->getContentSize() + CCSize{ 30,0 });
            auto searchbtnmenu = this->getChildByID("search-button-menu");
            auto rndbtnmenu = CCMenuItemSpriteExtra::create(CCSprite::create("GJ_gradientBG.png"), this, menu_selector(PerformRandomLevel::onBtn));//temporary sprite
            rndbtnmenu->setPosition(searchbtnmenu->getChildByID("search-user-button")->getPosition() + CCPoint{50,0});
            searchbtnmenu->addChild(rndbtnmenu);
            searchbtnmenu->setPosition(searchbtnmenu->getPosition() - CCPoint{30, 0});
            #define move(node, point) node->setPosition(node->getPosition() + point)
            auto wtf = CCPoint{ -30,0 };
            move(this->getChildByID("level-search-bar-bg"), wtf);
            move(this->getChildByID("search-bar"), wtf);
            return true;
        }
        return false;
    }
};

struct option {
    const char* name;
    const char* key;
    const char* info;
};
std::vector<option> gameplay;
std::vector<std::string> gameplayOpts = {
    "Auto-Retry",
    "Show restart button",
    "Auto-Checkpoints",
    "Fast practice reset",
    "Practice Death Effect",
    "Show Percentage",
    "High StartPos accuracy",
    "Quick checkpoint mode",
    "Disable shake effects",
    "Switch spider teleport color",
    "Switch dash fire color",
    "Switch wave trail color",
    "Enable move optimization",
    "Just, dont...",
    "Flip 2-Player Controls",
    "Always Limit Controls",
    "Disable explosion shake"
};
//-160
//32
//14,+16y
class CategorizedOptionsLayer : public geode::Popup<std::string, std::vector<option>> {
protected:
    std::vector<option> options;
    std::vector<CCLayer*> pages;
    int xpos, ypos;
    bool setup(std::string name, std::vector<option> opts) override {
        getChildOfType<CCLabelBMFont>(getChildOfType<CCLayer>(this,0),0);
        options = opts;
        return true;
    }
public:
    void addToggle(const char* name, const char* key, const char* info) {
        for (auto& i : options) {
            if (name == i.name) {
                auto toggler = CCMenuItemToggler::create(
                    CCSprite::create("GJ_checkOff_001.png"),
                    CCSprite::create("GJ_checkOff_001.png"),
                    this,
                    menu_selector(CategorizedOptionsLayer::egg)
                );
                toggler->setAnchorPoint(CCPoint{0.5f,0.5f});
                toggler->setUserObject(CCString::create(info));
            };
        }
    };
    void egg(CCObject*b) {

    }
};

class $modify(MoreOptionsLayer) {
    void addToggle(const char* name, const char* key, const char* info) {
        /*
#define h(opt)  \
if (std::find(GEODE_CONCAT(opt,Opts).begin(),GEODE_CONCAT(opt,Opts).end(),name) != GEODE_CONCAT(opt,Opts).end()) { \
    option o;  \
    o.name=name;  \
    o.key=key; \
    o.info=info;  \
    opt.push_back(o); \
}

    h(gameplay);
    */
    std::string h = "\nName: " + std::string(name) + "\nKey: " + std::string(key);// + "\nInfo: " + std::string(info);
    geode::log::info(h);
    }
/*
    bool init() {
        bool h = MoreOptionsLayer::init();
        this->removeFromParent();
        return h;
    };

    static MoreOptionsLayer* create() {
        auto h = new MoreOptionsLayer()
    }*/
};
