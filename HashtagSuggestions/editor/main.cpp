#include <Geode/Geode.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/SetGroupIDLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/SetupObjectTogglePopup.hpp>
#include <Geode/loader/Log.hpp>
#include <typeinfo>

//fuck you vscode
/*
auto theblocks = nlohmann::json::parse(R"(
{
	"1": [[1011, 0, 0, 180, 1010, 0], [1011, 0, 0, 180, 1010, 0], [507, 0, 20, 0, -1, 0], [507, -20, 0, -90, -1, 0], [507, 20, 0, 90, -1, 0], [507, 0, -10, 180, -1, 0]]
}
)");
*/
using namespace geode::prelude;
bool inPlaytest = false;
/*
#define getval(key,type) static_cast<type*>(current_dict->objectForKey(key))
#define setval(key,val) current_dict->setObject(val,key)
#define convertToCocos(obj, var) auto var = CCInteger::create(obj)
CCDictionary* e = new CCDictionary();
e

CCArray* createArray(std::vector<int> vect) {
    auto arry = CCArray::create();
    for (int& i : vect) {
        convertToCocos(i, item);
        arry->addObject(item);
    }
    return arry;
};

//default to 2 array layers
CCArray* createNestedArray(std::vector<std::vector<int>> vect) {
    auto arry = CCArray::create();
    for (std::vector<int>& i : vect) {
        auto item = createArray(i);
        arry->addObject(item);
    }
    return arry;
}

const char* b2s(bool thing) { return thing ? "true" : "false"; };

class FakeBlockPopup : public geode::Popup<CCArray*> {
protected:
    bool setup(CCArray* blocks) override {
        auto winSize = CCDirector::sharedDirector()->getWinSize();

        this->setTitle("Select faking method");
        auto d = winSize / 2;
        auto sep = CCSprite::create("gravityLine_001.png");
        sep->setPosition(d);
        sep->setScaleY(11.925f);
        sep->setScaleX(4.f);
        sep->setZOrder(12);

        auto recreateSect = CCMenu::create();
        recreateSect->setPosition(sep->getPosition() - CCSize{ 189,0 });
        recreateSect->setContentSize(CCSize{ 140,0 });

        auto recreateBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Recreate"), this, menu_selector(FakeBlockPopup::recreate));
        auto recreateBtnDesc = CCLabelBMFont::create("When you use this option, the mod\nwill recreate the object with\ndecoration objects.\nNote that not all objects are able\nto use this option,\nuse Scale instead.", "chatFont.fnt");
        recreateBtn->setPosition(recreateSect->getContentSize() / 2 + CCPoint{ 0,50 }); recreateBtn->setUserObject(blocks);
        recreateBtnDesc->setPosition(recreateSect->getContentSize() / 2 - CCPoint{ 0,20 });

        recreateSect->addChild(recreateBtn); recreateSect->addChild(recreateBtnDesc);

        auto scaleSect = CCMenu::create();
        scaleSect->setPosition(sep->getPosition() + CCSize{ 60,0 });
        scaleSect->setContentSize(CCSize{ 140,0 });

        auto scaleBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Scale"), this, menu_selector(FakeBlockPopup::scale));
        auto scaleBtnDesc = CCLabelBMFont::create("When you use this option, the mod\nwill resize the object to negative value.\nThis will make object's hitbox\n(almost) disappeared.", "chatFont.fnt");
        scaleBtn->setPosition(scaleSect->getContentSize() / 2 + CCPoint{ 0,50 }); scaleBtn->setUserObject(blocks);
        scaleBtnDesc->setPosition(scaleSect->getContentSize() / 2 - CCPoint{ 0,20 });

        scaleSect->addChild(scaleBtn); scaleSect->addChild(scaleBtnDesc);

        auto the = CCLabelBMFont::create("No-hitbox objects are ignored because why the heck you wanted to do that", "chatFont.fnt");
        the->setScale(0.7f); the->setPosition(d.width, 31);
        this->addChild(sep); this->addChild(scaleSect); this->addChild(recreateSect); this->addChild(the);

        return true;
    }
public:
    void recreate(CCObject* s) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(s);
        auto editorlayer = getChildOfType<LevelEditorLayer>(this->getParent(), 0);
        auto editorui = static_cast<EditorUI*>(editorlayer->getChildByID("EditorUI"));
        auto blocks = static_cast<CCArray*>(btn->getUserObject());
        CCObject* the;
        auto c = [&](int colid, GameObject* obj, int the) {
            if (colid == -1) {
                if (the == 0) { return obj->getBaseColor()->m_colorID; }
                else return obj->getDetailColor()->m_colorID;
            }
            else return colid;
        };
        auto e = [&](std::vector<short> gr) {
            std::string gs = "";
            for (int g : gr) {
                gs = gs + std::to_string(g) + ".";
            };
            return gs;
        };
        CCARRAY_FOREACH(blocks, the) {
            auto block = static_cast<GameObject*>(the);
            auto rebl = theblocks[std::to_string(block->m_objectID)];
            for (auto&d:rebl) {
                std::string group_id = "1," + std::to_string(0+d[0]);
                std::string x = ",2," + std::to_string(block->getPositionX() + d[1]);
                std::string y = ",3," + std::to_string(block->getPositionY() + d[2]);
                std::string rot = ",6," + std::to_string(block->getRotation() + d[3]);
                std::string basecol = ",21," + std::to_string(c(d[4], block, 0));
                std::string detailcol = ",22," + std::to_string(c(d[5], block, 1));
                std::string groups = ",51," + e(block->getGroupIDs());

                std::string objstr = group_id + x + y + rot + basecol + detailcol + groups + ";";
                auto  newblock = static_cast<GameObject*>(editorlayer->addObjectFromString(objstr));
                editorlayer->update(0);
            }
        };
        this->keyBackClicked();
    }
    void scale(CCObject* s) {
        auto btn = static_cast<CCMenuItemSpriteExtra*>(s);
        auto blocks = static_cast<CCArray*>(btn->getUserObject());
        CCObject* the;
        CCARRAY_FOREACH(blocks, the) {
            auto block = static_cast<GameObject*>(the);
            block->updateCustomScale(-(block->getScale()));
            block->setRotation(block->getRotation() + 180);
        };
        this->keyBackClicked();
    }
    static FakeBlockPopup* create(CCArray* blocks) {
        auto ret = new FakeBlockPopup();
        auto winSize = CCDirector::sharedDirector()->getWinSize() * 0.9f;
        if (ret && ret->init(520.f, winSize.height, blocks)) {
            ret->autorelease();
            return ret;
        };
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
    void fakeBlock(CCObject* s) {
        auto btn = static_cast<CCNode*>(s);
        auto editor = static_cast<EditorUI*>(btn->getParent()->getParent());
        auto objs = editor->getSelectedObjects();
        if (objs->count() == 0) FLAlertLayer::create("the", "select objects first", "ok")->show();
        else {
            auto s = FakeBlockPopup::create(objs);
            s->m_noElasticity = true;
            s->show();
        }
    }
};
*/

