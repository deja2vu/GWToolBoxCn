#pragma once

#include <GWCA/Constants/Constants.h>
#include <GWCA/Constants/Skills.h>

#include <GWCA/GameEntities/Item.h>

#include <GWCA/Managers/MapMgr.h>

#include <Defines.h>

// abstract class Toolbox Hotkey
// has the key code and pressed status
class TBHotkey {
public:
    enum Op {
        Op_None,
        Op_MoveUp,
        Op_MoveDown,
        Op_Delete,
        Op_BlockInput,
    };

    static bool show_active_in_header;
    static bool show_run_in_header;
    static bool hotkeys_changed;

    static TBHotkey* HotkeyFactory(CSimpleIni* ini, const char* section);

    bool pressed = false;   // if the key has been pressed
    bool active = true;     // if the hotkey is enabled/active
    bool show_message_in_emote_channel = true; // if hotkey should show message in emote channel when triggered
    bool show_error_on_failure = true; // if hotkey should show error message on failure
    bool ongoing = false; // used for hotkeys that need to execute more than once per toggle.
    bool block_gw = false; // true to consume the keypress before passing to gw.
    bool trigger_on_explorable = false; // Trigger when entering explorable area
    bool trigger_on_outpost = false; // Trigger when entering outpost area

    int map_id = 0;
    int prof_id = 0;
    int instance_type = 0;

    long hotkey = 0;
    long modifier = 0;

    // Create hotkey, load from file if 'ini' is not null
    TBHotkey(CSimpleIni* ini, const char* section);
    virtual ~TBHotkey(){};

    virtual bool CanUse();


    virtual void Save(CSimpleIni* ini, const char* section) const;

    void Draw(Op* op);

    virtual const char* Name() const = 0;
    virtual void Draw() = 0;
    virtual void Description(char *buf, size_t bufsz) const = 0;
    virtual void Execute() = 0;

protected:
    
    static bool isLoading() { return GW::Map::GetInstanceType() == GW::Constants::InstanceType::Loading; }
    static bool isExplorable() { return GW::Map::GetInstanceType() == GW::Constants::InstanceType::Explorable; }
    static bool isOutpost() { return GW::Map::GetInstanceType() == GW::Constants::InstanceType::Outpost; }

    const unsigned int ui_id = 0;   // an internal id to ensure interface consistency

private:
    static unsigned int cur_ui_id;

};

// hotkey to send a message in chat
// can be used for anything that uses SendChat
class HotkeySendChat : public TBHotkey {
private:
    char message[139];
    char channel = '/';

public:
    inline static const char* IniSection() { return "SendChat"; }
    const char* Name() const override { return IniSection(); }

    HotkeySendChat(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

class HotkeyEquipItem : public TBHotkey {
private:
    UINT bag_idx = 0;
    UINT slot_idx = 0;
    GW::Item* item = nullptr;
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    std::chrono::time_point<std::chrono::steady_clock> last_try;
    //char item_name[64] = {};
    char item_name[193] = { 0, };
    bool showSettingUI = false;
    CRITICAL_SECTION cs;
    struct ItemPrimaryKey
    {
        uint32_t model_id, model_file_id, mod_struct_size, interaction;
    };
    ItemPrimaryKey* key = nullptr;
    uint32_t index = 0;
public:
    static std::vector<uint32_t> nameIndex;
private:
    bool IsSameItem(const GW::Item* const itemV) {
        return key && itemV
            && (!key->model_id || !itemV->model_id || key->model_id == itemV->model_id)
            && (!key->model_file_id || !itemV->model_file_id || key->model_file_id == itemV->model_file_id)
            && (!key->mod_struct_size || !itemV->mod_struct_size || key->mod_struct_size == itemV->mod_struct_size)
            && (!key->interaction || !itemV->interaction || key->interaction == itemV->interaction);
    }
    void UpdateStatus();
    const GW::Item* LookUpItem();
    bool IsEquiped();
public:
    static const char* IniSection() { return "EquipItem"; }
    const char* Name() const override { return IniSection(); }

    HotkeyEquipItem(CSimpleIni* ini, const char* section);
    ~HotkeyEquipItem() {
        //DeleteCriticalSection(&cs);

        delete key;
    };
    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char* buf, size_t bufsz) const override;
    void Execute() override;

    bool IsEquippable(GW::Item* item);
    static void AddIndex(uint32_t& idx);
    static void RemoveIndex(const uint32_t idx);
    static bool HasIndex(const uint32_t idx);
};

// hotkey to use an item
// will use the item in explorable areas, and display a warning with given name if not found
class HotkeyUseItem : public TBHotkey {
private:
    UINT item_id = 0;
    char name[140];
public:
    static const char* IniSection() { return "UseItem"; }
    const char* Name() const override { return IniSection(); }

