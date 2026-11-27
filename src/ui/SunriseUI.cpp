#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/ui/BasedButton.hpp>
#include <Geode/ui/BasedButtonSprite.hpp>

#include <array>

using namespace geode::prelude;

namespace {

constexpr std::array<const char*, 5> kTabNames = {
    "OVERVIEW", "GAME", "PLAYER", "VISUAL", "SYSTEM"
};

constexpr std::array<const char*, 5> kPageTitles = {
    "SUNRISE CONTROL CENTER",
    "GAMEPLAY",
    "PLAYER",
    "VISUALS",
    "SYSTEM"
};

constexpr std::array<const char*, 5> kPageDescriptions = {
    "Your Geometry Dash tools, collected in one place.",
    "Gameplay modules will appear here when they are added.",
    "Player options and movement tools will appear here.",
    "Visual adjustments will appear here as separate modules.",
    "Performance and quality-of-life tools will appear here."
};

constexpr std::array<float, 5> kTabPositions = {
    -151.f, -76.f, 0.f, 76.f, 151.f
};

constexpr float kPageWidth = 366.f;
constexpr float kPageHeight = 116.f;

class SunrisePopup final : public Popup {
protected:
    bool init() {
        if (!Popup::init(420.f, 250.f)) {
            return false;
        }

        this->setTitle("SUNRISE", "goldFont.fnt", 0.72f, 18.f);

        auto size = m_mainLayer->getContentSize();

        auto subtitle = CCLabelBMFont::create(
            "MODULAR GEOMETRY DASH MENU",
            "chatFont.fnt"
        );
        if (subtitle) {
            subtitle->setPosition({size.width * 0.5f, size.height - 49.f});
            subtitle->setScale(0.48f);
            subtitle->setColor(ccc3(216, 226, 241));
            m_mainLayer->addChild(subtitle, 2);
        }

        auto tabPlate = CCLayerColor::create(
            ccc4(22, 27, 40, 235),
            size.width - 34.f,
            34.f
        );
        tabPlate->setPosition({17.f, size.height - 96.f});
        m_mainLayer->addChild(tabPlate, 1);

        m_tabsMenu = CCMenu::create();
        if (!m_tabsMenu) {
            return false;
        }
        m_tabsMenu->setPosition({size.width * 0.5f, size.height - 79.f});
        m_mainLayer->addChild(m_tabsMenu, 3);

        for (size_t i = 0; i < kTabNames.size(); ++i) {
            auto tab = TabButton::create(
                kTabNames[i],
                this,
                menu_selector(SunrisePopup::onTab)
            );
            if (!tab) {
                log::error("Could not create Sunrise tab {}", kTabNames[i]);
                continue;
            }

            tab->setScale(0.60f);
            tab->setTag(static_cast<int>(i));
            tab->setPosition({kTabPositions[i], 0.f});
            tab->toggle(i == 0);
            m_tabsMenu->addChild(tab);
            m_tabs[i] = tab;
        }

        m_tabIndicator = CCLayerColor::create(
            ccc4(143, 255, 55, 255),
            48.f,
            3.f
        );
        if (m_tabIndicator) {
            m_tabIndicator->setPosition({kTabPositions[0] - 24.f, -17.f});
            m_tabsMenu->addChild(m_tabIndicator, -1);
        }

        m_pageContent = this->createPage(0);
        if (!m_pageContent) {
            return false;
        }
        m_mainLayer->addChild(m_pageContent, 2);

        return true;
    }

    CCLayerColor* createPage(int pageIndex) {
        auto root = CCLayerColor::create(ccc4(0, 0, 0, 0), kPageWidth, kPageHeight);
        if (!root) {
            return nullptr;
        }
        root->setCascadeOpacityEnabled(true);
        root->setContentSize({kPageWidth, kPageHeight});
        root->setPosition({
            (m_mainLayer->getContentSize().width - kPageWidth) * 0.5f,
            17.f
        });

        // Black outer edge and inset dark face recreate the thick,
        // high-contrast plates used throughout Geometry Dash.
        auto frame = CCLayerColor::create(
            ccc4(5, 8, 15, 255),
            kPageWidth,
            kPageHeight
        );
        root->addChild(frame);

        auto face = CCLayerColor::create(
            ccc4(31, 38, 54, 248),
            kPageWidth - 4.f,
            kPageHeight - 4.f
        );
        face->setPosition({2.f, 2.f});
        frame->addChild(face);

        auto topLip = CCLayerColor::create(
            ccc4(143, 255, 55, 255),
            kPageWidth - 4.f,
            3.f
        );
        topLip->setPosition({2.f, kPageHeight - 5.f});
        frame->addChild(topLip, 2);

        auto leftStripe = CCLayerColor::create(
            ccc4(255, 190, 55, 255),
            4.f,
            kPageHeight - 8.f
        );
        leftStripe->setPosition({2.f, 4.f});
        frame->addChild(leftStripe, 2);

        auto heading = CCLabelBMFont::create(
            kPageTitles[static_cast<size_t>(pageIndex)],
            "bigFont.fnt"
        );
        if (heading) {
            heading->setAnchorPoint({0.f, 0.5f});
            heading->setPosition({17.f, 85.f});
            heading->setScale(0.52f);
            heading->setColor(ccc3(255, 211, 83));
            face->addChild(heading);
        }

        auto description = CCLabelBMFont::create(
            kPageDescriptions[static_cast<size_t>(pageIndex)],
            "chatFont.fnt"
        );
        if (description) {
            description->setAnchorPoint({0.f, 0.5f});
            description->setPosition({17.f, 54.f});
            description->setScale(0.60f);
            description->setColor(ccc3(235, 239, 247));
            face->addChild(description);
        }

        const char* stateText = pageIndex == 0
            ? "SUNRISE CORE READY"
            : "NO MODULES IN THIS CATEGORY YET";

        auto state = CCLabelBMFont::create(stateText, "chatFont.fnt");
        if (state) {
            state->setAnchorPoint({0.f, 0.5f});
            state->setPosition({17.f, 17.f});
            state->setScale(0.46f);
            state->setColor(pageIndex == 0
                ? ccc3(151, 255, 90)
                : ccc3(162, 176, 197));
            face->addChild(state);
        }

        auto version = CCLabelBMFont::create("ALPHA", "chatFont.fnt");
        if (version) {
            version->setAnchorPoint({1.f, 0.5f});
            version->setPosition({kPageWidth - 18.f, 17.f});
            version->setScale(0.46f);
            version->setColor(ccc3(255, 201, 84));
            face->addChild(version);
        }

        return root;
    }

