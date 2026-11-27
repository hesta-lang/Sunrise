#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/cocos/touch_dispatcher/CCTouchDispatcher.h>

#include <algorithm>
#include <array>
#include <cmath>

using namespace geode::prelude;

namespace {

void showSunrisePopup();

constexpr std::array<const char*, 6> kTabNames = {
    "HOME", "GAME", "PLAYER", "VISUAL", "SYSTEM", "LAB"
};

constexpr std::array<const char*, 6> kPageTitles = {
    "SUNRISE CONTROL CENTER",
    "GAMEPLAY",
    "PLAYER",
    "VISUALS",
    "SYSTEM",
    "MOTION LAB"
};

constexpr std::array<const char*, 6> kPageDescriptions = {
    "Your Geometry Dash tools, collected in one place.",
    "Gameplay modules will appear here when they are added.",
    "Player options and movement tools will appear here.",
    "Visual adjustments will appear here as separate modules.",
    "Performance and quality-of-life tools will appear here.",
    "Select a transition, then switch tabs to test it."
};

constexpr std::array<const char*, 4> kTransitionNames = {
    "HORIZON", "SLIDE", "FADE", "ZOOM"
};

constexpr std::array<float, 6> kTabPositions = {
    -160.f, -96.f, -32.f, 32.f, 96.f, 160.f
};

constexpr float kPageWidth = 394.f;
constexpr float kPageHeight = 132.f;
constexpr int kPageTag = 74011;

enum class TransitionMode : int {
    Horizon = 0,
    Slide = 1,
    Fade = 2,
    Zoom = 3
};

class SunrisePopup final : public Popup {
protected:
    bool init() {
        if (!Popup::init(430.f, 270.f)) {
            return false;
        }

        this->setTitle("SUNRISE", "goldFont.fnt", 0.72f, 17.f);

        auto size = m_mainLayer->getContentSize();

        auto subtitle = CCLabelBMFont::create(
            "MODULAR GEOMETRY DASH MENU",
            "chatFont.fnt"
        );
        if (subtitle) {
            subtitle->setPosition({size.width * 0.5f, size.height - 47.f});
            subtitle->setScale(0.44f);
            subtitle->setColor(ccc3(224, 230, 241));
            m_mainLayer->addChild(subtitle, 2);
        }

        m_tabsMenu = CCMenu::create();
        if (!m_tabsMenu) {
            return false;
        }
        m_tabsMenu->setPosition({size.width * 0.5f, size.height - 82.f});
        m_mainLayer->addChild(m_tabsMenu, 3);

        for (size_t i = 0; i < kTabNames.size(); ++i) {
            auto offSprite = ButtonSprite::create(
                kTabNames[i],
                "goldFont.fnt",
                "GJ_button_05.png",
                0.48f
            );
            auto onSprite = ButtonSprite::create(
                kTabNames[i],
                "goldFont.fnt",
                "GJ_button_01.png",
                0.48f
            );
            if (!offSprite || !onSprite) {
                log::error("Could not create Sunrise category button {}", kTabNames[i]);
                continue;
            }

            offSprite->setScale(0.72f);
            onSprite->setScale(0.72f);

            auto tab = CCMenuItemToggler::create(
                offSprite,
                onSprite,
                this,
                menu_selector(SunrisePopup::onTab)
            );
            if (!tab) {
                continue;
            }

            tab->setTag(static_cast<int>(i));
            tab->setPosition({kTabPositions[i], 0.f});
            tab->toggle(i == 0);
            m_tabsMenu->addChild(tab);
            m_tabs[i] = tab;
        }

        m_pageContent = this->createPage(0);
        if (!m_pageContent) {
            return false;
        }
        m_mainLayer->addChild(m_pageContent, 2);

        return true;
    }

