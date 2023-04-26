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
#define ccsprwfn(sprite) CCSprite::createWithSpriteFrameName(sprite)
#define ccmise(sprite,target,selector) CCMenuItemSpriteExtra::create(sprite,target,selector)
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
        std::vector<gd::string> header = {""};
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
            searchbg->setContentSize(searchbg->getContentSize() + CCSize{ 37,0 });
            auto searchbtnmenu = this->getChildByID("search-button-menu");
            auto rndbtnmenu = ccmise(CCSprite::create("GJ_gradientBG.png"), this, menu_selector(PerformRandomLevel::onBtn));//temporary sprite
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
    std::string desc;
};

#define createOptionList(name, data) \
std::vector<option> name; \
std::vector<std::vector<std::string>> GEODE_CONCAT(name, Data) = data

#define protect(...) __VA_ARGS__
createOptionList(gameplay, protect({
    {"Fast practice reset", "Reset time after crash in practice mode lowered from 1s t 0.5s", "0052"},
    {"Practice Death Effect", "Show special death effects in practice mode", "0100"},
    {"Quick checkpoint mode", "Tries to place checkpoints more often while in practice mode.", "0068"},
    {"Switch spider teleport color", "Toggle between using main/secondary color for spider teleport effect.","0061"},
    {"Switch dash fire color", "Toggle between using main/secondary color for dash fire effect.", "0062"},
    {"Switch wave trail color", "Toggle between using main/secondary color for wave trail.", "0096"},
    {"Just, dont...", "This option doesn't do anything... Well, nothing useful.", "0095"},
    {"Default mini icon", "Player icon in mimi mode is set to default", "0060"}
}));
createOptionList(perf, protect({
    {"High StartPos accuracy", "Increases the accuracy of start position calculations, but loading a start position takes longer.", "0067"}, 
    {"Smooth Fix", "Makes some optimizations that can reduce lag. Disable if game speed becomes inconsistent.", "0023"},
    {"Force Smooth Fix", "Smooth fix is normally disabled if a level is lagging, this forces smooth fix to remain enabled. Toggle to test if the performance is better with smooth fix always enabled.", "0101"},
    {"Enable move optimization", "Optimize some moving objects. Increases performance but can create some minor visual glitches.", "0065"},
    {"High Capacity Mode", "Increases draw capacity for batch nodes at level start. Use to improve performance on some levels. May cause issues on low-end devices.", "0066"},
    {"Load songs to memory", "Songs are loaded to memory before playing. Increase load time but can improve performance.", "0019"},
    {"Smooth fix in editor", "Enabled Smooth Fix while playtesting in the editor.", "0102"}
}));
createOptionList(access, protect({
    {"Flip 2-Player Controls","Flip which side controls which player during 2-player dual mode.","0010"},
    {"Always Limit Controls", "Player 1 controls are limited to one side even if the dual mode is inactive.","0011"},
    {"Disable explosion shake", "", "0014"},
    {"Disable shake effects", "", "0081"},
    {"Disable gravity effect", "", "0072"},
    {"Disable Thumbstick", "Disable mouse movement using the controller thumb stick.", "0028"},
    {"Auto-Retry", "", "0026"},
    {"Show restart button", "Always show the restart button on the pause screen.", "0074"},
    {"Show Percentage", "Show the current percentage next to the progress bar.", "0040"},
    {"Auto-Checkpoints", "Automatically place checkpoints while in practice mode.", "0027"},
    {"Flip pause button", "", "0015"}
}));
createOptionList(online, protect({
    {"Increase max levels", "Increases the maximum saved levels from 20 to 100.", "0042"},
    {"Inc Local Levels Per Page","Increases Created/Saved levels per page from 10 to 20.", "0093"},
    {"Show leaderboard percent","To upload your level progress to the Level Leaderboard in 2.11 you need to replay levels completed before 2.11. This option toggles viewing the Leaderboard percentage you have on the levels.","0099"},
    {"Disable high object alert", "The alert showed when trying to play levels with a high object count is removed.", "0082"}
}));



#define paginatorSetup \
int focus = 0; \
std::vector<CCLayer*> pages; \
CCMenu* paginatorMenu;\
void paging(CCObject* b) {\
    auto btn = static_cast<CCMenuItem*>(b); \
    int side = btn->getTag(); \
    pages[focus]->setVisible(false); \
    focus += side; \
    pages[focus]->setVisible(true); \
    if (focus == 0 || focus == pages.size()) {btn->setVisible(false);} \
    else {paginatorMenu->getChildByTag(-side)->setVisible(true);} \
}

#define pagina(id, cls) \
auto GEODE_CONCAT(paginator, id) = ccmise(CCSprite::createWithSpriteFrameName("GJ_backBtn_001.png"),this,menu_selector(cls::paging)); \
x = 20.5; \
side = -1; \
if (id == 2) { \
    x = ws.width - 20.5;\
    side = 1; \
} \
GEODE_CONCAT(paginator, id)->setPosition(x,ws.height/2); \
GEODE_CONCAT(paginator, id)->setScaleX(side); \
paginatorMenu->addChild(GEODE_CONCAT(paginator, id))

#define create_paginator(cls) \
float x; int side;\
pagina(1, cls);\
pagina(2, cls)


//-160, 80 (toggler pos)
//14,+16y (info btn)
//14 (label)
//48 (gap)

// https://www.programiz.com/cpp-programming/multilevel-multiple-inheritance#:~:text=C%2B%2B%20Multiple%20Inheritance
class CategorizedOptionsLayer : public geode::Popup<std::string, std::vector<option>> {
protected:
    paginatorSetup;
    int x = -160, y = 80, optsInPage = 0;

