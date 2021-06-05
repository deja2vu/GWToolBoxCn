#include "stdafx.h"
#include "Widgets/Minimap/Minimap.h"


#include <GWCA/Constants/Constants.h>
#include <GWCA/GameContainers/Array.h>
#include <GWCA/GameContainers/GamePos.h>

#include <GWCA/Context/CharContext.h>
#include <GWCA/Context/GameContext.h>
#include <GWCA/Context/WorldContext.h>

#include <GWCA/GameEntities/Agent.h>
#include <GWCA/GameEntities/Skill.h>
#include <GWCA/GameEntities/Hero.h>

#include <GWCA/Packets/Opcodes.h>

#include <GWCA/Managers/CtoSMgr.h>
#include <GWCA/Managers/GameThreadMgr.h>
#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/PartyMgr.h>
#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/EffectMgr.h>
#include <GWCA/Managers/ItemMgr.h>
#include <GWCA/Managers/PlayerMgr.h>
#include <GWCA/Managers/SkillbarMgr.h>
#include <GWCA/Managers/MemoryMgr.h>

#include <ImGuiAddons.h>
#include <Keys.h>
#include <Logger.h>

#include <Modules/ChatCommands.h>
#include <Windows/BuildsWindow.h>
#include <Windows/HeroBuildsWindow.h>
#include <Windows/Hotkeys.h>
#include <Windows/HotkeysWindow.h>
#include <Windows/PconsWindow.h>

#include <string>


bool TBHotkey::show_active_in_header = true;
bool TBHotkey::show_run_in_header = true;
bool TBHotkey::hotkeys_changed = false;
unsigned int TBHotkey::cur_ui_id = 0;
std::vector<uint32_t> HotkeyEquipItem::nameIndex = {};
TBHotkey *TBHotkey::HotkeyFactory(CSimpleIni *ini, const char *section)
{
    std::string str(section);
    if (str.compare(0, 7, "hotkey-") != 0)
        return nullptr;
    size_t first_sep = 6;
    size_t second_sep = str.find(L':', first_sep);
    std::string id = str.substr(first_sep + 1, second_sep - first_sep - 1);
    std::string type = str.substr(second_sep + 1);

    if (type.compare(HotkeySendChat::IniSection()) == 0) {
        return new HotkeySendChat(ini, section);
    } else if (type.compare(HotkeyUseItem::IniSection()) == 0) {
        return new HotkeyUseItem(ini, section);
    } else if (type.compare(HotkeyDropUseBuff::IniSection()) == 0) {
        return new HotkeyDropUseBuff(ini, section);
    } else if (type.compare(HotkeyToggle::IniSection()) == 0 &&
               HotkeyToggle::IsValid(ini, section)) {
        return new HotkeyToggle(ini, section);
    } else if (type.compare(HotkeyAction::IniSection()) == 0) {
        return new HotkeyAction(ini, section);
    } else if (type.compare(HotkeyTarget::IniSection()) == 0) {
        return new HotkeyTarget(ini, section);
    } else if (type.compare(HotkeyMove::IniSection()) == 0) {
        return new HotkeyMove(ini, section);
    } else if (type.compare(HotkeyDialog::IniSection()) == 0) {
        return new HotkeyDialog(ini, section);
    } else if (type.compare(HotkeyPingBuild::IniSection()) == 0) {
        return new HotkeyPingBuild(ini, section);
    } else if (type.compare(HotkeyHeroTeamBuild::IniSection()) == 0) {
        return new HotkeyHeroTeamBuild(ini, section);
    } else if (type.compare(HotkeyEquipItem::IniSection()) == 0) {
        return new HotkeyEquipItem(ini, section);
    } else if (type.compare(HotkeyFlagHero::IniSection()) == 0) {
        return new HotkeyFlagHero(ini, section);
    } else {
        return nullptr;
    }
}

TBHotkey::TBHotkey(CSimpleIni *ini, const char *section)
    : ui_id(++cur_ui_id)
{
    if (ini) {
        hotkey = ini->GetLongValue(section, VAR_NAME(hotkey), hotkey);
        modifier = ini->GetLongValue(section, VAR_NAME(modifier), modifier);
        active = ini->GetBoolValue(section, VAR_NAME(active), active);
        map_id = ini->GetLongValue(section, VAR_NAME(map_id), map_id);
        prof_id = ini->GetLongValue(section, VAR_NAME(prof_id), prof_id);
        show_message_in_emote_channel =
            ini->GetBoolValue(section, VAR_NAME(show_message_in_emote_channel),
                              show_message_in_emote_channel);
        show_error_on_failure = ini->GetBoolValue(
            section, VAR_NAME(show_error_on_failure), show_error_on_failure);
        block_gw = ini->GetBoolValue(section, VAR_NAME(block_gw), block_gw);
        trigger_on_explorable = ini->GetBoolValue(
            section, VAR_NAME(trigger_on_explorable), trigger_on_explorable);
        trigger_on_outpost = ini->GetBoolValue(
            section, VAR_NAME(trigger_on_outpost), trigger_on_outpost);
    }
}
bool TBHotkey::CanUse()
{
    return !isLoading() && !GW::Map::GetIsObserving() && GW::MemoryMgr::GetGWWindowHandle() == GetActiveWindow();
}
void TBHotkey::Save(CSimpleIni *ini, const char *section) const
{
    ini->SetLongValue(section, VAR_NAME(hotkey), hotkey);
    ini->SetLongValue(section, VAR_NAME(map_id), map_id);
    ini->SetLongValue(section, VAR_NAME(prof_id), prof_id);
    ini->SetLongValue(section, VAR_NAME(modifier), modifier);
    ini->SetBoolValue(section, VAR_NAME(active), active);
    ini->SetBoolValue(section, VAR_NAME(block_gw), block_gw);
    ini->SetBoolValue(section, VAR_NAME(show_message_in_emote_channel),
                      show_message_in_emote_channel);
    ini->SetBoolValue(section, VAR_NAME(show_error_on_failure),
                      show_error_on_failure);
    ini->SetBoolValue(section, VAR_NAME(trigger_on_explorable),
                      trigger_on_explorable);
    ini->SetBoolValue(section, VAR_NAME(trigger_on_outpost),
                      trigger_on_outpost);
}
static const char *professions[] = {u8"所有",          u8"战士",     u8"游侠",
                                    u8"僧侣",         u8"死灵", u8"幻术",
                                    u8"元素", u8"暗杀",    u8"祭祀",
                                    u8"圣言",      u8"神唤"};