    CCLayerColor* createPage(int pageIndex) {
        auto root = CCLayerColor::create(
            ccc4(0, 0, 0, 0),
            kPageWidth,
            kPageHeight
        );
        if (!root) {
            return nullptr;
        }

        root->setTag(kPageTag);
        root->setCascadeOpacityEnabled(true);
        root->setContentSize({kPageWidth, kPageHeight});
        root->setPosition({
            (m_mainLayer->getContentSize().width - kPageWidth) * 0.5f,
            19.f
        });

        auto frame = CCLayerColor::create(
            ccc4(9, 12, 20, 255),
            kPageWidth,
            kPageHeight
        );
        root->addChild(frame);

        auto face = CCLayerColor::create(
            ccc4(38, 46, 63, 255),
            kPageWidth - 4.f,
            kPageHeight - 4.f
        );
        face->setPosition({2.f, 2.f});
        frame->addChild(face);

        auto topAccent = CCLayerColor::create(
            ccc4(255, 195, 58, 255),
            kPageWidth - 4.f,
            3.f
        );
        topAccent->setPosition({2.f, kPageHeight - 5.f});
        frame->addChild(topAccent, 2);

        auto sideAccent = CCLayerColor::create(
            ccc4(143, 255, 55, 255),
            4.f,
            kPageHeight - 8.f
        );
        sideAccent->setPosition({2.f, 4.f});
        frame->addChild(sideAccent, 2);

        auto heading = CCLabelBMFont::create(
            kPageTitles[static_cast<size_t>(pageIndex)],
            "bigFont.fnt"
        );
        if (heading) {
            heading->setAnchorPoint({0.f, 0.5f});
            heading->setPosition({17.f, 105.f});
            heading->setScale(0.47f);
            heading->setColor(ccc3(255, 215, 87));
            face->addChild(heading);
        }

        auto description = CCLabelBMFont::create(
            kPageDescriptions[static_cast<size_t>(pageIndex)],
            "chatFont.fnt"
        );
        if (description) {
            description->setAnchorPoint({0.f, 0.5f});
            description->setPosition({17.f, 81.f});
            description->setScale(0.49f);
            description->setColor(ccc3(238, 241, 248));
            face->addChild(description);
        }

        if (pageIndex == 5) {
            this->addMotionLabControls(face);
        } else {
            const char* stateText = pageIndex == 0
                ? "SUNRISE CORE READY"
                : "NO MODULES IN THIS CATEGORY YET";

            auto state = CCLabelBMFont::create(stateText, "chatFont.fnt");
            if (state) {
                state->setAnchorPoint({0.f, 0.5f});
                state->setPosition({17.f, 21.f});
                state->setScale(0.45f);
                state->setColor(pageIndex == 0
                    ? ccc3(151, 255, 90)
                    : ccc3(177, 187, 203));
                face->addChild(state);
            }

            auto version = CCLabelBMFont::create("ALPHA", "chatFont.fnt");
            if (version) {
                version->setAnchorPoint({1.f, 0.5f});
                version->setPosition({kPageWidth - 18.f, 21.f});
                version->setScale(0.45f);
                version->setColor(ccc3(255, 205, 90));
                face->addChild(version);
            }
        }

        return root;
    }

    void addMotionLabControls(CCLayerColor* face) {
        auto modeMenu = CCMenu::create();
        if (!modeMenu) {
            return;
        }
        modeMenu->setPosition({0.f, 0.f});
        face->addChild(modeMenu, 4);

        constexpr std::array<float, 4> positions = {
            55.f, 148.f, 241.f, 334.f
        };

        for (size_t i = 0; i < kTransitionNames.size(); ++i) {
            auto sprite = ButtonSprite::create(
                kTransitionNames[i],
                "goldFont.fnt",
                "GJ_button_05.png",
                0.42f
            );
            if (!sprite) {
                continue;
            }
            sprite->setScale(0.68f);

            auto button = CCMenuItemSpriteExtra::create(
                sprite,
                this,
                menu_selector(SunrisePopup::onMotionLabMode)
            );
            if (!button) {
                continue;
            }

            button->setTag(static_cast<int>(i));
            button->setPosition({positions[i], 51.f});
            button->setOpacity(
                static_cast<int>(i) == m_transitionMode ? 255 : 170
            );
            modeMenu->addChild(button);
            m_motionButtons[i] = button;
        }

        auto previewTrack = CCLayerColor::create(
            ccc4(17, 24, 36, 255),
            kPageWidth - 34.f,
            26.f
        );
        previewTrack->setPosition({17.f, 9.f});
        face->addChild(previewTrack, 1);

        m_labModeLabel = CCLabelBMFont::create(
            "HORIZON  /  240 MS",
            "chatFont.fnt"
        );
        if (m_labModeLabel) {
            m_labModeLabel->setAnchorPoint({0.f, 0.5f});
            m_labModeLabel->setPosition({10.f, 13.f});
            m_labModeLabel->setScale(0.44f);
            m_labModeLabel->setColor(ccc3(207, 222, 240));
            previewTrack->addChild(m_labModeLabel, 2);
        }

        m_labDemo = CCLayerColor::create(
            ccc4(143, 255, 55, 255),
            22.f,
            17.f
        );
        if (m_labDemo) {
            m_labDemo->setPosition({
                previewTrack->getContentSize().width - 31.f,
                4.5f
            });
            previewTrack->addChild(m_labDemo, 3);
        }
    }

