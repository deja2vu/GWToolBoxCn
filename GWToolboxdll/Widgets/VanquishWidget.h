#pragma once

#include <ToolboxWidget.h>

class VanquishWidget : public ToolboxWidget {
    VanquishWidget() {};
    ~VanquishWidget() {};

public:
    static VanquishWidget& Instance() {
        static VanquishWidget instance;
        return instance;
    }

    const char* Name() const override { return u8"清图完成量"; }

    const char* Icon() const override { return ICON_FA_SKULL;  }

    void Draw(IDirect3DDevice9* pDevice) override;

    void DrawSettingInternal() override;
};