static const char *instance_types[] = {"所有", "城镇", "探索"};
void TBHotkey::Draw(Op *op)
{
    auto ShowHeaderButtons = [&]() {
        if (show_active_in_header || show_run_in_header) {
            ImGui::PushID(static_cast<int>(ui_id));
            ImGui::PushID("header");
            ImGuiStyle &style = ImGui::GetStyle();
            const float btn_width = 50.0f * ImGui::GetIO().FontGlobalScale;
            if (show_active_in_header) {
                ImGui::SameLine(
                    ImGui::GetContentRegionAvail().x -
                    ImGui::GetTextLineHeight() - style.FramePadding.y * 2 -
                    (show_run_in_header
                         ? (btn_width + ImGui::GetStyle().ItemSpacing.x)
                         : 0));
                if (ImGui::Checkbox("", &active))
                    hotkeys_changed = true;
                if (ImGui::IsItemHovered())
                    ImGui::SetTooltip(
                        u8"当你选择后，此快捷键会被启用");
            }
            if (show_run_in_header) {
                ImGui::SameLine(ImGui::GetContentRegionAvail().x -
                                btn_width);
                if (ImGui::Button(u8"运行", ImVec2(btn_width, 0.0f))) {
                    Execute();
                }
            }
            ImGui::PopID();
            ImGui::PopID();
        }
    };

    // === Header ===
    char header[256];
    char desbuf[128];
    char keybuf[128];
    char profbuf[64] = {'\0'};
    char mapbuf[64] = {'\0'};
    if (prof_id)
        snprintf(profbuf, 64, " [%s]", professions[prof_id]);
    if (map_id) {
        if (map_id < sizeof(GW::Constants::NAME_FROM_ID) / sizeof(*GW::Constants::NAME_FROM_ID))
            snprintf(mapbuf, 64, " [%s]", GW::Constants::NAME_FROM_ID[map_id]);
        else
            snprintf(mapbuf, 64, " [Map %d]", map_id);
    }
    Description(desbuf, 128);
    ModKeyName(keybuf, 128, modifier, hotkey, u8"<无热键>");
    snprintf(header, 128, "%s [%s]%s%s###header%u", desbuf, keybuf, profbuf,
             mapbuf, ui_id);
    ImGuiTreeNodeFlags flags = (show_active_in_header || show_run_in_header)
                                   ? ImGuiTreeNodeFlags_AllowItemOverlap
                                   : 0;
    if (!ImGui::CollapsingHeader(header, flags)) {
        ShowHeaderButtons();
    } else {
        ShowHeaderButtons();
        ImGui::PushID(static_cast<int>(ui_id));
        ImGui::PushItemWidth(-70.0f);
        // === Specific section ===
        Draw();

        // === Hotkey section ===
        if (ImGui::Checkbox(u8"当快捷键执行时，屏蔽游戏内的键盘消息",
                            &block_gw))
            hotkeys_changed = true;
        ImGui::ShowHelp(
            u8"如果启用此项，当你执行此快捷键的时候，你的其他按键对于游戏无效");
        if (ImGui::Checkbox(u8"当你切换入探索区域的时候,执行此快捷键",
                            &trigger_on_explorable))
            hotkeys_changed = true;
        if (ImGui::Checkbox(u8"当你切换入城镇的时候，执行此快捷键",
                            &trigger_on_outpost))
            hotkeys_changed = true;
        if (ImGui::InputInt(u8"地图 ID", &map_id))
            hotkeys_changed = true;
        ImGui::ShowHelp(
            u8"此快捷键对于某个地图才有效果 (地图ID = 0, 代表所有地图)");

        if (ImGui::Combo(u8"职业", &prof_id, professions, 11))
            hotkeys_changed = true;
        ImGui::ShowHelp(
            u8"此快捷键只有在所设职业的时候，才有效果 (职业 0 = 表示所有职业)");
        ImGui::Separator();
        if (ImGui::Checkbox("###active", &active))
            hotkeys_changed = true;
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("The hotkey can trigger only when selected");
        ImGui::SameLine();
        static LONG newkey = 0;
        char keybuf2[128];
        snprintf(keybuf2, 128, u8"点此修改: %s", keybuf);
        if (ImGui::Button(keybuf2, ImVec2(-70.0f, 0))) {
            ImGui::OpenPopup(u8"选择快捷键");
            newkey = 0;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(u8"点此修改快捷键");
        if (ImGui::BeginPopup(u8"选择快捷键")) {
            *op = Op_BlockInput;
            ImGui::Text(u8"请按键");
            int newmod = 0;
            if (ImGui::GetIO().KeyCtrl)
                newmod |= ModKey_Control;
            if (ImGui::GetIO().KeyAlt)
                newmod |= ModKey_Alt;
            if (ImGui::GetIO().KeyShift)
                newmod |= ModKey_Shift;

            if (newkey == 0) { // we are looking for the key
                for (int i = 0; i < 512; ++i) {
                    if (i == VK_CONTROL)
                        continue;
                    if (i == VK_SHIFT)
                        continue;
                    if (i == VK_MENU)
                        continue;
                    if (ImGui::GetIO().KeysDown[i]) {
                        newkey = i;
                    }
                }
            } else { // key was pressed, close if it's released
                if (!ImGui::GetIO().KeysDown[newkey]) {
                    hotkey = newkey;
                    modifier = newmod;
                    ImGui::CloseCurrentPopup();
                    hotkeys_changed = true;
                }
            }

            // write the key
            char newkey_buf[256];
            ModKeyName(newkey_buf, 256, newmod, newkey);
            ImGui::Text("%s", newkey_buf);
            if (ImGui::Button(u8"取消", ImVec2(-1.0f, 0)))
                ImGui::CloseCurrentPopup();
            ImGui::EndPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button(u8"运行", ImVec2(70.0f, 0.0f))) {
            Execute();
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(u8"现在执行此快捷键");

        // === Move and delete buttons ===
        if (ImGui::Button(u8"上移",
                          ImVec2(ImGui::GetWindowContentRegionWidth() / 3.0f,
                                 0))) {
            *op = Op_MoveUp;
            hotkeys_changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(u8"把此快捷键移动到最上方");
        ImGui::SameLine();
        if (ImGui::Button(u8"下移",
                          ImVec2(ImGui::GetWindowContentRegionWidth() / 3.0f,
                                 0))) {
            *op = Op_MoveDown;
            hotkeys_changed = true;
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(u8"把此快捷键移动到最下方");
        ImGui::SameLine();
        if (ImGui::Button(u8"删除",
                          ImVec2(ImGui::GetWindowContentRegionWidth() / 3.0f,
                                 0))) {
            ImGui::OpenPopup(u8"确认删除?");
        }
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip(u8"删除此快捷键");
        if (ImGui::BeginPopupModal(u8"确认删除?", nullptr,
                                   ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text(u8"确认吗?\n删除后，无法恢复！\n\n",
                        Name());
            if (ImGui::Button(u8"确定", ImVec2(120, 0))) {
                *op = Op_Delete;
                hotkeys_changed = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"取消", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopItemWidth();
        ImGui::PopID();
    }
}

HotkeySendChat::HotkeySendChat(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    strcpy_s(message, ini->GetValue(section, "msg", ""));
    channel = ini->GetValue(section, "channel", "/")[0];
}
void HotkeySendChat::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetValue(section, "msg", message);
    char buf[8];
    snprintf(buf, 8, "%c", channel);
    ini->SetValue(section, "channel", buf);
}
void HotkeySendChat::Description(char *buf, size_t bufsz) const
{
    snprintf(buf, bufsz, "Send chat '%c%s'", channel, message);
}
void HotkeySendChat::Draw()
{
    int index = 0;
    switch (channel) {
        case '/':
            index = 0;
            break;
        case '!':
            index = 1;
            break;
        case '@':
            index = 2;
            break;
        case '#':
            index = 3;
            break;
        case '$':
            index = 4;
            break;
        case '%':
            index = 5;
            break;
        case '"':
            index = 6;
            break;
    }
    static const char *channels[] = {"/ Commands", "! All",   "@ Guild",
                                     "# Group",    "$ Trade", "% Alliance",
                                     "\" Whisper"};
    if (ImGui::Combo("Channel", &index, channels, 7)) {
        switch (index) {
            case 0:
                channel = '/';
                break;
            case 1:
                channel = '!';
                break;
            case 2:
                channel = '@';
                break;
            case 3:
                channel = '#';
                break;
            case 4:
                channel = '$';
                break;
            case 5:
                channel = '%';
                break;
            case 6:
                channel = '"';
                break;
            default:
                channel = '/';
                break;
        }
        show_message_in_emote_channel = channel == '/' &&
                                        show_message_in_emote_channel;
        hotkeys_changed = true;
    }
    if (ImGui::InputText(u8"消息", message, 139))
        hotkeys_changed = true;
    if (channel == '/' && ImGui::Checkbox(u8"触发的时候，消息提示我",
                                          &show_message_in_emote_channel))
        hotkeys_changed = true;
}
void HotkeySendChat::Execute()
{
    if (!CanUse())
        return;
    if (show_message_in_emote_channel && channel == L'/') {
        Log::Info("/%s", message);
    }
    GW::Chat::SendChat(channel, message);
}

HotkeyUseItem::HotkeyUseItem(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    item_id = static_cast<size_t>(ini->GetLongValue(section, "ItemID", 0));
    strcpy_s(name, ini->GetValue(section, "ItemName", ""));
}
void HotkeyUseItem::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetLongValue(section, "ItemID", static_cast<long>(item_id));
    ini->SetValue(section, "ItemName", name);
}
void HotkeyUseItem::Description(char *buf, size_t bufsz) const
{
    if (name[0] == '\0') {
        snprintf(buf, bufsz, "Use #%d", item_id);
    } else {
        snprintf(buf, bufsz, "Use %s", name);
    }
}
void HotkeyUseItem::Draw()
{
    if (ImGui::InputInt(u8"物品 ID", (int *)&item_id))
        hotkeys_changed = true;
    if (ImGui::InputText(u8"物品名称", name, 140))
        hotkeys_changed = true;
    if (ImGui::Checkbox(u8"失败的时候，消息提示我",
                        &show_error_on_failure))
        hotkeys_changed = true;
}
void HotkeyUseItem::Execute()
{
    if (!CanUse())
        return;
    if (item_id == 0)
        return;

    bool used = GW::Items::UseItemByModelId(item_id, 1, 4);
    if (!used &&
        GW::Map::GetInstanceType() == GW::Constants::InstanceType::Outpost) {
        used = GW::Items::UseItemByModelId(item_id, 8, 16);
    }

    if (!used && show_error_on_failure) {
        if (name[0] == '\0') {
            Log::Error(u8"物品 #%d 没有找到!", item_id);
        } else {
            Log::Error(u8"%s 没有找到!", name);
        }
    }
}


void HotkeyEquipItem::UpdateStatus()
{
    if (bag_idx < 1 || bag_idx > 5 || slot_idx < 1 || slot_idx > 25) {
        if (show_error_on_failure)
            Log::Error("Invalid bag slot %d/%d!", bag_idx, slot_idx);
        return;
    }
    GW::Bag* b = GW::Items::GetBag(bag_idx);
    if (!b) {
        if (show_error_on_failure)
            Log::Error("Bag #%d not found!", bag_idx);
        return;
    }
    GW::ItemArray items = b->items;
    if (!items.valid() || slot_idx > items.size()) {
        if (show_error_on_failure)
            Log::Error("Invalid bag slot %d/%d!", bag_idx, slot_idx);
        return;
    }
    GW::Item* itemV = items.at(slot_idx - 1);
    if (!IsEquippable(itemV)) {
        if (show_error_on_failure)
            Log::Error("No equippable item in bag %d slot %d", bag_idx,
                slot_idx);
        //item = nullptr;
        return;
    }
    EnterCriticalSection(&cs);
    key->model_id = itemV->model_id;
    key->model_file_id = itemV->model_file_id;
    key->mod_struct_size = itemV->mod_struct_size;
    key->interaction = itemV->interaction;
    LeaveCriticalSection(&cs);
}
const GW::Item* HotkeyEquipItem::LookUpItem()
{
    GW::Bag** bags = GW::Items::GetBagArray();
    GW::Bag* bag = NULL;

    for (int bagIndex = 1; bagIndex <= 5; ++bagIndex) {
        bag = bags[bagIndex];
        if (bag != NULL) {
            GW::ItemArray items = bag->items;
            for (size_t i = 0; i < items.size(); i++) {
                if (items[i]) {
                    if ((items[i]->model_id == key->model_id)
                        && (items[i]->model_file_id == key->model_file_id)
                        && (items[i]->mod_struct_size == key->mod_struct_size)
                        && (items[i]->interaction == key->interaction)
                        ) {

                        return items[i];
                    }
                }
            }
        }
    }

    return NULL;

}
bool HotkeyEquipItem::IsEquiped()
{
    GW::Bag* Mybag = GW::Items::GetBag((uint32_t)GW::Constants::Bag::Equipped_Items);
    if (!Mybag) return true;
    GW::ItemArray items = Mybag->items;
    for (uint32_t i = 0; i < items.size(); i++) {
        if (items[i]) {
            if ((items[i]->model_id == key->model_id)
                && (items[i]->model_file_id == key->model_file_id)
                && (items[i]->mod_struct_size == key->mod_struct_size)
                && (items[i]->interaction == key->interaction)
                ) {
                return true;
            }
        }
    }
    return false;
}
HotkeyEquipItem::HotkeyEquipItem(CSimpleIni* ini, const char* section)
    : TBHotkey(ini, section)
{
    // @Cleanup: Add error handling
    key = new ItemPrimaryKey();
    InitializeCriticalSection(&cs);
    //bag_idx = static_cast<size_t>(ini->GetLongValue(section, "Bag", 0));
    //slot_idx = static_cast<size_t>(ini->GetLongValue(section, "Slot", 0));
    index = static_cast<size_t>(ini->GetLongValue(section, "index", 0));
    this->AddIndex(index);
    const char* name = ini->GetValue(section, "item_name", std::to_string(index).c_str());
    memcpy(item_name, name, strlen(name) + 1);
    key->model_id = static_cast<size_t>(ini->GetLongValue(section, "model_id", 0));
    key->model_file_id = static_cast<size_t>(ini->GetLongValue(section, "model_file_id", 0));
    key->mod_struct_size = static_cast<size_t>(ini->GetLongValue(section, "mod_struct_size", 0));
    key->interaction = static_cast<size_t>(ini->GetLongValue(section, "interaction", 0));
}
void HotkeyEquipItem::Save(CSimpleIni* ini, const char* section) const
{
    TBHotkey::Save(ini, section);
    //ini->SetLongValue(section, "Bag", static_cast<long>(bag_idx));
    //ini->SetLongValue(section, "Slot", static_cast<long>(slot_idx));
    ini->SetLongValue(section, "index", static_cast<long>(index));
    ini->SetLongValue(section, "model_id", static_cast<long>(key->model_id));
    ini->SetLongValue(section, "model_file_id", static_cast<long>(key->model_file_id));
    ini->SetLongValue(section, "mod_struct_size", static_cast<long>(key->mod_struct_size));
    ini->SetLongValue(section, "interaction", static_cast<long>(key->interaction));
    ini->SetValue(section, "item_name", (char*)item_name);
}
void HotkeyEquipItem::Description(char* buf, size_t bufsz) const
{
    snprintf(buf, bufsz, u8"装备 %s ", item_name);
}
void HotkeyEquipItem::Draw()
{
    if (ImGui::Button(u8"请输入装备在包里的位置", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
        showSettingUI = true;
    }
    ImGui::SameLine();
    ImGui::ShowHelp(u8"获取装备状态");
    if (showSettingUI) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin(u8"设置装备状态", &showSettingUI, ImGuiWindowFlags_AlwaysAutoResize);
        if (ImGui::InputInt(u8"包 (1-5)", (int*)&bag_idx))
            hotkeys_changed = true;
        if (ImGui::InputInt(u8"槽位 (1-25)", (int*)&slot_idx))
            hotkeys_changed = true;
        if (ImGui::InputText(u8"装备名称", item_name, 193))
            hotkeys_changed = true;
        if (ImGui::Button(u8"保存装备状态", ImVec2(400, 0))) {
            UpdateStatus();
            showSettingUI = false;
        }
        ImGui::End();
    }
    if (ImGui::Checkbox(u8"当发生错误的时候,提示我",
        &show_error_on_failure))
        hotkeys_changed = true;
}
bool HotkeyEquipItem::IsEquippable(GW::Item* _item)
{
    if (!_item)
        return false;
    switch (static_cast<GW::Constants::ItemType>(_item->type)) {
    case GW::Constants::ItemType::Axe:
    case GW::Constants::ItemType::Boots:
    case GW::Constants::ItemType::Bow:
    case GW::Constants::ItemType::Chestpiece:
    case GW::Constants::ItemType::Offhand:
    case GW::Constants::ItemType::Gloves:
    case GW::Constants::ItemType::Hammer:
    case GW::Constants::ItemType::Headpiece:
    case GW::Constants::ItemType::Leggings:
    case GW::Constants::ItemType::Wand:
    case GW::Constants::ItemType::Shield:
    case GW::Constants::ItemType::Staff:
    case GW::Constants::ItemType::Sword:
    case GW::Constants::ItemType::Daggers:
    case GW::Constants::ItemType::Scythe:
    case GW::Constants::ItemType::Spear:
    case GW::Constants::ItemType::Costume:
        break;
    default:
        return false;
        break;
    }
    return true;
    // 2021-05-02: Disabled customised check, conflicts with obfuscator module, not worth the hassle - the hotkey will fail with a message on timeout anyway - Jon
    /*
    if (!_item->customized)
    return true;
    GW::GameContext *g = GW::GameContext::instance();
    GW::CharContext *c = g ? g->character : nullptr;
    return c && c->player_name &&
    wcscmp(c->player_name, _item->customized) == 0;*/
}
void HotkeyEquipItem::RemoveIndex(const uint32_t idx)
{
    if (nameIndex.size() == 0) return;
    std::vector<uint32_t>::iterator it = std::find(nameIndex.begin(), nameIndex.end(), idx);
    if (it != nameIndex.end()) {
        nameIndex.erase(it);
    }
}
bool HotkeyEquipItem::HasIndex(const uint32_t idx)
{
    std::vector<uint32_t>::iterator it = std::find(nameIndex.begin(), nameIndex.end(), idx);
    return it != nameIndex.end();
}
void HotkeyEquipItem::AddIndex(uint32_t& idx)
{
    if (idx != 0) {
        if (HasIndex(idx)) return;
        nameIndex.push_back(idx);
        return;
    }
    if (nameIndex.size() == 0) {
        nameIndex.push_back(1);
        idx = 1;
        return;
    }
    if (nameIndex.size() == 1) {
        if (nameIndex.at(0) <= 1)
        {
            idx = nameIndex.at(0) + 1;
        }
        else
            idx = nameIndex.at(0) - 1;
        nameIndex.push_back(idx);
        return;
    }
    std::sort(nameIndex.begin(), nameIndex.end());
    idx = nameIndex.back() + 1;
    for (std::vector<uint32_t>::iterator it = nameIndex.begin(); it != nameIndex.end(); ++it)
    {
        if (((it + 1) != nameIndex.end()) && ((*(it + 1) - *it) > 1)) {
            idx = *it + 1;
            break;
        }
    }
    nameIndex.push_back(idx);

}
void HotkeyEquipItem::Execute()
{
    if (!CanUse())
        return;
    if (!ongoing) {
        EnterCriticalSection(&cs);
        item = (GW::Item*)LookUpItem();
        uint32_t status = 0;
        if (!item) {
            if (IsEquiped()) {
                status = 1;
            }
            else
                status = 2;
        }
        LeaveCriticalSection(&cs);
        switch (status)
        {
        case 1: {
            Log::Error("has equpied");
            return;
        };
        case 2: {
            Log::Error("can't find this item");
            return;
        };
        default:
            break;
        }
        ongoing = true;
        start_time = std::chrono::steady_clock::now();
    }
    else {
        last_try = std::chrono::steady_clock::now();
        __int64 diff_mills =
            std::chrono::duration_cast<std::chrono::milliseconds>(last_try -
                start_time)
            .count();
        if (diff_mills < 500) {
            return; // Wait 250ms between tries.
        }
        if (diff_mills > 5000) {
            if (show_error_on_failure)
                Log::Error("Failed to equip item in bag %d slot %d", bag_idx,
                    slot_idx);
            ongoing = false;
            item = nullptr;
            return;
        }
    }

    if (!item || !item->item_id) {
        if (show_error_on_failure)
            Log::Error("Failed to equip item in bag %d slot %d", bag_idx,
                slot_idx);
        ongoing = false;
        item = nullptr;
        return;
    }
    if (item->bag && item->bag->bag_type == 2) {
        // Log::Info("Success!");
        ongoing = false;
        item = nullptr;
        return; // Success!
    }
    //GW::AgentLiving *p = GW::Agents::GetCharacter();
    GW::AgentLiving* p = GW::Agents::GetPlayerAsAgentLiving();
    if (!p || p->GetIsDead()) {
        if (show_error_on_failure)
            Log::Error("Failed to equip item in bag %d slot %d", bag_idx,
                slot_idx);
        ongoing = false;
        item = nullptr;
        return;
    }
    if (p->GetIsKnockedDown()) {
        // Log::Info("knocked down or missing"); // Player knocked down or
        // casting; wait.
        return;
    }
    if (p->skill) {
        GW::CtoS::SendPacket(0x4, GAME_CMSG_CANCEL_MOVEMENT); // Cancel action
                                                              // if casting a
                                                              // skill. Return
                                                              // here and wait
                                                              // before
                                                              // equipping
                                                              // items.
                                                              // Log::Info("cancel action");
        return;
    }
    if (p->GetIsIdle() || p->GetIsMoving()) {
        GW::Items::EquipItem(item);
        // Log::Info("equip %d", item->item_id);
    }
    else {
        GW::Agents::Move(p->pos); // Move to clear model state e.g. attacking,
                                  // aftercast
                                  // Log::Info("not idle nor moving, %d",p->model_state);
    }
}


bool HotkeyDropUseBuff::GetText(void *data, int idx, const char **out_text)
{
    static char other_buf[64];
    switch ((SkillIndex)idx) {
        case Recall:
            *out_text = u8"回归";
            break;
        case UA:
            *out_text = u8"坚毅灵气";
            break;
        case HolyVeil:
            *out_text = u8"神圣遮罩";
            break;
        default:
            snprintf(other_buf, 64, u8"技能ID:#%d", (int)data);
            *out_text = other_buf;
            break;
    }
    return true;
}
HotkeyDropUseBuff::SkillIndex HotkeyDropUseBuff::GetIndex() const
{
    switch (id) {
        case GW::Constants::SkillID::Recall:
            return Recall;
        case GW::Constants::SkillID::Unyielding_Aura:
            return UA;
        case GW::Constants::SkillID::Holy_Veil:
            return HolyVeil;
        default:
            return Other;
    }
}
HotkeyDropUseBuff::HotkeyDropUseBuff(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    id = (GW::Constants::SkillID)ini->GetLongValue(
        section, "SkillID", (long)GW::Constants::SkillID::Recall);
}
void HotkeyDropUseBuff::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetLongValue(section, "SkillID", (long)id);
}
void HotkeyDropUseBuff::Description(char *buf, size_t bufsz) const
{
    const char *skillname;
    GetText((void *)id, GetIndex(), &skillname);
    snprintf(buf, bufsz, "Drop/Use %s", skillname);
}
void HotkeyDropUseBuff::Draw()
{
    SkillIndex index = GetIndex();
    if (ImGui::Combo(u8"技能", (int *)&index,
                     u8"回归\0坚毅灵气a\0神圣遮罩\0其他", 4)) {
        switch (index) {
            case HotkeyDropUseBuff::Recall:
                id = GW::Constants::SkillID::Recall;
                break;
            case HotkeyDropUseBuff::UA:
                id = GW::Constants::SkillID::Unyielding_Aura;
                break;
            case HotkeyDropUseBuff::HolyVeil:
                id = GW::Constants::SkillID::Holy_Veil;
                break;
            case HotkeyDropUseBuff::Other:
                id = (GW::Constants::SkillID)0;
                break;
            default:
                break;
        }
        hotkeys_changed = true;
    }
    if (index == Other) {
        if (ImGui::InputInt(u8"技能 ID", (int *)&id))
            hotkeys_changed = true;
    }
}
void HotkeyDropUseBuff::Execute()
{
    if (!CanUse())
        return;

    GW::Buff *buff = GW::Effects::GetPlayerBuffBySkillId(id);
    if (buff && buff->skill_id) {
        GW::Effects::DropBuff(buff->buff_id);
    } else {
        int islot = GW::SkillbarMgr::GetSkillSlot(id);
        if (islot >= 0) {
            uint32_t slot = static_cast<uint32_t>(islot);
            if (GW::SkillbarMgr::GetPlayerSkillbar()->skills[slot].recharge == 0) {
                GW::GameThread::Enqueue([slot] () -> void {
                    GW::SkillbarMgr::UseSkill(slot, GW::Agents::GetTargetId(),
                        static_cast<uint32_t>(ImGui::IsKeyDown(VK_CONTROL)));
                });
            }
        }
    }
}

bool HotkeyToggle::GetText(void *, int idx, const char **out_text)
{
    switch ((Toggle)idx) {
        case Clicker:
            *out_text = u8"连续点击鼠标";
            return true;
        case Pcons:
            *out_text = u8"启停自动用药";
            return true;
        case CoinDrop:
            *out_text = u8"连续丢1g金币到地上";
            return true;
        case Tick:
            *out_text = u8"切换准备就绪和未就绪";
            return true;
        default:
            return false;
    }
}

bool HotkeyToggle::IsValid(CSimpleIni *ini, const char *section)
{
    switch (ini->GetLongValue(section, "ToggleID", static_cast<long>(Clicker))) {
        case static_cast<long>(Clicker):
        case static_cast<long>(Pcons):
        case static_cast<long>(CoinDrop):
        case static_cast<long>(Tick):
            return true;
        default:
            return false;
    }
}
HotkeyToggle::HotkeyToggle(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    target = (Toggle)ini->GetLongValue(section, "ToggleID", (long)Clicker);
}
void HotkeyToggle::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetLongValue(section, "ToggleID", (long)target);
}
void HotkeyToggle::Description(char *buf, size_t bufsz) const
{
    const char *name;
    GetText(nullptr, (int)target, &name);
    snprintf(buf, bufsz, "Toggle %s", name);
}
void HotkeyToggle::Draw()
{
    if (ImGui::Combo(u8"切换###combo", (int *)&target, GetText, nullptr,
                     n_targets))
        hotkeys_changed = true;
    if (ImGui::Checkbox(u8"触发的时候，消息提示我",
                        &show_message_in_emote_channel))
        hotkeys_changed = true;
}
void HotkeyToggle::Execute()
{
    bool _active;
    switch (target) {
        case HotkeyToggle::Clicker:
            _active = HotkeysWindow::Instance().ToggleClicker();
            if (show_message_in_emote_channel)
                Log::Info(u8"鼠标连续点击 %s", _active ? u8"已启用" : u8"已停用");
            break;
        case HotkeyToggle::Pcons:
            PconsWindow::Instance().ToggleEnable();
            // writing to chat is done by ToggleActive if needed
            break;
        case HotkeyToggle::CoinDrop:
            _active = HotkeysWindow::Instance().ToggleCoinDrop();
            if (show_message_in_emote_channel)
                Log::Info(u8"自动丢1g金币到地上 %s",
                          _active ? u8"已启用" : u8"已停用");
            break;
        case HotkeyToggle::Tick:
            const auto ticked = GW::PartyMgr::GetIsPlayerTicked();
            GW::PartyMgr::Tick(!ticked);
            break;
    }
}

bool HotkeyAction::GetText(void *, int idx, const char **out_text)
{
    switch ((Action)idx) {
        case OpenXunlaiChest:
            *out_text = u8"打开桑莱箱子";
            return true;
        case OpenLockedChest:
            *out_text = u8"打开锁住的宝箱";
            return true;
        case DropGoldCoin:
            *out_text = u8"丢金币到地上";
            return true;
        case ReapplyTitle:
            *out_text = u8"显示头衔";
            return true;
        case EnterChallenge:
            *out_text = u8"开始主线或挑战任务";
            return true;
        default:
            return false;
    }
}
HotkeyAction::HotkeyAction(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    action =
        (Action)ini->GetLongValue(section, "ActionID", (long)OpenXunlaiChest);
}
void HotkeyAction::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetLongValue(section, "ActionID", (long)action);
}
void HotkeyAction::Description(char *buf, size_t bufsz) const
{
    const char *name;
    GetText(nullptr, (int)action, &name);
    snprintf(buf, bufsz, "%s", name);
}
void HotkeyAction::Draw()
{
    if (ImGui::Combo(u8"执行###combo", (int *)&action, GetText, nullptr,
                     n_actions))
        hotkeys_changed = true;
}
void HotkeyAction::Execute()
{
    if (!CanUse())
        return;
    switch (action) {
        case HotkeyAction::OpenXunlaiChest:
            if (isOutpost()) {
                GW::GameThread::Enqueue([]() { 
                    GW::Items::OpenXunlaiWindow();
                    });
            }
            break;
        case HotkeyAction::OpenLockedChest: {
            if (isExplorable()) {
                GW::Agent *target = GW::Agents::GetTarget();
                if (target && target->type == 0x200) {
                    GW::Agents::GoSignpost(target);
                    GW::Items::OpenLockedChest();
                }
            }
            break;
        }
        case HotkeyAction::DropGoldCoin:
            if (isExplorable()) {
                GW::Items::DropGold(1);
            }
            break;
        case HotkeyAction::ReapplyTitle:
            ChatCommands::CmdReapplyTitle(nullptr, 0, nullptr);
            break;
        case HotkeyAction::EnterChallenge:
            GW::Chat::SendChat('/',L"enter");
            break;
    }
}

HotkeyTarget::HotkeyTarget(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    // don't print target hotkey to chat by default
    show_message_in_emote_channel = false;
    name[0] = 0;
    if (!ini)
        return;
    long ini_id = ini->GetLongValue(section, "TargetID", -1);
    if (ini_id > 0)
        id = static_cast<uint32_t>(ini_id);
    long ini_type = ini->GetLongValue(section, "TargetType", -1);
    if (ini_type >= HotkeyTargetType::NPC && ini_type < HotkeyTargetType::Count)
        type = static_cast<HotkeyTargetType>(ini_type);
    std::string ini_name = ini->GetValue(section, "TargetName", "");
    strcpy_s(name, ini_name.substr(0, sizeof(name)-1).c_str());
    name[sizeof(name)-1] = 0;
    
    ini->GetBoolValue(section, VAR_NAME(show_message_in_emote_channel),
                        show_message_in_emote_channel);
}
void HotkeyTarget::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetLongValue(section, "TargetID", static_cast<long>(id));
    ini->SetLongValue(section, "TargetType", static_cast<long>(type));
    ini->SetValue(section, "TargetName", name);
}
void HotkeyTarget::Description(char *buf, size_t bufsz) const
{
    if (!name[0]) {
        snprintf(buf, bufsz, "Target %s #%d", type_labels[type], id);
    } else {
        snprintf(buf, bufsz, "Target %s", name);
    }
}
void HotkeyTarget::Draw()
{
    ImGui::PushItemWidth(ImGui::GetWindowContentRegionWidth() / 3);
    hotkeys_changed |= ImGui::Combo(u8"类型", (int *)&type, type_labels, 3);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth() / 2);
    hotkeys_changed |= ImGui::InputInt(identifier_labels[type], (int *)&id);
    ImGui::PopItemWidth();
    hotkeys_changed |= ImGui::InputText(u8"名称", name, 140);
    hotkeys_changed |= ImGui::Checkbox(u8"当执行的时候，消息提示我", &show_message_in_emote_channel);
}
void HotkeyTarget::Execute()
{
    if (!CanUse() || id == 0)
        return;

    const GW::AgentArray& agents = GW::Agents::GetAgentArray();
    if (!agents.valid())
        return;

    GW::Agent *me = agents[GW::Agents::GetPlayerId()];
    if (me == nullptr)
        return;

    float distance = GW::Constants::SqrRange::Compass;
    size_t closest = (size_t)-1;

    for (size_t i = 0, size = agents.size();i < size; ++i) {
        if (!agents[i] || agents[i]->type != types[type])
            continue;
        switch (types[type]) {
            case 0x400: {
                GW::AgentItem *agent = agents[i]->GetAsAgentItem();
                if (!agent) continue;
                GW::Item *item = GW::Items::GetItemById(agent->item_id);
                if (!item || item->model_id != id)
                    continue;
            } break;
            case 0x200: {
                GW::AgentGadget *agent = agents[i]->GetAsAgentGadget();
                if (!agent || agent->gadget_id != id)
                    continue;
            } break;
            default: {
                GW::AgentLiving *agent = agents[i]->GetAsAgentLiving();
                if (!agent || agent->player_number != id || agent->hp <= 0)
                    continue;
            } break;
        }
        float newDistance = GW::GetSquareDistance(me->pos, agents[i]->pos);
        if (newDistance < distance) {
            closest = i;
            distance = newDistance;
        }
    }
    if (closest != (size_t)-1) {
        GW::Agent *agent = agents[closest];
        GW::GameThread::Enqueue([agent] { GW::Agents::ChangeTarget(agent); });
    }
    if (show_message_in_emote_channel) {
        char buf[256];
        Description(buf, 256);
        Log::Info(u8"已触发 %s", buf);
    }
}