class CollapsableEditorToolbar : public CCLayer {
private:
    bool pinned = true;
public:
    static CollapsableEditorToolbar* create() {
        auto h = new CollapsableEditorToolbar();
        if (h && h->init()) {
            h->autorelease();
            return h;
        }
        CC_SAFE_DELETE(h);
        return nullptr;
    }
    //pin / unpin the toolbar
    void pinToolbar(CCObject* h) {
	    pinned = !pinned;
    }

    void collapse() {
        auto action = CCEaseBounceOut::create(CCMoveTo::create(0.5f, CCPoint{0,-90}));
        this->runAction(action);
    }
    void reveal() {
        auto action = CCEaseBounceOut::create(CCMoveTo::create(0.5f, CCPoint{0,90}));
        this->runAction(action);
    }
    virtual bool ccTouchBegan(CCTouch* t, CCEvent* thisdoesntexist) {
        /*
        if (pinned) {
            auto canvas = CCDirector::sharedDirector()->getWinSize();
            //why is this lives in ccnode
            auto curloc = this->convertToNodeSpace(t->getLocation());
            auto contentSize = this->getContentSize();
            CCRect* r = new CCRect();
            r->setRect(0, 0, contentSize.width, contentSize.height);
                r->containsPoint(curloc) ? this->reveal() : this->collapse();
        };
        */
        geode::log::info("hi");
        return CCLayer::ccTouchBegan(t,thisdoesntexist);
    }
};

