#include <Geode/Geode.hpp>
#include <Geode/modify/CCTransitionFade.hpp> //only this is available fsr

using namespace geode::prelude;
#include <Geode/modify/LevelSearchLayer.hpp>
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


//change transition speed

