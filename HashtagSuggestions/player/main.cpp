#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/loader/Log.hpp>

using namespace geode::prelude;


class $modify(PlayLayer) {
	bool init(GJGameLevel*h) {
		PlayLayer::init(h);
		//i have no idea
		auto level = getChildOfType<CCLayer>(this, 0);
		auto idkhowtousesprbatchnode = CCLayer::create();
		idkhowtousesprbatchnode->setID("iamdumb");
		level->addChild(idkhowtousesprbatchnode);
		return true;
	};
    void addObject(GameObject* p0) {
        if (p0->getType() != GameObjectType::Decoration && Mod::get()->getSettingValue<bool>("ultra-ldm")) PlayLayer::addObject(p0);
    };
	void markCheckpoint() {
		if (m_fields->m_isDualMode) {
			auto level = getChildOfType<CCLayer>(this, 0);
			auto asmentionedabove = level->getChildByID("iamdumb");
			auto fakecheckpoint = CCSprite::create("checkpoint_01_001.png");
			fakecheckpoint->setPosition(m_fields->m_player2->getPosition());
			asmentionedabove->addChild(fakecheckpoint);
		}
		geode::log::info(m_fields->m_isDualMode ? "true" : "false");
		PlayLayer::markCheckpoint();
	}
}
