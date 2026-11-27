#include <Geode/Geode.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

#include <array>

using namespace geode::prelude;

namespace {

constexpr int kContentTag = 74120;
constexpr std::array<const char*, 5> kTabNames = {
    "OVERVIEW", "GAME", "PLAYER", "VISUAL", "SYSTEM"
};

constexpr std::array<const char*, 5> kPageTitles = {
    "CONTROL CENTER",
    "GAMEPLAY",
    "PLAYER",
    "VISUALS",
    "SYSTEM"
};

constexpr std::array<const char*, 5> kPageDescriptions = {
    "Sunrise is ready to become your in-game hub.\nFeature modules will be added here independently.",
    "Gameplay controls will live here.\nNo gameplay modifications are active yet.",
    "Player and movement options will live here.\nThis section is ready for future modules.",
    "Visual settings will live here.\nVisual changes will remain individually configurable.",
    "Performance and utility tools will live here.\nThe menu foundation is active."
};

class SunrisePopup final : public Popup {
protected:
    bool init() {
        if (!Popup::init(420.f, 250.f)) {
            return false;
        }

        this->setTitle("SUNRISE");

        auto size = m_mainLayer->getContentSize();

        auto tabBar = CCLayerColor::create(
            ccc4(8, 18, 34, 225),
            size.width - 36.f,
            38.f
        );
        tabBar->setPosition({18.f, size.height - 96.f});
        m_mainLayer->addChild(tabBar, 1);

        auto tabsMenu = CCMenu::create();
        tabsMenu->setPosition({size.width * 0.5f, size.height - 77.f});
        tabsMenu->setContentSize(size);
        m_mainLayer->addChild(tabsMenu, 2);

        for (size_t i = 0; i < kTabNames.size(); ++i) {
            auto label = CCLabelBMFont::create(kTabNames[i], "goldFont.fnt");
            if (!label) {
                continue;
            }

            label->setScale(0.43f);

            auto button = CCMenuItemSpriteExtra::create(
                label,
                this,
                menu_selector(SunrisePopup::onTab)
            );
            if (!button) {
                continue;
            }

            button->setTag(static_cast<int>(i));
            button->setPosition({
                (static_cast<float>(i) - 2.f) * ((size.width - 54.f) / 5.f),
                0.f
            });
            button->setOpacity(i == 0 ? 255 : 165);
            tabsMenu->addChild(button);
            m_tabs[i] = button;
        }

        this->refreshContent();
        return true;
    }

    void onTab(CCObject* sender) {
        auto node = static_cast<CCNode*>(sender);
        auto selected = node->getTag();
        if (selected < 0 || selected >= static_cast<int>(kTabNames.size())) {
            return;
        }

        m_activeTab = selected;

        for (size_t i = 0; i < m_tabs.size(); ++i) {
            if (m_tabs[i]) {
                m_tabs[i]->setOpacity(static_cast<int>(i) == m_activeTab ? 255 : 165);
            }
        }

        this->refreshContent();
    }

    void refreshContent() {
        m_mainLayer->removeChildByTag(kContentTag, true);

        auto content = CCNode::create();
        content->setTag(kContentTag);
        m_mainLayer->addChild(content, 3);

        auto size = m_mainLayer->getContentSize();
        auto panelWidth = size.width - 36.f;
        constexpr float panelHeight = 112.f;

        auto panel = CCLayerColor::create(
            ccc4(7, 16, 31, 238),
            panelWidth,
            panelHeight
        );
        panel->setPosition({18.f, 18.f});
        content->addChild(panel);

        auto accent = CCLayerColor::create(
            ccc4(255, 177, 55, 255),
            3.f,
            panelHeight
        );
        accent->setPosition({0.f, 0.f});
        panel->addChild(accent);

        auto heading = CCLabelBMFont::create(
            kPageTitles[static_cast<size_t>(m_activeTab)],
            "goldFont.fnt"
        );
        if (heading) {
            heading->setAnchorPoint({0.f, 0.5f});
            heading->setPosition({16.f, 83.f});
            heading->setScale(0.55f);
            heading->setColor(ccc3(255, 202, 105));
            panel->addChild(heading);
        }

        auto description = CCLabelBMFont::create(
            kPageDescriptions[static_cast<size_t>(m_activeTab)],
            "chatFont.fnt"
        );
        if (description) {
            description->setAnchorPoint({0.f, 0.5f});
            description->setPosition({16.f, 51.f});
            description->setScale(0.54f);
            description->setColor(ccc3(221, 230, 243));
            panel->addChild(description);
        }

        auto status = CCLabelBMFont::create(
            m_activeTab == 0 ? "INTERFACE READY" : "MODULES NOT ADDED",
            "chatFont.fnt"
        );
        if (status) {
            status->setAnchorPoint({0.f, 0.5f});
            status->setPosition({16.f, 16.f});
            status->setScale(0.42f);
            status->setColor(ccc3(132, 164, 191));
            panel->addChild(status);
        }

        auto version = CCLabelBMFont::create("ALPHA", "chatFont.fnt");
        if (version) {
            version->setAnchorPoint({1.f, 0.5f});
            version->setPosition({panelWidth - 14.f, 16.f});
            version->setScale(0.42f);
            version->setColor(ccc3(255, 177, 55));
            panel->addChild(version);
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

private:
    int m_activeTab = 0;
    std::array<CCMenuItemSpriteExtra*, 5> m_tabs{};
};

void showSunrisePopup() {
    if (auto popup = SunrisePopup::create()) {
        popup->show();
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
    auto sprite = CCSprite::createWithSpriteFrameName("sunrise-button.png"_spr);
    if (!sprite) {
        log::error("Sunrise button sprite could not be loaded.");
        return;
    }

    // The resource is supplied at UHD quality; scale the visible button
    // to a compact touch target for Geometry Dash's landscape layout.
    sprite->setScale(0.32f);

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
