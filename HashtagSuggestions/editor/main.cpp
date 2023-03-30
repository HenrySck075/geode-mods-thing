#include <Geode/Geode.hpp>
#include <Geode/cocos/base_nodes/CCNode.h>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/loader/Log.hpp>
#include <typeinfo>
//currently only supports int
CCArray*createFromStdArray(std::array h) {
    auto d = CCArray::create();
    for (auto&item : h) {
        std::string typ=typeid(item).name();
        if (typ=="int") {
            auto i = CCInteger::create(item);
        } else if (typ=="array") {
            auto i = createFromStdArray(item);
        }
        d->addObject(i);
    };
    return d;
}
auto theblocks = CCDictionary::create();
theblocks->setObject(CCString::create("1"), createFromStdArray([[1011,0,0,180,1010,0],[1011,0,0,180,1010,0],[507,0,20,0,-1,0],[507, -20, 0, -90,-1,0],[507, 20, 0, 90,-1,0],[507, 0, -20, 180,-1,0]]));

using namespace geode::prelude;
bool inPlaytest = false;

//https://discord.com/channels/822510988409831486/822518226307121202/1086426868892303392
const char*b2s(bool stuff) { 
	return stuff ? "true" : "false"; 
};
class FakeBlockPopup : public geode::Popup<CCArray*> {
protected:
	bool setup(CCArray* blocks) override {
		auto winSize = CCDirector::sharedDirector()->getWinSize();

		// convenience function provided by Popup 
		// for adding/setting a title to the popup
		this->setTitle("Select faking method");
		auto egg = CCMenu::create();
		auto da = CCMenuItemSpriteExtra::create(CCSprite::create("GJ_button_01.png"), this, menu_selector(FakeBlockPopup::something));
		da->setUserObject(blocks);
		egg->addChild(da);
		egg->setPosition(m_title->getPosition());

		auto d = winSize / 2;
		auto sep = CCSprite::create("gravityLine_001.png");
		sep->setPosition(d);
		sep->setScaleY(11.925f); sep->setScaleX(4.f);
		sep->setZOrder(22);


		auto recreateSect = CCMenu::create();
		recreateSect->setPosition(sep->getPosition() - CCSize{189,0});
		recreateSect->setContentSize(CCSize{ 140 ,0});

		auto recreateBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Recreate"), this, menu_selector(FakeBlockPopup::recreate));
		auto recreateBtnDesc = CCLabelBMFont::create("When you use this option, the mod\nwill recreate the object with\ndecoration objects.\nNote that not all objects are able\nto use this option,\nPlease use Scale instead.", "chatFont.fnt");
		recreateBtn->setPosition(recreateSect->getContentSize() / 2 + CCPoint{ 0,50 }); recreateBtn->setUserObject(blocks);
		recreateBtnDesc->setPosition(recreateSect->getContentSize() / 2 - CCPoint{0,20});
		
		recreateSect->addChild(recreateBtn); recreateSect->addChild(recreateBtnDesc);

		auto scaleSect = CCMenu::create();
		scaleSect->setPosition(sep->getPosition() + CCSize{ 60,0 });
		scaleSect->setContentSize(CCSize{ 140 ,0 });

		auto scaleBtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("Scale"), this, menu_selector(FakeBlockPopup::scale));
		auto scaleBtnDesc = CCLabelBMFont::create("When you use this option, the mod\nwill resize the object to negative value.\nThis will make object's hitbox\n(almost) disappear.", "chatFont.fnt");
		scaleBtn->setPosition(scaleSect->getContentSize() / 2 + CCPoint{ 0,+50 }); scaleBtn->setUserObject(blocks);
		scaleBtnDesc->setPosition(scaleSect->getContentSize() / 2 - CCPoint{ 0,20 });

		scaleSect->addChild(scaleBtn); scaleSect->addChild(scaleBtnDesc);