HotkeyMove::HotkeyMove(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    x = (float)ini->GetDoubleValue(section, "x", 0.0);
    y = (float)ini->GetDoubleValue(section, "y", 0.0);
    range = (float)ini->GetDoubleValue(section, "distance",
                                       GW::Constants::Range::Compass);
    strcpy_s(name, ini->GetValue(section, "name", ""));
}
void HotkeyMove::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetDoubleValue(section, "x", x);
    ini->SetDoubleValue(section, "y", y);
    ini->SetDoubleValue(section, "distance", range);
    ini->SetValue(section, "name", name);
}
void HotkeyMove::Description(char *buf, size_t bufsz) const
{
    if (name[0] == '\0') {
        snprintf(buf, bufsz, "Move to (%.0f, %.0f)", x, y);
    } else {
        snprintf(buf, bufsz, "Move to %s", name);
    }
}
void HotkeyMove::Draw()
{
    if (ImGui::InputFloat("x", &x, 0.0f, 0.0f))
        hotkeys_changed = true;
    if (ImGui::InputFloat("y", &y, 0.0f, 0.0f))
        hotkeys_changed = true;
    if (ImGui::InputFloat(u8"范围", &range, 0.0f, 0.0f))
        hotkeys_changed = true;
    ImGui::ShowHelp(
        "此快捷键只会在此范围内执行.\n无限制的时候，请设置为0.");
    if (ImGui::InputText(u8"名称", name, 140))
        hotkeys_changed = true;
    if (ImGui::Checkbox(u8"当此快捷键执行的时候，消息提示我",
                        &show_message_in_emote_channel))
        hotkeys_changed = true;
}
void HotkeyMove::Execute()
{
    if (!CanUse())
        return;
    GW::Agent *me = GW::Agents::GetPlayer();
    double dist = GW::GetDistance(me->pos, GW::Vec2f(x, y));
    if (range != 0 && dist > range)
        return;
    GW::Agents::Move(x, y);
    if (name[0] == '\0') {
        if (show_message_in_emote_channel)
            Log::Info(u8"移动到 (%.0f, %.0f)", x, y);
    } else {
        if (show_message_in_emote_channel)
            Log::Info(u8"移动到 %s", name);
    }
}

