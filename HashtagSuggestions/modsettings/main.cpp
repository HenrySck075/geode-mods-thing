#include <Geode/Geode.hpp>
#include <Geode/modify/CCKeyboardDispatcher>
#include <Geode/ui/internal/settings/ModSettingsPopup.hpp>
#include <vector>

using namespace geode::prelude;

bool any(std::function func, std::vector<CCNode> iter) {
    bool h = false;
    for (auto& i : iter) {
        if (func(i)) h = true;
    }
    return h;
};
void openModSettings(Mod* mod) {
    CCDirector::sharedDirector()->getRunningScene()->addChild(ModSettingsPopup::create(mod))
};
class $modify(CCKeyboardDispatcher) {
    bool hs_combinationWaiting = false;
    std::vector<CCNode> m_playerrel = {PauseLayer};
    auto containsChildOfType = [&](CCNode child, CCNode* parent, int index) {
        if (getChildOfType<child>(parent, index) != nullptr) return true;
        return false;
    }
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
        if (down && key == CTRL_LEFT) {
            m_fields->hs_combinationWaiting = true;
        };
        if (down && key == SPACE && m_fields->hs_combinationWaiting) {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            if any(m_fields->containsChildOfType, m_fields->m_playerrel) {

            }
            return true;
        };
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
    }
};
