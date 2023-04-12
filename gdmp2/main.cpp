#include <Geode/Geode.hpp>

CCDictionary* players = new CCDictionary();
#define add(k, v) players->setObject(v ,k)
#define basecreate(class_, ...)             \
    auto pret = new class_();               \
    if (pret && pret->init(__VA_ARGS__)) {  \
        pret->autorelease();                \
        return pret;                        \
    }                                       \
    CC_SAFE_DELETE(pret);                   \
    return nullptr;                         


class OnlinePlayerObject : public PlayerObject {
protected:
public:
    static OnlinePlayerObject* create(int p0, int p1, CCLayer* p2) {
        basecreate(OnlinePlayerObject, p0, p1, p2)
    }
    void d
    playerDestroyed(bool p) {PlayerObject::playerDestroyed(false);}
};