HotkeyDialog::HotkeyDialog(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    id = static_cast<size_t>(ini->GetLongValue(section, "DialogID", 0));
    strcpy_s(name, ini->GetValue(section, "DialogName", ""));
}
void HotkeyDialog::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetLongValue(section, "DialogID", static_cast<long>(id));
    ini->SetValue(section, "DialogName", name);
}
void HotkeyDialog::Description(char *buf, size_t bufsz) const
{
    if (name[0] == '\0') {
        snprintf(buf, bufsz, "Dialog #%zu", id);
    } else {
        snprintf(buf, bufsz, "Dialog %s", name);
    }
}
void HotkeyDialog::Draw()
{
    if (ImGui::InputInt(u8"任务 ID", (int *)&id))
        hotkeys_changed = true;
    if (ImGui::InputText(u8"任务 名称", name, 140))
        hotkeys_changed = true;
    if (ImGui::Checkbox(u8"当此快捷键触发的时候，消息提示我",
                        &show_message_in_emote_channel))
        hotkeys_changed = true;
}
void HotkeyDialog::Execute()
{
    if (!CanUse())
        return;
    if (id == 0)
        return;
    GW::Agents::SendDialog(id);
    if (show_message_in_emote_channel)
        Log::Info(u8"接任务 %s,ID: (%d)", name, id);
}