    HotkeyUseItem(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    bool CanUse() override { return TBHotkey::CanUse() && item_id != 0; }

    void Draw() override;
    void Description(char* buf, size_t bufsz) const override;
    void Execute() override;
};

// hotkey to drop a buff if currently active, and cast it on the current target if not
// can be used for recall, ua, and maybe others?
class HotkeyDropUseBuff : public TBHotkey {
    enum SkillIndex {
        Recall,
        UA,
        HolyVeil,
        Other
    };
    static bool GetText(void* data, int idx, const char** out_text);
    SkillIndex GetIndex() const;

public:
    GW::Constants::SkillID id;

    static const char* IniSection() { return "DropUseBuff"; }
    const char* Name() const override { return IniSection(); }

    HotkeyDropUseBuff(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

// hotkey to toggle a toolbox function
class HotkeyToggle : public TBHotkey {
    const int n_targets = 4;
    enum Toggle {
        Clicker,
        Pcons,
        CoinDrop,
        Tick
    };
    static bool GetText(void*, int idx, const char** out_text);

public:
    Toggle target; // the thing to toggle

    static bool IsValid(CSimpleIni *ini, const char *section);
    static const char* IniSection() { return "Toggle"; }
    const char* Name() const override { return IniSection(); }

    HotkeyToggle(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

class HotkeyAction : public TBHotkey {
    const int n_actions = 5;
    enum Action {
        OpenXunlaiChest,
        OpenLockedChest,
        DropGoldCoin,
        ReapplyTitle,
        EnterChallenge,
    };
    static bool GetText(void*, int idx, const char** out_text);

public:
    Action action;

    static const char* IniSection() { return "Action"; }
    const char* Name() const override { return IniSection(); }

    HotkeyAction(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

// hotkey to target something in-game
// it will target the closest agent with the given PlayerNumber (aka modelID)
class HotkeyTarget : public TBHotkey {
private:
    const uint32_t types[3] = { 0xDB,0x200,0x400 };
    const char *type_labels[3] = { "NPC", "Signpost", "Item" };
    const char *identifier_labels[3] = { "Model ID", "Gadget ID", "Item ModelID" };
    enum HotkeyTargetType : int
    {
        NPC,
        Signpost,
        Item,
        Count
    } type = HotkeyTargetType::NPC;
    uint32_t id = 0;
    char name[140];
public:
    static const char* IniSection() { return "Target"; }
    const char* Name() const override { return IniSection(); }

    HotkeyTarget(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

// hotkey to move to a specific position
// it will only move in explorables, and if in compass range from destination
class HotkeyMove : public TBHotkey {
public:
    float x = 0.0;
    float y = 0.0;
    float range = 0.0;
    char name[140];

    static const char* IniSection() { return "Move"; }
    const char* Name() const override { return IniSection(); }

    HotkeyMove(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

class HotkeyDialog : public TBHotkey {
public:
    size_t id = 0;
    char name[140];

    static const char* IniSection() { return "Dialog"; }
    const char* Name() const override { return IniSection(); }

    HotkeyDialog(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

class HotkeyPingBuild : public TBHotkey {
    static bool GetText(void*, int idx, const char** out_text);

public:
    size_t index = 0;

    static const char* IniSection() { return "PingBuild"; }
    const char* Name() const override { return IniSection(); }

    HotkeyPingBuild(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

class HotkeyHeroTeamBuild : public TBHotkey {
    static bool GetText(void*, int idx, const char** out_text);

public:
    size_t index = 0;

    static const char* IniSection() { return "HeroTeamBuild"; }
    const char* Name() const override { return IniSection(); }

    HotkeyHeroTeamBuild(CSimpleIni* ini, const char* section);

    void Save(CSimpleIni* ini, const char* section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};

class HotkeyFlagHero : public TBHotkey
{
public:
    float degree = 0.0;
    float distance = 0.0;
    int hero = 0;

    static const char *IniSection()
    {
        return "FlagHero";
    }
    const char *Name() const override
    {
        return IniSection();
    }

    HotkeyFlagHero(CSimpleIni *ini, const char *section);

    void Save(CSimpleIni *ini, const char *section) const override;

    void Draw() override;
    void Description(char *buf, size_t bufsz) const override;
    void Execute() override;
};