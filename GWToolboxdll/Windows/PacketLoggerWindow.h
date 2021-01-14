#pragma once

#include <GWCA/Utilities/Hook.h>

#include <ToolboxWindow.h>

class PacketLoggerWindow : public ToolboxWindow {
    PacketLoggerWindow() {};
    ~PacketLoggerWindow() {};
public:
    static PacketLoggerWindow& Instance() {
        static PacketLoggerWindow instance;
        return instance;
    }

    const char* Name() const override { return u8"�鿴��Ϸ������Ϣ"; }
    void Draw(IDirect3DDevice9* pDevice) override;

    void Initialize() override;
    void SaveSettings(CSimpleIni* ini) override;
    void LoadSettings(CSimpleIni* ini) override;
    void Update(float delta) override;
    void Enable();
    void Disable();

    
private:
    uint32_t identifiers[512] = { 0 }; // Presume 512 is big enough for header size...
    GW::HookEntry hook_entry;

    GW::HookEntry DisplayDialogue_Entry;
    GW::HookEntry MessageCore_Entry;
    GW::HookEntry MessageLocal_Entry;
    GW::HookEntry NpcGeneralStats_Entry;
};