class $modify(EditorPauseLayer) {
    bool init(LevelEditorLayer * h) {
        EditorPauseLayer::init(h);
        if (h->m_level->m_levelID == -4) {
            auto resumemenu = this->getChildByID("resume-menu");
            auto saepos = resumemenu->getChildByID("save-and-exit-button")->getPosition();
            resumemenu->getChildByID("exit-button")->setPosition(saepos - CCPoint{ 0, 20.25f });
            resumemenu->getChildByID("resume-button")->setPosition(saepos + CCPoint{ 0, 20.25f });
            resumemenu->removeChild(resumemenu->getChildByID("save-and-exit-button"));
            resumemenu->removeChild(resumemenu->getChildByID("save-and-play-button"));
            resumemenu->removeChild(resumemenu->getChildByID("save-button"));
            resumemenu->removeChild(resumemenu->getChildByID("exit-button"));
        }
        return true;
    }
    void onExitNoSave(CCObject*s) {
        EditorPauseLayer::onExitEditor(s);  
    }
};
class $modify(EditorUI) {
    bool init(LevelEditorLayer * h) {
        EditorUI::init(h);
        /*
        auto editorbuttons = this->getChildByID("editor-buttons-menu");
        auto the = CCSprite::create("GJ_button_01-uhd.png");
        the->setOpacity(50);
        auto fakeblockbtn = CCMenuItemSpriteExtra::create(the, the, this, menu_selector(FakeBlockPopup::fakeBlock));
        fakeblockbtn->setID("fakeblock-gen-button");
        fakeblockbtn->setPosition(editorbuttons->getChildByID("copy-button")->getPosition() - CCPoint{ 38,0 });
        editorbuttons->addChild(fakeblockbtn);
        */
        //cologne features
        auto wid=CCDirector::sharedDirector()->getWinSize().width;
        auto bottomEditLayer = CollapsableEditorToolbar::create();
        bottomEditLayer->setAnchorPoint(CCPoint{0,0});
        bottomEditLayer->setContentSize(CCSize{wid, 40});

        #define transferParent(node) \
        node->removeFromParentAndCleanup(false); \
        bottomEditLayer->addChild(node)

        auto bg = getChildOfType<CCSprite>(this, 0);
        auto line1 = getChildOfType<CCSprite>(this, 1);
        auto line2 = getChildOfType<CCSprite>(this, 2);
        auto delcat = this->getChildByID("delete-category-menu");
        auto toolbar = this->getChildByID("toolbar-toggles-menu");
        auto toolcat = this->getChildByID("toolbar-categories-menu");
        auto build = this->getChildByID("build-tabs-menu");

        transferParent(bg);
        transferParent(line1);
        transferParent(line2);
        transferParent(delcat);
        transferParent(toolbar);
        transferParent(toolcat);
        transferParent(build);

        while (true) {
            auto egg=getChildOfType<EditButtonBar>(this,0);
            if (egg == nullptr) break;
            transferParent(egg);
        };

        auto pinBtn = CCMenuItemToggler::create(
            CCSprite::create("GJ_button_01.png"),
            CCSprite::create("GJ_button_02.png"),
            this,
            menu_selector(CollapsableEditorToolbar::pinToolbar));
        pinBtn->setPosition(wid-34,95);
        pinBtn->setContentSize(CCSize{0.5f,0.5f});
        bottomEditLayer->addChild(pinBtn);
        this->addChild(bottomEditLayer);
        return true;

    };
    void onPlaytest(CCObject * s) {
        auto objlayer = getChildOfType<CCLayer>(this->getParent(), 0);
        auto the = objlayer->getChildByID("touch");
        if (the != nullptr) { objlayer->removeChild(the); };
        auto touchPoints = CCLayer::create();
        touchPoints->setID("touch");
        objlayer->addChild(touchPoints);
        inPlaytest = true;
        EditorUI::onPlaytest(s);
    }
    void onStopPlaytest(CCObject * s) {
        inPlaytest = false;
        EditorUI::onStopPlaytest(s);
    }
};
// i cant guarantee that this is working
class $modify(PlayerObject) {
    int m_circlsiz = Mod::get()->getSettingValue<int64_t>("playtest-circlesize");
    void pushButton(int s) {
        if (inPlaytest and Mod::get()->getSettingValue<bool>("playtest-touchindicate")) {
            auto poi = CCSprite::createWithSpriteFrameName("darkblade_01_color_001.png");
            poi->setColor(m_fields->m_playerColor1);//this is somehow incorrect, my icon is all blue how is there green
            poi->setPosition(this->getPosition());
            poi->setScale(m_fields->m_circlsiz / 10.f);
            this->getParent()->getParent()->getChildByID("touch")->addChild(poi);//
        };
        PlayerObject::pushButton(s);
    }
    void releaseButton(int s) {
        if (inPlaytest and Mod::get()->getSettingValue<bool>("playtest-releaseindicate")) {
            auto poi = CCSprite::createWithSpriteFrameName("darkblade_01_color_001.png");
            auto col = m_fields->m_playerColor1;
            poi->setColor(ccc3(255 - col.r, 255 - col.g, 255 - col.b));
            poi->setPosition(this->getPosition());
            poi->setScale(m_fields->m_circlsiz / 10.f);
            this->getParent()->getParent()->getChildByID("touch")->addChild(poi);//
        };
        PlayerObject::releaseButton(s);
    }
};

