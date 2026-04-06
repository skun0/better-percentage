#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

ccColor3B hsvToRgb(float h) {
    float r = 0.f, g = 0.f, b = 0.f;
    float s = 1.f, v = 1.f;

    int i = int(h * 6.f);
    float f = h * 6.f - i;
    float p = v * (1.f - s);
    float q = v * (1.f - f * s);
    float t = v * (1.f - (1.f - f) * s);

    switch (i % 6) {
    case 0: r = v; g = t; b = p; break;
    case 1: r = q; g = v; b = p; break;
    case 2: r = p; g = v; b = t; break;
    case 3: r = p; g = q; b = v; break;
    case 4: r = t; g = p; b = v; break;
    case 5: r = v; g = p; b = q; break;
    }

    return {
        (GLubyte)(r * 255),
        (GLubyte)(g * 255),
        (GLubyte)(b * 255)
    };
}

class $modify(MyPlayLayer, PlayLayer) {

    float getRelativePercent() {
        float current = this->getCurrentPercent();

        if (this->m_startPos) {
            float startX = this->m_startPos->getPosition().x;
            float totalLength = this->m_levelLength;
            float startPercent = (startX / totalLength) * 100.f;
            float remaining = 100.f - startPercent;
            return (remaining > 0.f) ? ((current - startPercent) / remaining * 100.f) : 0.f;
        }

        return current;
    }

    void updateProgressbar() {
        PlayLayer::updateProgressbar();

        auto mod = Mod::get();
        if (!this->m_percentageLabel) return;

        if (!mod->getSettingValue<bool>("enabled")) {
            this->m_percentageLabel->setScale(0.5f);
            this->m_percentageLabel->setColor(ccc3(255, 255, 255));
            return;
        }

        float percent = getRelativePercent();
        int decimals = (int)mod->getSettingValue<int64_t>("decimals");

        std::string format = "{:." + std::to_string(decimals) + "f}%";
        std::string text = fmt::format(fmt::runtime(format), percent);
        this->m_percentageLabel->setString(text.c_str());

        float scale = (float)mod->getSettingValue<double>("scale");
        this->m_percentageLabel->setScale(scale);

        bool rainbow = mod->getSettingValue<bool>("rainbow");
        bool golden = mod->getSettingValue<bool>("golden");

        int best = this->m_level ? this->m_level->m_normalPercent : 0;

        if (rainbow) {
            float hue = percent / 100.f;
            this->m_percentageLabel->setColor(hsvToRgb(hue));
        }
        else if (golden && percent > best) {
            this->m_percentageLabel->setColor(ccc3(255, 215, 0));
        }
        else {
            this->m_percentageLabel->setColor(
                mod->getSettingValue<ccColor3B>("color")
            );
        }
    }
};