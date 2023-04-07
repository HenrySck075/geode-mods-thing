#include <Geode/Geode.hpp>
#include <Geode/modify/CCTransitionFade.hpp>
#include <Geode/modify/CCTransitionCrossFade.hpp>
#include <Geode/modify/CCTransitionFadeBL.hpp>
#include <Geode/modify/CCTransitionFadeTR.hpp>
#include <Geode/modify/CCTransitionScene.hpp>

float transitionSpeed = (float) Mod::get()->getSettingValue<double>("transition-speed");
class $modify(CCTransitionCrossFade) {
    static CCTransitionCrossFade* create(float duration, CCScene* scene) {
        return CCTransitionCrossFade::create(transitionSpeed,scene);
    }
};

class $modify(CCTransitionFade) {
    static CCTransitionFade* create(float duration, CCScene* scene) {
        return CCTransitionFade::create(transitionSpeed, scene);
    }
    static CCTransitionFade* create(float duration, CCScene* scene, ccColor3B const& color) {
        return CCTransitionFade::create(transitionSpeed, scene, color);
    }
};

class $modify(CCTransitionFadeBL) {
    static CCTransitionFadeBL* create(float duration, CCScene* ) {
        return CCTransitionFadeBL::create(transitionSpeed, scene);
    }
}
$execute {
    listenForSettingChanges("transition-speed", +[](double v) {
        float transitionSpeed = (float) v;
    });
}


//change transition speed