class TempLELCreate {
public:
    void onB(CCObject* idk) {
        auto e = CCScene::create();
        auto templevel = GJGameLevel::create();
        templevel->m_levelID = -4;
        e->addChild(LevelEditorLayer::create(templevel));
        CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f, e));
    }
};
class $modify(LevelBrowserLayer) {
    bool init(GJSearchObject * s) {
        LevelBrowserLayer::init(s);
        auto newlevlmenu = this->getChildByID("new-level-menu");
        if (newlevlmenu != nullptr and Mod::get()->getSettingValue<bool>("editor-temporarylevel")) {
            auto tempnewbutton = CCMenuItemSpriteExtra::create(CCSprite::createWithSpriteFrameName("GJ_newBtn_001.png"), this, menu_selector(TempLELCreate::onB));
            tempnewbutton->setPosition(newlevlmenu->getChildByID("new-level-button")->getPosition() + CCPoint{ 0,70 });
            newlevlmenu->addChild(tempnewbutton);
        };
        return true;
    }
};
class LastGroupIDWorkaround : public CCLayer {
public:
    int m_lastGroupIDAdded = 0;
    static LastGroupIDWorkaround* create() {
        auto ret = new LastGroupIDWorkaround();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        };
        CC_SAFE_DELETE(ret);
        return nullptr;
    };
    void lastObject(CCObject* s) {
        auto textinput = getChildOfType<CCTextInputNode>(static_cast<CCMenuItemSpriteExtra*>(s)->getParent()->getParent(), 0);
        textinput->setString(gd::string(std::to_string(m_lastGroupIDAdded)));
    }
};
class $modify(LevelEditorLayer) {
    bool init(GJGameLevel * a) {
        LevelEditorLayer::init(a);
        auto r = LastGroupIDWorkaround::create();
        auto winHeight = CCDirector::sharedDirector()->getWinSize().height;
        r->setVisible(false);
        r->setPosition(0, winHeight);
        this->addChild(r);
        return true;
    };
};

class $modify(SetGroupIDLayer) {
    void onAddGroup(CCObject * s) {
        //oval_smile
        auto the = getChildOfType<LastGroupIDWorkaround>(getChildOfType<LevelEditorLayer>(static_cast<CCNode*>(s)->getParent()->getParent()->getParent()->getParent(), 0),0);
        the->m_lastGroupIDAdded = m_fields->m_groupIDValue;
        SetGroupIDLayer::onAddGroup(s);
    };
};
class $modify(SetupObjectTogglePopup) {
    bool init(EffectGameObject * p0, CCArray * p1) {
        SetupObjectTogglePopup::init(p0, p1);
        auto menu = getChildOfType<CCMenu>(getChildOfType<CCLayer>(this, 0), 0);
        auto lastgroupbtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Last ID"), this, menu_selector(LastGroupIDWorkaround::lastObject));
        lastgroupbtn->setPosition(CCPoint{ 0,67 });
        menu->addChild(lastgroupbtn);
        return true;
    };
};