bool HotkeyPingBuild::GetText(void *, int idx, const char **out_text)
{
    if (idx >= (int)BuildsWindow::Instance().BuildCount())
        return false;
    *out_text = BuildsWindow::Instance().BuildName(static_cast<size_t>(idx));
    return true;
}
HotkeyPingBuild::HotkeyPingBuild(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    index = static_cast<size_t>(ini->GetLongValue(section, "BuildIndex", 0));
}
void HotkeyPingBuild::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetLongValue(section, "BuildIndex", static_cast<long>(index));
}
void HotkeyPingBuild::Description(char *buf, size_t bufsz) const
{
    const char *buildname = BuildsWindow::Instance().BuildName(index);
    if (buildname == nullptr)
        buildname = u8"<没找到>";
    snprintf(buf, bufsz, "Ping build '%s'", buildname);
}
void HotkeyPingBuild::Draw()
{
    int icount = static_cast<int>(BuildsWindow::Instance().BuildCount());
    int iindex = static_cast<int>(index);
    if (ImGui::Combo(u8"组技能", &iindex, GetText, nullptr, icount)) {
        if (0 <= iindex)
            index = static_cast<size_t>(iindex);
        hotkeys_changed = true;
    }
}
void HotkeyPingBuild::Execute()
{
    if (!CanUse())
        return;

    BuildsWindow::Instance().Send(index);
}