		auto the = CCLabelBMFont::create("No-hitbox objects are ignored because why the heck you wanted to do that", "chatFont.fnt");//
		the->setScale(0.7f); the->setPosition(d.width, 31);
		this->addChild(sep); this->addChild(scaleSect); this->addChild(recreateSect); this->addChild(the); this->addChild(egg);

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
				else { return obj->getDetailColor()->m_colorID; }
			}
			else { return colid; };
		};
		auto e = [&](std::vector<short> gr) {
			std::string gs = "";
			for (int g : gr) {
				gs = gs + std::to_string(g) + ".";
			};
			return gs;
		};
		CCARRAY_FOREACH(blocks, the) {
			//fuck you
			auto block = static_cast<GameObject*>(the);
			//if (theblocks.contains(std::to_string(block->m_objectID))) { continue; };
			auto&rebl = theblocks[std::to_string(block->m_objectID)];
			for (int i = 0; i <= sizeof(rebl); i++) {
				auto d = rebl[i];
				std::string group_id = "1," + d.at(0);
				std::string x = ",2," + std::to_string(block->getPositionX() + d.at(1));
				std::string y = ",3," + std::to_string(block->getPositionY() + d.at(2));
				std::string rot = ",6," + std::to_string(block->getRotation() + d.at(3));
				std::string basecol = ",21," + std::to_string(c(d.at(4), block, 0));
				std::string detailcol = ",22," + std::to_string(c(d.at(5), block, 1));
				std::string groups = ",51," + e(block->getGroupIDs());
				//funny n
				std::string objstr = group_id + x + y + rot + basecol + detailcol + groups + ";";
				auto newblock = static_cast<GameObject*>(editorlayer->createObjectsFromString(objstr,true)->objectAtIndex(0));
				editorui->selectObject(newblock, true);
			}
			editorlayer->removeObject(block, true);
		};
		this->keyBackClicked();
	}
	void scale(CCObject* s){
		auto btn = static_cast<CCMenuItemSpriteExtra*>(s);
		//auto editorlayer = getChildOfType<LevelEditorLayer>(btn->getParent()->getParent()->getParent(), 0);
		//auto editorui = static_cast<EditorUI*>(editorlayer->getChildByID("EditorUI"));
		auto blocks = static_cast<CCArray*>(btn->getUserObject());
		CCObject* the;
		CCARRAY_FOREACH(blocks, the) {
			auto block = static_cast<GameObject*>(the);
			block->updateCustomScale(-(block->getScale()));
			block->setRotation(block->getRotation() + 180);
			//block->updateState();
		};
		this->keyBackClicked();
	}
	void something(CCObject* s) {
		auto blocks = static_cast<CCArray*>(static_cast<CCMenuItemSpriteExtra*>(s)->getUserObject());
		CCObject* canyounot;
		CCARRAY_FOREACH(blocks, canyounot) {
			FLAlertLayer::create("e", gd::string(std::to_string(static_cast<GameObject*>(canyounot)->m_objectID)), "uwheioutfg")->show();
		}
	}
	static FakeBlockPopup* create(CCArray*blocks) {
		auto ret = new FakeBlockPopup();
		auto winSize = CCDirector::sharedDirector()->getWinSize()*0.9f;
		if (ret && ret->init(520.f, winSize.height, blocks)) {
			ret->autorelease();
			return ret;
		}
		CC_SAFE_DELETE(ret);
		return nullptr;
	};
	void fakeBlock(CCObject* s) {
		auto btn = static_cast<CCNode*>(s);
		auto editor = static_cast<EditorUI*>(btn->getParent()->getParent());
		auto objs = editor->getSelectedObjects();
		if (objs->count() == 0) {
			FLAlertLayer::create("the", "select objects first\n(idk how to disable button help)", "ok")->show();
		}
		else {
			auto s = FakeBlockPopup::create(objs);
			s->m_noElasticity = true;
			s->show();
		}
	}
};

class $modify(EditorPauseLayer) {
	bool init(LevelEditorLayer * h) {
		//mf removeChildByID doesnt even work
		EditorPauseLayer::init(h);
		if (h->m_level->m_levelID == -4) {
			auto resumemenu = this->getChildByID("resume-menu");
			auto tempexitbtn = CCMenuItemSpriteExtra::create(ButtonSprite::create("     Exit     "), this, menu_selector(EditorPauseLayer::onExitEditor));
			auto saepos = resumemenu->getChildByID("save-and-exit-button")->getPosition();
			tempexitbtn->setPosition(saepos-CCPoint{0,20.25f });
			resumemenu->getChildByID("resume-button")->setPosition(saepos + CCPoint{ 0,20.25f });
			resumemenu->removeChild(resumemenu->getChildByID("save-and-play-button"));
			resumemenu->removeChild(resumemenu->getChildByID("save-and-exit-button"));
			resumemenu->removeChild(resumemenu->getChildByID("save-button"));
			resumemenu->removeChild(resumemenu->getChildByID("exit-button"));
			tempexitbtn->setID("exit-button");
			resumemenu->addChild(tempexitbtn);
		}
		return true;
	}
};
class $modify(EditorUI) {
	bool init(LevelEditorLayer * h) {
		EditorUI::init(h);
		auto editorbuttons = this->getChildByID("editor-buttons-menu");
		auto the = CCSprite::create("GJ_button_01-uhd.png");
		the->setOpacity(50);
		auto fakeblockbtn = CCMenuItemSpriteExtra::create(the,the, this, menu_selector(FakeBlockPopup::fakeBlock));
		fakeblockbtn->setID("fakeblock-gen-button");
		fakeblockbtn->setPosition(editorbuttons->getChildByID("copy-button")->getPosition() - CCPoint{ 38,0 });
		editorbuttons->addChild(fakeblockbtn);
	};
	void onPlaytest(CCObject*s) {
		auto objlayer = getChildOfType<CCLayer>(this->getParent(), 0);
		auto the = objlayer->getChildByID("touch");
		if (the != nullptr) { objlayer->removeChild(the); };
		auto touchPoints = CCLayer::create();
		touchPoints->setID("touch");
		objlayer->addChild(touchPoints);
		inPlaytest = true;
		EditorUI::onPlaytest(s);
	}
	void onStopPlaytest(CCObject* s) {
		inPlaytest = false;
		EditorUI::onStopPlaytest(s);
	}
};
// i cant guarantee that this is working
class $modify(PlayerObject) {
	int m_circlsiz = Mod::get()->getSettingValue<int64_t>("playtest-circlesize");
	void pushButton(int s) {
		if (inPlaytest and Mod::get()->getSettingValue<bool>("playtest-touchindicate")) {
			auto poi= CCSprite::createWithSpriteFrameName("darkblade_01_color_001.png");
			poi->setColor(m_fields->m_playerColor1);//this is somehow incorrect, my icon is all blue how is there green
			poi->setPosition(this->getPosition());
			poi->setScale(m_fields->m_circlsiz/10.f);
			this->getParent()->getParent()->getChildByID("touch")->addChild(poi);//
		};
		PlayerObject::pushButton(s);
	}
	void releaseButton(int s) {
		if (inPlaytest and Mod::get()->getSettingValue<bool>("playtest-releaseindicate")) {
			auto poi = CCSprite::createWithSpriteFrameName("darkblade_01_color_001.png");
			auto col = m_fields->m_playerColor1;
			poi->setColor(ccc3(255-col.r, 255-col.g, 255-col.b));
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
		templevel->m_levelID=-4;
		e->addChild(LevelEditorLayer::create(templevel));
		CCDirector::sharedDirector()->pushScene(CCTransitionFade::create(0.5f,e));
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
