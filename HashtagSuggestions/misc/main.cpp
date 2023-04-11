#include <Geode/Geode.hpp>
#include <Geode/modify/CCTransitionFade.hpp> //only this is available fsr
#include <Geode/utils/cocos.hpp>
#include <Geode/modify/LevelSearchLayer.hpp>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace geode::prelude;
#define setTS float transitionSpeed = (float) Mod::get()->getSettingValue<double>("transition-speed");
class $modify(CCTransitionFade) {
    virtual bool initWithDuration(float d, CCScene* s, ccColor3B const& c) {
        setTS
        return CCTransitionFade::initWithDuration(transitionSpeed,s,c);
    }
};
//event listener broke in beta.13
/*
$execute {
    listenForSettingChanges("transition-speed", +[](double v) {
        float transitionSpeed = (float) v;
    });
}
*/

class PerformRandomLevel : public CCLayer {
protected:
    bool init() {
        auto winSize=CCDirector::sharedDirector()->getWinSize();

        auto bg = CCSprite::create("GJ_gradientBG.png");
        auto bgSize = bg->getContentSize();

        bg->setScale(winSize.width / bgSize.width, winSize.height / bgSize.height);
        bg->setAnchorPoint(CCPoint{0,0});
        bg->setZOrder(-1);

        auto thetext = CCLabelBMFont::create("Getting random level", "bigFont.fnt");
        thetext->setContentSize(thrtext->getContentSize() * 1.5f);
        thetext->setPosition(winSize / 2 + CCPoint{0,30});


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
    void pushLayerWithTransition(float duration, CCLayer* layer) {
        auto scene = CCScene::create();
        scene->addChild(layer);
        CCDirector->sharedDirector()->pushSceneWithTransition(CCTransitionFade::create(duration, scene));
    }
    void onBtn(CCObject*h ) {
        //it might be __pad48 but idk
        auto searchlay = static_cast<LevelSearchLayer*>(static_cast<CCNode*>(h)->getParent()->getParent());
        auto searchObj = searchlay->getSearchObject(SearchType::Search, gd::string(searchlay->m_searchInput->getString());

        setTS
        this->pushLayerWithTransition(transitionSpeed, PerformRandomLevel::create());

        json the;
        the["secret"] = "Wmfd2893gb7";
        the["type"] = 0;
        the["page"] = 1;
        the["uncompleted"] = int(searchObj->m_uncompletedFilter);
        the["onlyCompleted"] = int(searchObj->m_completedFilter);
        the["featured"] = int(searchObj->m_featuredFilter);
        the["original"] = int(searchObj->m_originalFilter);
        the["twoPlayer"] = int(searchObj->m_twoPlayerFilter);
        the["coins"] = int(searchObj->m_coinsFilter);
        the["epic"] = int(searchObj->m_epicFilter);
        noStar - Unrated filter. Not sent when off, 1 for on.

        star - Rated filter. Not sent when off, 1 for on.

        song - Official song filter. Not sent when off.

        customSong - Enable newgrounds song filter, song parameter will be songID. Not sent when off.




        

    }
}