    void onTab(CCObject* sender) {
        auto selected = static_cast<CCNode*>(sender)->getTag();
        if (selected < 0 || selected >= static_cast<int>(kTabNames.size())) {
            return;
        }
        if (selected == m_activeTab) {
            return;
        }

        auto nextPage = this->createPage(selected);
        if (!nextPage) {
            return;
        }

        const int direction = selected > m_activeTab ? 1 : -1;
        m_activeTab = selected;

        for (size_t i = 0; i < m_tabs.size(); ++i) {
            if (m_tabs[i]) {
                m_tabs[i]->toggle(static_cast<int>(i) == m_activeTab);
            }
        }

        // Remove an older outgoing page before starting a new transition.
        if (m_outgoingPage) {
            m_outgoingPage->stopAllActions();
            m_outgoingPage->removeFromParentAndCleanup(true);
            m_outgoingPage = nullptr;
        }

        auto oldPage = m_pageContent;
        m_pageContent = nextPage;
        if (selected != 5) {
            m_labDemo = nullptr;
            m_labModeLabel = nullptr;
            m_motionButtons.fill(nullptr);
        }

        m_mainLayer->addChild(nextPage, 3);
        this->animatePageIn(nextPage, direction);

        if (oldPage) {
            oldPage->stopAllActions();
            m_outgoingPage = oldPage;
            this->animatePageOut(oldPage, direction);
        }
    }

    void animatePageIn(CCLayerColor* page, int direction) {
        if (!page) {
            return;
        }

        page->setOpacity(0);
        page->setScale(1.f);
        auto destination = page->getPosition();
        float distance = 22.f;
        float duration = 0.24f;

        if (m_transitionMode == static_cast<int>(TransitionMode::Slide)) {
            distance = 54.f;
            duration = 0.31f;
        } else if (m_transitionMode == static_cast<int>(TransitionMode::Fade)) {
            distance = 0.f;
            duration = 0.22f;
        } else if (m_transitionMode == static_cast<int>(TransitionMode::Zoom)) {
            distance = 0.f;
            duration = 0.26f;
            page->setScale(0.91f);
        }

        page->setPosition({
            destination.x + static_cast<float>(direction) * distance,
            destination.y
        });

        CCActionInterval* visualAction = nullptr;
        if (m_transitionMode == static_cast<int>(TransitionMode::Zoom)) {
            visualAction = CCSpawn::create(
                CCScaleTo::create(duration, 1.f),
                CCFadeTo::create(duration, 255),
                nullptr
            );
        } else {
            visualAction = CCSpawn::create(
                CCMoveTo::create(duration, destination),
                CCFadeTo::create(duration, 255),
                nullptr
            );
        }

        page->runAction(CCEaseSineOut::create(visualAction));
    }

    void animatePageOut(CCLayerColor* page, int direction) {
        if (!page) {
            return;
        }

        float duration = 0.14f;
        float distance = 14.f;

        if (m_transitionMode == static_cast<int>(TransitionMode::Slide)) {
            duration = 0.20f;
            distance = 34.f;
        } else if (m_transitionMode == static_cast<int>(TransitionMode::Fade)) {
            duration = 0.16f;
            distance = 0.f;
        } else if (m_transitionMode == static_cast<int>(TransitionMode::Zoom)) {
            duration = 0.18f;
            distance = 0.f;
        }

        CCActionInterval* visualAction = nullptr;
        if (m_transitionMode == static_cast<int>(TransitionMode::Zoom)) {
            visualAction = CCSpawn::create(
                CCScaleTo::create(duration, 0.96f),
                CCFadeTo::create(duration, 0),
                nullptr
            );
        } else {
            visualAction = CCSpawn::create(
                CCMoveBy::create(
                    duration,
                    ccp(static_cast<float>(direction) * -distance, 0.f)
                ),
                CCFadeTo::create(duration, 0),
                nullptr
            );
        }

        auto remove = CCCallFuncN::create(
            this,
            callfuncN_selector(SunrisePopup::removeOutgoingPage)
        );
        page->runAction(CCSequence::create(
            CCEaseSineIn::create(visualAction),
            remove,
            nullptr
        ));
    }

    void removeOutgoingPage(CCNode* node) {
        if (!node) {
            return;
        }
        if (node == m_outgoingPage) {
            m_outgoingPage = nullptr;
        }
        if (node != m_pageContent && node->getParent()) {
            node->removeFromParentAndCleanup(true);
        }
    }