    bool setup(std::string name, std::vector<option> opt) override {
        this->setTitle(name, "bigFont.fnt");
        for (option i : opt) {
            this->addToggle(i);
        }
        auto ws = CCDirector::sharedDirector()->getWinSize();
        create_paginator(CategorizedOptionsLayer);
        return true;
    }
    void addToggle(option opt) {
        auto ws = CCDirector::sharedDirector()->getWinSize();
        
        auto toggler = CCMenuItemToggler::create(
            ccsprwfn("GJ_checkOff_001.png"),
            ccsprwfn("GJ_checkOn_001.png"),
            this, menu_selector(CategorizedOptionsLayer::onToggle));
        toggler->setUserObject(ccstr(opt.key));
        toggler->setPosition(x,y);

        auto optName = CCLabelBMFont::create(opt.name, "bigFont.fnt");
        optName->setPosition(x-14,y);
        optName->setAnchorPoint(ccp(0,0.5));

        if (focus == pages.size()) {
            auto page = CCLayer::create();
            page->setAnchorPoint(ccp(0.5,0.5));
            pages.push_back(page);
            m_mainLayer->addChild(page);
            focus++;
        };
        CCMenu* menu;
        auto memi = getChildOfType<CCMenu>(pages[focus], 0);
        if (memi == nullptr) {
            menu = CCMenu::create();
            menu->setAnchorPoint(ccp(0.5,0.5));
            pages[focus]->addChild(menu);
        } 
        else {
            menu = getChildOfType<CCMenu>(pages[focus], 0);
        };
        
        if (opt.desc != "") {
            auto info = ccmise(ccsprwfn("GJ_infoIcon_001.png"), this, menu_selector(CategorizedOptionsLayer::onDesc));
            info->setContentSize(CCSize{11.5, 11.5});
            info->setUserObject(ccstr(opt.desc));
            info->setPosition(x-14, y+16);
            menu->addChild(info);
        }
        
        menu->addChild(toggler);
        pages[focus]->addChild(optName);
        optsInPage++;

        if (x == -160) {x = 14;}
        else {
            x = -328;
            y -= 48;
        }
    }
    void onToggle(CCObject* b) {
        auto btn = static_cast<CCMenuItem*>(b);
        std::string target = "gv_" + std::string(static_cast<CCString*>(btn->getUserObject())->getCString());
        GameManager::sharedState()->setGameVariable(target.c_str(), !btn->isSelected());
        btn->isSelected() ? btn->unselected() : btn->selected();
    }
    void onDesc(CCObject* b) {
        FLAlertLayer::create("Info", static_cast<CCString*>(static_cast<CCMenuItem*>(b)->getUserObject())->getCString(), "OK")->show();
    }
public:
    static CategorizedOptionsLayer* create(std::string name, std::vector<option> opt) {
        auto ret = new CategorizedOptionsLayer();
        if (ret && ret->init(284.f, 160.f, name, opt)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

class CategorizedOptionsEntry : public geode::Popup<int> {
private:
    int x = -80, y = 60;
    paginatorSetup

    class OffsetListener : public TextInputDelegate {
        virtual void textInputClosed(CCTextInputNode* h) {
            GameManager::sharedState()->setIntGameVariable("timeOffset",std::stoi(h->getString()));
        };
    };

    void movePos() {
        if (x == 80) y -= 48;
        x = -x;
    };

    void loadOption(CCObject*b){
        std::vector<option> data;
        std::string opt = static_cast<CCString*>(static_cast<CCMenuItem*>(b)->getUserObject())->getCString();

        if (opt == "Gameplay") {
            data = gameplay;
        } else if (opt == "Performance") {
            data = perf;
        } else if (opt == "Accessibility") {
            data = access;
        } else if (opt == "Online") {
            data = online;
        }

        CategorizedOptionsLayer::create(opt, data)->show();
    }
    void addOption(std::string name) {
        auto btn = ccmise(ButtonSprite::create(name.c_str()),this,menu_selector(CategorizedOptionsEntry::loadOption)); 
        btn->setAnchorPoint(ccp(0.5,0.5));
        btn->setUserObject(ccstr(name));
        pages[0]->addChild(btn);
    }
    bool setup(int u) override {
        this->setTitle("Options");

        auto l = CCLayer::create();
        l->setAnchorPoint(ccp(0.5,0.5));
        m_mainLayer->addChild(l);
        pages.push_back(l);
        for (auto&i : {"Gameplay", "Performance", "Accessibility", "Online"}) {
            this->addOption(i);
            this->movePos();
        }

        auto extl = CCLayer::create();
        extl->setAnchorPoint(ccp(0.5,0.5));
        pages.push_back(extl);
        m_mainLayer->addChild(extl);
        focus++;

        auto msLabel = CCLabelBMFont::create("Music offset (MS)", "bigFont.fnt");
        msLabel->setPosition(-125,-80);
        extl->addChild(msLabel);

        auto moBox = CCTextInputNode::create(100,50,"Offset","bigFont.fnt");
        CategorizedOptionsEntry::OffsetListener* delegate;
        moBox->setDelegate(delegate);
        moBox->setPosition(-185.5,-110);
        extl->addChild(moBox);
        
        //todo: 
        //- option 1: add parental options layer
        //- option 2: copy the last page in original options layer
        //auto parental = ccmise(ccsprwfn("GJ_profileButton_001.png"),this,menu_selector())
    }
public:
    static CategorizedOptionsEntry* create() {
        auto ret = new CategorizedOptionsEntry();
        if (ret && ret->init(284.f, 160.f, 1)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};