bool HotkeyHeroTeamBuild::GetText(void *, int idx, const char **out_text)
{
    if (idx >= (int)HeroBuildsWindow::Instance().BuildCount())
        return false;
    size_t index = static_cast<size_t>(idx);
    *out_text = HeroBuildsWindow::Instance().BuildName(index);
    return true;
}
HotkeyHeroTeamBuild::HotkeyHeroTeamBuild(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    index = static_cast<size_t>(ini->GetLongValue(section, "BuildIndex", 0));
}
void HotkeyHeroTeamBuild::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetLongValue(section, "BuildIndex", static_cast<long>(index));
}
void HotkeyHeroTeamBuild::Description(char *buf, size_t bufsz) const
{
    const char *buildname = HeroBuildsWindow::Instance().BuildName(index);
    if (buildname == nullptr)
        buildname = u8"<没有找到>";
    snprintf(buf, bufsz, "Load Hero Team Build '%s'", buildname);
}
void HotkeyHeroTeamBuild::Draw()
{
    int icount = static_cast<int>(HeroBuildsWindow::Instance().BuildCount());
    int iindex = static_cast<int>(index);
    if (ImGui::Combo(u8"组技能", &iindex, GetText, nullptr, icount)) {
        if (0 <= iindex)
            index = static_cast<size_t>(iindex);
        hotkeys_changed = true;
    }
}
void HotkeyHeroTeamBuild::Execute()
{
    if (!CanUse())
        return;
    HeroBuildsWindow::Instance().Load(index);
}