    void onMotionLabMode(CCObject* sender) {
        auto selected = static_cast<CCNode*>(sender)->getTag();
        if (selected < 0 || selected >= static_cast<int>(kTransitionNames.size())) {
            return;
        }

        m_transitionMode = selected;
        for (size_t i = 0; i < m_motionButtons.size(); ++i) {
            if (m_motionButtons[i]) {
                m_motionButtons[i]->setOpacity(
                    static_cast<int>(i) == selected ? 255 : 170
                );
            }
        }

        if (m_labModeLabel) {
            auto duration = selected == 1 ? 310 : selected == 2 ? 220 : selected == 3 ? 260 : 240;
            m_labModeLabel->setString(
                fmt::format("{}  /  {} MS", kTransitionNames[selected], duration).c_str()
            );
        }

        this->previewMotionMode();
    }

    void previewMotionMode() {
        if (!m_labDemo) {
            return;
        }

        auto track = m_labDemo->getParent();
        if (!track) {
            return;
        }

        constexpr float width = 22.f;
        const float targetX = track->getContentSize().width - 31.f;
        constexpr float targetY = 4.5f;

        m_labDemo->stopAllActions();
        m_labDemo->setOpacity(0);
        m_labDemo->setScale(1.f);
        m_labDemo->setPosition({targetX, targetY});

        float distance = 18.f;
        float duration = 0.24f;
        if (m_transitionMode == static_cast<int>(TransitionMode::Slide)) {
            distance = 42.f;
            duration = 0.31f;
            m_labDemo->setPosition({targetX - distance, targetY});
        } else if (m_transitionMode == static_cast<int>(TransitionMode::Fade)) {
            distance = 0.f;
            duration = 0.22f;
        } else if (m_transitionMode == static_cast<int>(TransitionMode::Zoom)) {
            distance = 0.f;
            duration = 0.26f;
            m_labDemo->setScale(0.35f);
        } else {
            m_labDemo->setPosition({targetX - distance, targetY});
        }

        CCActionInterval* visualAction = nullptr;
        if (m_transitionMode == static_cast<int>(TransitionMode::Zoom)) {
            visualAction = CCSpawn::create(
                CCScaleTo::create(duration, 1.f),
                CCFadeTo::create(duration, 255),
                nullptr
            );
        } else {
            visualAction = CCSpawn::create(
                CCMoveTo::create(duration, ccp(targetX, targetY)),
                CCFadeTo::create(duration, 255),
                nullptr
            );
        }
        m_labDemo->runAction(CCEaseSineOut::create(visualAction));
    }

public:
    static SunrisePopup* create() {
        auto ret = new SunrisePopup();
        if (ret->init()) {
            ret->autorelease();
            return ret;
        }

        delete ret;
        return nullptr;
    }

    void showAnimated() {
        this->setScale(0.94f);
        this->show();
        this->runAction(CCEaseSineOut::create(
            CCScaleTo::create(0.24f, 1.f)
        ));
    }

private:
    int m_activeTab = 0;
    int m_transitionMode = static_cast<int>(TransitionMode::Horizon);
    CCMenu* m_tabsMenu = nullptr;
    CCLayerColor* m_pageContent = nullptr;
    CCLayerColor* m_outgoingPage = nullptr;
    CCLayerColor* m_labDemo = nullptr;
    CCLabelBMFont* m_labModeLabel = nullptr;
    std::array<CCMenuItemToggler*, 6> m_tabs{};
    std::array<CCMenuItemSpriteExtra*, 4> m_motionButtons{};
};

void showSunrisePopup() {
    if (auto popup = SunrisePopup::create()) {
        popup->showAnimated();
    } else {
        log::error("Could not initialize the Sunrise control panel.");
    }
}

// This follows the same touch-layer pattern used by Music Integrations:
// a full-screen CCLayer, a targeted touch delegate, a resource loaded with
// CCSprite::create(..._spr), and direct insertion into the parent layer.
class SunriseFloatingToggle final : public CCLayer {
protected:
    CCSprite* m_background = nullptr;
    CCPoint m_touchStart = CCPointZero;
    CCPoint m_startPosition = CCPointZero;
    CCPoint m_targetPosition = CCPointZero;
    bool m_dragged = false;
    bool m_touchingButton = false;
    bool m_inGame = false;

public:
    static SunriseFloatingToggle* create(bool inGame) {
        auto ret = new SunriseFloatingToggle();
        if (ret && ret->init(inGame)) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }

