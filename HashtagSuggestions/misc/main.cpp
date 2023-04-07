#include <Geode/Geode.hpp>
#include <Geode/modify/CCTransitionFade.hpp>
#include <Geode/modify/CCTransitionCrossFade.hpp>
#include <Geode/modify/CCTransitionFadeBL.hpp>
#include <Geode/modify/CCTransitionFadeTR.hpp>
#include <Geode/modify/CCTransitionFadeDown.hpp>
#include <Geode/modify/CCTransitionFadeUp.hpp>
#include <Geode/modify/CCTransitionFlipAngular.hpp>
#include <Geode/modify/CCTransitionFlip.hpp>
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
    static CCTransitionFadeBL* create(float duration, CCScene* scene) {
        return CCTransitionFadeBL::create(transitionSpeed, scene);
    }
};

class $modify(CCTransitionFadeTR) {
    static CCTransitionFadeTR* create(float duration, CCScene* scene) {
        return CCTransitionFadeTR::create(transitionSpeed, scene);
    }
};

class $modify(CCTransitionFadeDown) {
    static CCTransitionFadeDown* create(float duration, CCScene* scene) {
        return CCTransitionFadeDown::create(transitionSpeed, scene);
    }
};

class $modify(CCTransitionFadeUp) {
    static CCTransitionFadeUp* create(float duration, CCScene* scene) {
        return CCTransitionFadeUp::create(transitionSpeed, scene);
    }
};

class $modify(CCTransitionFlipAngular) {
    static CCTransitionFlipAngular* create(float duration, CCScene* scene, tOrientation o) {
        return CCTransitionFlipAngular::create(transitionSpeed, scene, o);
    }
    static CCTransitionFlipAngular* create(float duration, CCScene* scene) {
        return CCTransitionFlipAngular::create(transitionSpeed, scene);
    }
};

class $modify(CCTransitionFlipX) {
    static CCTransitionFlipX* create(float duration, CCScene* scene, tOrientation o) {
        return CCTransitionFlipX::create(transitionSpeed, scene, o);
    }
    static CCTransitionFlipX* create(float duration, CCScene* scene) {
        return CCTransitionFlipX::create(transitionSpeed, scene);
    }
};

$execute {
    listenForSettingChanges("transition-speed", +[](double v) {
        float transitionSpeed = (float) v;
    });
}


//change transition speed