    void onTab(CCObject* sender) {
        auto selected = static_cast<CCNode*>(sender)->getTag();
        if (selected < 0 || selected >= static_cast<int>(kTabNames.size())) {
            return;
        }
        if (selected == m_activeTab) {
            return;
        }

        const int direction = selected > m_activeTab ? 1 : -1;
        m_activeTab = selected;

        for (size_t i = 0; i < m_tabs.size(); ++i) {
            if (m_tabs[i]) {
                m_tabs[i]->toggle(static_cast<int>(i) == m_activeTab);
            }
        }

        // Horizon-style restrained selection motion: 240 ms, eased,
        // with a moving marker instead of a hard tab redraw.
        if (m_tabIndicator) {
            m_tabIndicator->stopAllActions();
            auto markerMove = CCMoveTo::create(
                0.24f,
                ccp(kTabPositions[static_cast<size_t>(selected)] - 24.f, -17.f)
            );
            m_tabIndicator->runAction(CCEaseSineOut::create(markerMove));
        }

        auto nextPage = this->createPage(selected);
        if (!nextPage) {
            return;
        }
        nextPage->setOpacity(0);
        nextPage->setPositionX(
            nextPage->getPositionX() + static_cast<float>(direction) * 13.f
        );
        m_mainLayer->addChild(nextPage, 3);

        if (m_pageContent) {
            m_pageContent->stopAllActions();
            auto oldExit = CCSpawn::create(
                CCMoveBy::create(
                    0.14f,
                    ccp(static_cast<float>(direction) * -13.f, 0.f)
                ),
                CCFadeTo::create(0.14f, 0),
                nullptr
            );
            auto removeOld = CCCallFuncN::create(
                this,
                callfuncN_selector(SunrisePopup::removeOldPage)
            );
            m_pageContent->runAction(CCSequence::create(
                CCEaseSineIn::create(oldExit),
                removeOld,
                nullptr
            ));
        }

        auto enter = CCSpawn::create(
            CCMoveTo::create(
                0.24f,
                ccp(
                    (m_mainLayer->getContentSize().width - kPageWidth) * 0.5f,
                    17.f
                )
            ),
            CCFadeTo::create(0.24f, 255),
            nullptr
        );
        nextPage->runAction(CCEaseSineOut::create(enter));
        m_pageContent = nextPage;
    }

    void removeOldPage(CCNode* node) {
        if (node && node != m_pageContent && node->getParent()) {
            node->removeFromParentAndCleanup(true);
        }
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
        this->setScale(0.92f);
        this->show();
        this->runAction(CCEaseSineOut::create(
            CCScaleTo::create(0.24f, 1.f)
        ));
    }

private:
    int m_activeTab = 0;
    CCMenu* m_tabsMenu = nullptr;
    CCLayerColor* m_tabIndicator = nullptr;
    CCLayerColor* m_pageContent = nullptr;
    std::array<TabButton*, 5> m_tabs{};
};

void showSunrisePopup() {
    if (auto popup = SunrisePopup::create()) {
        popup->showAnimated();
    } else {
        log::error("Could not initialize the Sunrise control panel.");
    }
}

void addSunriseFloatingButton(
    CCNode* parent,
    CCObject* target,
    SEL_MenuHandler callback,
    float yPosition
) {
    auto sprite = CircleButtonSprite::createWithSpriteFrameName(
        "sunrise-button.png"_spr,
        0.60f,
        CircleBaseColor::Green,
        CircleBaseSize::Small
    );
    if (!sprite) {
        log::error("Sunrise button sprite could not be loaded.");
        return;
    }
    sprite->setScale(0.86f);

    auto button = CCMenuItemSpriteExtra::create(sprite, target, callback);
    if (!button) {
        log::error("Sunrise floating button could not be created.");
        return;
    }

    auto menu = CCMenu::create();
    if (!menu) {
        return;
    }

    menu->addChild(button);
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    menu->setPosition({winSize.width - 38.f, yPosition});
    parent->addChild(menu, 10000);
}

} // namespace

class $modify(SunriseMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) {
            return false;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        addSunriseFloatingButton(
            this,
            this,
            menu_selector(SunriseMenuLayer::onSunrisePressed),
            winSize.height - 54.f
        );

        return true;
    }

    void onSunrisePressed(CCObject*) {
        showSunrisePopup();
    }
};

class $modify(SunrisePlayLayer, PlayLayer) {
    bool init(GJGameLevel* level, bool dontCreateObjects) {
        if (!PlayLayer::init(level, dontCreateObjects)) {
            return false;
        }

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        addSunriseFloatingButton(
            this,
            this,
            menu_selector(SunrisePlayLayer::onSunrisePressed),
            winSize.height * 0.58f
        );

        return true;
    }

    void onSunrisePressed(CCObject*) {
        showSunrisePopup();
    }
};