    bool init(bool inGame) {
        if (!CCLayer::init()) {
            return false;
        }

        m_inGame = inGame;
        auto size = CCDirector::sharedDirector()->getWinSize();
        this->setContentSize(size);
        this->setTouchEnabled(true);

        // Deliberately use CCSprite::create, matching Music Integrations'
        // resource-loading path instead of relying on a spritesheet frame.
        m_background = CCSprite::create("sunrise-button.png"_spr);
        if (!m_background) {
            log::error("Sunrise: could not load sunrise-button.png from mod resources");
            return false;
        }

        m_background->setScale(0.62f);
        m_background->setOpacity(225);
        m_background->setPosition(
            inGame
                ? ccp(43.f, size.height * 0.58f)
                : ccp(size.width - 43.f, size.height - 48.f)
        );
        this->addChild(m_background, 1);
        m_targetPosition = m_background->getPosition();
        this->scheduleUpdate();
        return true;
    }

    void registerWithTouchDispatcher() override {
        CCTouchDispatcher::get()->addTargetedDelegate(
            this,
            kCCMenuHandlerPriority - 1,
            true
        );
    }

    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override {
        if (!m_background || !touch) {
            return false;
        }

        auto point = this->convertToNodeSpace(touch->getLocation());
        auto bounds = m_background->boundingBox();
        bounds.origin.x -= 12.f;
        bounds.origin.y -= 12.f;
        bounds.size.width += 24.f;
        bounds.size.height += 24.f;
        if (!bounds.containsPoint(point)) {
            return false;
        }

        m_touchingButton = true;
        m_background->setOpacity(255);
        m_dragged = false;
        m_touchStart = point;
        m_startPosition = m_background->getPosition();
        m_targetPosition = m_startPosition;
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent* event) override {
        if (!m_touchingButton || !m_background || !touch) {
            return;
        }

        auto point = this->convertToNodeSpace(touch->getLocation());
        float dx = point.x - m_touchStart.x;
        float dy = point.y - m_touchStart.y;
        if (std::abs(dx) > 8.f || std::abs(dy) > 8.f) {
            m_dragged = true;
        }
        if (!m_dragged) {
            return;
        }

        auto size = this->getContentSize();
        auto bounds = m_background->boundingBox();
        float halfWidth = bounds.size.width * 0.5f;
        float halfHeight = bounds.size.height * 0.5f;
        float x = std::clamp(
            m_startPosition.x + dx,
            halfWidth,
            size.width - halfWidth
        );
        float y = std::clamp(
            m_startPosition.y + dy,
            halfHeight,
            size.height - halfHeight
        );
        m_targetPosition = CCPoint(x, y);
    }

    void update(float dt) override {
        CCLayer::update(dt);
        if (!m_background || !m_dragged) {
            return;
        }

        auto current = m_background->getPosition();
        float blend = 1.f - std::exp(-12.f * std::min(dt, 0.1f));
        m_background->setPosition({
            current.x + (m_targetPosition.x - current.x) * blend,
            current.y + (m_targetPosition.y - current.y) * blend
        });
    }

    void ccTouchEnded(CCTouch* touch, CCEvent* event) override {
        if (!m_touchingButton) {
            return;
        }

        m_touchingButton = false;
        if (m_background) {
            m_background->setOpacity(225);
        }
        if (!m_dragged) {
            showSunrisePopup();
        }
    }

    void ccTouchCancelled(CCTouch* touch, CCEvent* event) override {
        m_touchingButton = false;
        if (m_background) {
            m_background->setOpacity(225);
        }
    }
};

} // namespace

class $modify(SunriseMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto toggle = SunriseFloatingToggle::create(false);
        if (toggle) {
            toggle->setID("sunrise-floating-toggle");
            this->addChild(toggle, 999);
            log::info("Sunrise: floating menu toggle added to MenuLayer");
        } else {
            log::error("Sunrise: failed to create MenuLayer floating toggle");
        }
        return true;
    }
};

class $modify(SunrisePlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool dontCreateObjects) {
        if (!PlayLayer::init(level, dontCreateObjects)) {
            return false;
        }

        auto toggle = SunriseFloatingToggle::create(true);
        if (toggle) {
            toggle->setID("sunrise-floating-toggle");
            this->addChild(toggle, 999);
            log::info("Sunrise: floating menu toggle added to PlayLayer");
        } else {
            log::error("Sunrise: failed to create PlayLayer floating toggle");
        }
        return true;
    }
};
