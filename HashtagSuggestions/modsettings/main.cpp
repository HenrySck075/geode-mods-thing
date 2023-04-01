#include <Geode/Geode.hpp>
#include <Geode/modify/CCKeyboardDispatcher>

class $modify(CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down) {
        if (down && key == KEY_F11) {
            auto scene = CCDirector::sharedDirector()->getRunningScene();
            return true;
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down);
    }
};