HotkeyFlagHero::HotkeyFlagHero(CSimpleIni *ini, const char *section)
    : TBHotkey(ini, section)
{
    degree = static_cast<float>(ini->GetDoubleValue(section, "degree", degree));
    distance = static_cast<float>(ini->GetDoubleValue(section, "distance", distance));
    hero = ini->GetLongValue(section, "hero", hero);
    if (hero < 0) hero = 0;
    if (hero > 11) hero = 11;
}
void HotkeyFlagHero::Save(CSimpleIni *ini, const char *section) const
{
    TBHotkey::Save(ini, section);
    ini->SetDoubleValue(section, "degree", degree);
    ini->SetDoubleValue(section, "distance", distance);
    ini->SetLongValue(section, "hero", hero);
}
void HotkeyFlagHero::Description(char *buf, size_t bufsz) const
{
    if (hero == 0) {
        snprintf(buf, bufsz, "Flag All Heroes");
    } else {
        snprintf(buf, bufsz, "Flag Hero %d", hero);
    }
}
void HotkeyFlagHero::Draw()
{
    hotkeys_changed |= ImGui::DragFloat(u8"角度", &degree, 0.0f, -360.0f, 360.f);
    hotkeys_changed |= ImGui::DragFloat(u8"距离", &distance, 0.0f, 0.0f, 10'000.f);
    if (hotkeys_changed && distance < 0.f)
        distance = 0.f;
    hotkeys_changed |= ImGui::InputInt(u8"英雄", &hero, 1);
    if (hotkeys_changed && hero < 0)
        hero = 0;
    else if (hotkeys_changed && hero > 11)
        hero = 11;
    ImGui::ShowHelp(u8"可以插旗控制的英雄为1-11号.\n如果是0，则控制所有英雄.");
    ImGui::Text(u8"如果要在超级雷达中插旗控英雄，请使用下面的指令:");
    ImGui::TextColored({1.f, 1.f, 0.f, 1.f}, "/flag %d toggle", hero);
}
void HotkeyFlagHero::Execute()
{
    if (!isExplorable())
        return;

    const GW::Vec3f allflag = GW::GameContext::instance()->world->all_flag;

    if (hero < 0)
        return;
    if (hero == 0) {
        if (allflag.x != 0 && allflag.y != 0 && (!std::isinf(allflag.x) || !std::isinf(allflag.y))) {
            GW::PartyMgr::UnflagAll();
            return;
        }
    } else {
        const GW::HeroFlagArray &flags = GW::GameContext::instance()->world->hero_flags;
        if (!flags.valid() || static_cast<uint32_t>(hero) > flags.size())
            return;

        const GW::HeroFlag &flag = flags[hero - 1];
        if (!std::isinf(flag.flag.x) || !std::isinf(flag.flag.y)) {
            GW::PartyMgr::UnflagHero(hero);
            return; 
        }
    }

    const GW::AgentLiving *player = GW::Agents::GetPlayerAsAgentLiving();
    if (!player)
        return;
    const GW::AgentLiving *target = GW::Agents::GetTargetAsAgentLiving();

    float reference_radiant = player->rotation_angle;

    if (target && target != player) {
        float dx = target->x - player->x;
        float dy = target->y - player->y;

        reference_radiant = std::atan(dx == 0 ? dy : dy / dx);
        if (dx < 0) {
            reference_radiant += static_cast<float>(M_PI);
        } else if (dx > 0 && dy < 0) {
            reference_radiant += 2 * static_cast<float>(M_PI);
        }
    }

    const float radiant = degree * static_cast<float>(M_PI) / 180.f;
    const float x = player->x + distance * std::cos(reference_radiant - radiant);
    const float y = player->y + distance * std::sin(reference_radiant - radiant);

    GW::GamePos pos = GW::GamePos(x, y, 0);

    if (hero == 0) {
        GW::PartyMgr::FlagAll(pos);
    } else {
        GW::PartyMgr::FlagHero(hero, pos);
    }
}