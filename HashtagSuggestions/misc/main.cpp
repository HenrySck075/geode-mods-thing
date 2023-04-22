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
    std::string desc;
};
#define createOptionList(name, list, desc) \
std::vector<option> name; \
std::vector<std::string> GEODE_CONCAT(name, Opts) = list;\
std::vector<std::string> GEODE_CONCAT(name, Desc) = desc

createOptionList(gameplay, {
    "Fast practice reset",
    "Practice Death Effect",
    "Show Percentage",
    "Quick checkpoint mode",
    "Switch spider teleport color",
    "Switch dash fire color",
    "Switch wave trail color",
    "Just, dont...",
    "Default mini icon",
}, {
    "Reset time after crash in practice mode lowered from 1s t 0.5s",
    "Show special death effects in practice mode",
    "Show the current percentage next to the progress bar.",
    "Tries to place checkpoints more often while in practice mode.",
    "Toggle between using main/secondary color for spider teleport effect.",
    "Toggle between using main/secondary color for dash fire effect.",
    "Toggle between using main/secondary color for wave trail.",
    "This option doesn't do anything... Well, nothing useful.",
    "Player icon in mimi mode is set to default"
});
createOptionList(perf, {
    "High StartPos accuracy",
    "Smooth Fix",
    "Force Smooth Fix",
    "Enable move optimization",
    "High Capacity Mode",
    "Load songs to memory",
    "Smooth fix in editor"
}, {
    "Increases the accuracy of start position calculations, but loading a start position takes longer.",
    "Makes some optimizations that can reduce lag. Disable if game speed becomes inconsistent.",
    "Smooth fix is normally disabled if a level is lagging, this forces smooth fix to remain enabled. Toggle to test if the performance is better with smooth fix always enabled.",
    "Optimize some moving objects. Increases performance but can create some minor visual glitches.",
    "Increases draw capacity for batch nodes at level start. Use to improve performance on some levels. May cause issues on low-end devices.",
    "Songs are loaded to memory before playing. Increase load time but can improve performance.",
    "Enabled Smooth Fix while playtesting in the editor."
});
createOptionList(access, {
    "Flip 2-Player Controls",
    "Always Limit Controls",
    "Disable explosion shake",
    "Disable shake effects",
    "Disable gravity effect"
    "Auto-Retry",
    "Show restart button",
    "Auto-Checkpoints",
    "Flip pause button",
    "Higher audio quality\n(req. restart)",
}, {
    "Flip which side controls which player during 2-player dual mode.",
    "Player 1 controls are limited to one side even if the dual mode is inactive.",
    "",
    "",
    "",
    "",
    "Always show the restart button on the pause screen.",
    "Automatically place checkpoints while in practice mode.",
    "",
    "Switch sample rate from 24000 to 44100",
});
createOptionsList(online, {
    "Increase max levels",
    "Inc Local Levels Per Page",
    "Show leaderboard percent",
    "Disable high object alert",
},{
    "Increases the maximum saved levels from 20 to 100.",
    "Increases Created/Saved levels per page from 10 to 20.",
    "To upload your level progress to the Level Leaderboard in 2.11 you need to replay levels completed before 2.11. This option toggles viewing the Leaderboard percentage you have on the levels.",
    "The alert showed when trying to play levels with a high object count is removed.",

});
//-160
//32
//14,+16y
class CategorizedOptionsLayer : public geode::Popup<std::string, std::vector<option>> {
protected:
    std::vector<option> options;
    std::vector<CCLayer*> pages;
    int focus = -1;
    int xpos = -160, ypos = 80;
    bool setup(std::string name, std::vector<option> opts) override {
        this->setTitle(this);
        options = opts;
	auto kbtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Keys", "goldFont.png", "GJ_button_04.png"),this,menu_selector(this->what));
	kbtn->setPosiion(CCPoint{150,110});

	auto winWidth = CCDirector::sharedDirector()->getWinSize().width;

	int pos;

	auto pager = CCMenu::create();
	pager->setID("idk");
#define itltcp(scale,id) \
auto spr = CCSprite::createWithSpriteFrameName("GJ_backBtn_001.png"); \
spr->setScaleX((float) scale); \
spr->setTag(scale); \
auto pageExplorer##id = CCMenuItemSpriteExtra::create(spr,this,menu_selector()); \
if (scale == 1) { \
    pos = 36; \
} \
else { \
    pos = winWidth - 36; \
}; \
pager->addChild(pageExplorer##id);

	itltcp(1,1);
	itltcp(-1,2);
	for (option o : options) {
	    this->addToggle(o);
	}

        m_mainLayer->addChild(kbtn);
	m_mainLayer->addChild(pager);
	m_mainLayer->setAnchorPoint(CCPoint{0.5f,0.5f});
        return true;
    }
    void what(CCObject*n) {
	KeybindingsLayer::create("what","is","love")->show();
    }
    void no(CCObject*h) {};
public:
    void movePage(CCObject*butter) {
	pages[focus]->setVisible(false);
	focus += static_cast<CCMenuItem*>(butter)->getTag();
	pages[focus]->setVisible(true);

	if (focus == 0) {
	    getChildOfType<CCMenuItemSpriteExtra>(m_mainLayer->getChildByID("idk"),0)->setVisible(false);
	} else if (focus == pages.size() - 1) {
            getChildOfType<CCMenuItemSpriteExtra>(m_mainLayer->getChildByID("idk"),1)->setVisible(false);
	} else {
	    CCObject*s;
	    CCARRAY_FOREACH(m_mainLayer->getChildByID("idk")->getChildren(), s) {
		auto btn = static_cast<CCMenuItem*>(s);
		btn->setVisible(true);
	    }
	}
    }
    void addToggle(option opt) {
        auto toggler = CCMenuItemToggler::create(
            CCSprite::create("GJ_checkOff_001.png"),
            CCSprite::create("GJ_checkOff_001.png"),
            this,
            menu_selector(CategorizedOptionsLayer::egg)
        );
        toggler->setAnchorPoint(CCPoint{0.5f,0.5f});
        toggler->setUserObject(CCString::create(opt.key));
	toggler->setPosition(0,0);

	auto label = CCLabelBMFont::create(opt.name, "bigFont.fnt");
	label->setAnchorPoint(CCPoint{0,0.5});
	label->setPosition(26, 0);

	auto menu = CCMenu::create();
	menu = setAnchorPoint(CCPoint{0.5,0.5});
	menu->setPosition(xpos, ypos);
	menu->addChild(toggler);
	if (opt.desc != "") {
	    auto info = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_infoBtn_001.png"),this,menu_selector());
	    info->setPosition(-18,16);
	    menu->addChild(info);
	};
	menu->addChild(label);
#define ihaveto \
auto layer = CCLayer::create(); \
layer->setAnchorPoint(CCPoint{0.5,0.5}); \
layer->addChild(menu); \
pages->push_back(layer); \
focus++;

	//if no page created
	if (focus == -1) {
	    ihaveto
	}
	//else if there's 10 options in a page
	else if (pages[focus]->getChildrenCount() == 10) {
	    ihaveto
	}
	//else
	else {
	    pages[focus]->addChild(menu);
	}

	//lastly, change the position variables
	if (xpos == -160) {
	    xpos = 32;
	}
	else {
	    xpos = -160;
	    ypos -= 60;
	}
    };
    void rice(CCObject*b){
	FLAlertLayer::create("Info", static_cast<CCString>(static_cast<CCMenuItem*>(b)->getUserObject())->getCString(), "OK")->show();
    }
    void egg(CCObject*b) {
	auto btn = static_cast<CCMenuItemToggler*>(b);
	auto info = static_cast<CCString>(->getUserObject())->getCString();
	GameManager::sharedState()->setGameVariable(info, btn->isToggled());
    }
};

class $modify(MoreOptionsLayer) {
    void addToggle(const char* name, const char* key, const char* info) {
        
#define h(opt)  \
auto begin = GEODE_CONCAT(opt,Opts).begin(); \
auto idx = std::find(begin,GEODE_CONCAT(opt,Opts).end(),name); \
if (idx != GEODE_CONCAT(opt,Opts).end()) { \
    int location = idx - begin;\
    option o;  \
    o.name=name;  \
    o.key=key; \
    o.info=info;  \
    o.desc = GEODE_CONCAT(opt, Desc)[location]; \
    opt.push_back(o); \
}

    h(gameplay);
    h(perf);
    h(access);
    h(online);
    
    std::string h = "\nName: " + std::string(name) + "\nKey: " + std::string(key);
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
