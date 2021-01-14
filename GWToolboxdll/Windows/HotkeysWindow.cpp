#include "stdafx.h"

#include <GWCA/Constants/Constants.h>
#include <GWCA/GameContainers/Array.h>
#include <GWCA/GameEntities/Agent.h>

#include <GWCA/Managers/ItemMgr.h>
#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/MemoryMgr.h>
#include <GWCA/Managers/StoCMgr.h>

#include <GuiUtils.h>
#include <Keys.h>
#include <Logger.h>

#include <Modules/Resources.h>
#include <Windows/HotkeysWindow.h>

void HotkeysWindow::Initialize() {
    ToolboxWindow::Initialize();
    clickerTimer = TIMER_INIT();
    dropCoinsTimer = TIMER_INIT();
}
void HotkeysWindow::Terminate() {
    ToolboxWindow::Terminate();
    for (TBHotkey* hotkey : hotkeys) {
        delete hotkey;
    }
}
bool HotkeysWindow::IsMapReady()
{
    return GW::Map::GetIsMapLoaded() && GW::Map::GetInstanceType() != GW::Constants::InstanceType::Loading && !GW::Map::GetIsObserving();
}

void HotkeysWindow::Draw(IDirect3DDevice9* pDevice) {
    UNREFERENCED_PARAMETER(pDevice);
    if (!visible) return;
    // === hotkey panel ===
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver, ImVec2(.5f, .5f));
    ImGui::SetNextWindowSize(ImVec2(300, 400), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(Name(), GetVisiblePtr(), GetWinFlags())) {
        if (ImGui::Button(u8"��˽���ݼ�...", ImVec2(ImGui::GetWindowContentRegionWidth(), 0))) {
            ImGui::OpenPopup(u8"����ݼ�");
        }
        if (ImGui::BeginPopup(u8"����ݼ�")) {
            if (ImGui::Selectable(u8"����ָ����")) {
                hotkeys.push_back(new HotkeySendChat(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"����ָ���������");
            if (ImGui::Selectable(u8"ʹ������Ʒ")) {
                hotkeys.push_back(new HotkeyUseItem(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"ʹ��ĳ��ҩƷ�����⣬��֮��");
            if (ImGui::Selectable(u8"���漼������")) {
                hotkeys.push_back(new HotkeyDropUseBuff(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"ʹ��ĳ���ӳּ��ܻ���ȡ���˼ӳּ���");
            if (ImGui::Selectable(u8"�Զ����ظ�ִ�еĹ���")) {
                hotkeys.push_back(new HotkeyToggle(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"����������ĳ���ط���������ͣ�Զ���ҩ����������ҵ����ϵ�");
            if (ImGui::Selectable(u8"ִ��һ�������ԵĶ���")) {
                hotkeys.push_back(new HotkeyAction(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"Զ�̿���ͨ���ѵȸ�������\n������ʾ����ͷ�Σ��������ͷ��.");
            if (ImGui::Selectable(u8"ѡ��Ŀ��")) {
                hotkeys.push_back(new HotkeyTarget(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"ͨ������Ŀ���ID,ʵ�ֿ���ѡ���Ŀ��");
            if (ImGui::Selectable(u8"�ƶ�������")) {
                hotkeys.push_back(new HotkeyMove(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"������Ҫ�ƶ�������ֵ�󣬵����ݼ����ƶ���������");
            if (ImGui::Selectable(u8"������")) {
                hotkeys.push_back(new HotkeyDialog(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"�Ӹ������񣬱������T2��");
            if (ImGui::Selectable(u8"���ͼ���")) {
                hotkeys.push_back(new HotkeyPingBuild(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"���Լ��������ļ��ܷ��ͳ�ȥ");
            if (ImGui::Selectable(u8"����Ӣ���ŶӺ�Ӣ���ŵļ���")) {
                hotkeys.push_back(new HotkeyHeroTeamBuild(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"�����Լ����õ�Ӣ���ź�����Ҫ�ļ���");
            if (ImGui::Selectable(u8"���������ͻ���")) {
                hotkeys.push_back(new HotkeyEquipItem(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered()) ImGui::SetTooltip(u8"ʹ��ĳ�����ף��صȻ��ף�����ĳ�����������ȣ�����ë��");
            if (ImGui::Selectable(u8"�������Ӣ��")) {
                hotkeys.push_back(new HotkeyFlagHero(nullptr, nullptr));
            }
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(u8"�������Ӣ�۵������õ�����");
            ImGui::EndPopup();
        }

        // === each hotkey ===
        block_hotkeys = false;
        for (unsigned int i = 0; i < hotkeys.size(); ++i) {
            TBHotkey::Op op = TBHotkey::Op_None;
            hotkeys[i]->Draw(&op);
            switch (op) {
            case TBHotkey::Op_None: break;
            case TBHotkey::Op_MoveUp:
                if (i > 0) std::swap(hotkeys[i], hotkeys[i - 1]);
                break;
            case TBHotkey::Op_MoveDown:
                if (i < hotkeys.size() - 1) {
                    std::swap(hotkeys[i], hotkeys[i + 1]);
                    // render the moved one and increase i
                    TBHotkey::Op op2;
                    hotkeys[i++]->Draw(&op2);
                }
                break;
            case TBHotkey::Op_Delete: {
                TBHotkey* hk = hotkeys[i];
                hotkeys.erase(hotkeys.begin() + static_cast<int>(i));
                delete hk;
                --i;
            }
                                      break;
            case TBHotkey::Op_BlockInput:
                block_hotkeys = true;
                break;

            default:
                break;
            }
        }
    }
    ImGui::End();
}

void HotkeysWindow::DrawSettingInternal() {
    ToolboxWindow::DrawSettingInternal();
    ImGui::Checkbox("Show 'Active' checkbox in header", &TBHotkey::show_active_in_header);
    ImGui::Checkbox("Show 'Run' button in header", &TBHotkey::show_run_in_header);
}

void HotkeysWindow::LoadSettings(CSimpleIni* ini) {
    ToolboxWindow::LoadSettings(ini);
    show_menubutton = ini->GetBoolValue(Name(), VAR_NAME(show_menubutton), true);

    TBHotkey::show_active_in_header = ini->GetBoolValue(Name(), "show_active_in_header", false);
    TBHotkey::show_run_in_header = ini->GetBoolValue(Name(), "show_run_in_header", false);

    // clear hotkeys from toolbox
    for (TBHotkey* hotkey : hotkeys) {
        delete hotkey;
    }
    hotkeys.clear();

    // then load again
    CSimpleIni::TNamesDepend entries;
    ini->GetAllSections(entries);
    for (CSimpleIni::Entry& entry : entries) {
        TBHotkey* hk = TBHotkey::HotkeyFactory(ini, entry.pItem);
        if (hk) hotkeys.push_back(hk);
    }

    TBHotkey::hotkeys_changed = false;
}
void HotkeysWindow::SaveSettings(CSimpleIni* ini) {
    ToolboxWindow::SaveSettings(ini);
    ini->SetBoolValue(Name(), "show_active_in_header", TBHotkey::show_active_in_header);
    ini->SetBoolValue(Name(), "show_run_in_header", TBHotkey::show_run_in_header);

    if (TBHotkey::hotkeys_changed) {
        // clear hotkeys from ini
        CSimpleIni::TNamesDepend entries;
        ini->GetAllSections(entries);
        for (CSimpleIni::Entry& entry : entries) {
            if (strncmp(entry.pItem, "hotkey-", 7) == 0) {
                ini->Delete(entry.pItem, nullptr);
            }
        }

        // then save again
        char buf[256];
        for (unsigned int i = 0; i < hotkeys.size(); ++i) {
            snprintf(buf, 256, "hotkey-%03d:%s", i, hotkeys[i]->Name());
            hotkeys[i]->Save(ini, buf);
        }
    }
}

bool HotkeysWindow::WndProc(UINT Message, WPARAM wParam, LPARAM lParam) {
    UNREFERENCED_PARAMETER(lParam);
    if (GW::Chat::GetIsTyping())
        return false;
    if (GW::MemoryMgr::GetGWWindowHandle() != GetActiveWindow())
        return false;
    long keyData = 0;
    switch (Message) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        keyData = static_cast<long>(wParam);
        break;
    case WM_XBUTTONDOWN:
    case WM_MBUTTONDOWN:
        if (LOWORD(wParam) & MK_MBUTTON) keyData = VK_MBUTTON;
        if (LOWORD(wParam) & MK_XBUTTON1) keyData = VK_XBUTTON1;
        if (LOWORD(wParam) & MK_XBUTTON2) keyData = VK_XBUTTON2;
        break;
    case WM_XBUTTONUP:
    case WM_MBUTTONUP:
        // leave keydata to none, need to handle special case below
        break;
    default:
        break;
    }

    switch (Message) {
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
    case WM_XBUTTONDOWN:
    case WM_MBUTTONDOWN: {
        long modifier = 0;
        if (GetKeyState(VK_CONTROL) < 0)
            modifier |= ModKey_Control;
        if (GetKeyState(VK_SHIFT) < 0)
            modifier |= ModKey_Shift;
        if (GetKeyState(VK_MENU) < 0)
            modifier |= ModKey_Alt;

        bool triggered = false;
        for (TBHotkey* hk : hotkeys) {
            if (!block_hotkeys && hk->active 
                && !hk->pressed && keyData == hk->hotkey
                && modifier == hk->modifier
                && (hk->map_id == 0 || hk->map_id == static_cast<int>(map_id))
                && (hk->prof_id == 0 || hk->prof_id == static_cast<int>(prof_id))) {

                hk->pressed = true;
                current_hotkey = hk;
                hk->Execute();
                current_hotkey = nullptr;
                if (hk->block_gw)
                    triggered = true;
            }
        }
        return triggered;
    }

    case WM_KEYUP:
    case WM_SYSKEYUP:
        for (TBHotkey* hk : hotkeys) {
            if (hk->pressed && keyData == hk->hotkey) {
                hk->pressed = false;
            }
        }
        return false;

    case WM_XBUTTONUP:
        for (TBHotkey* hk : hotkeys) {
            if (hk->pressed && (hk->hotkey == VK_XBUTTON1 || hk->hotkey == VK_XBUTTON2)) {
                hk->pressed = false;
            }
        }
        return false;
    case WM_MBUTTONUP:
        for (TBHotkey* hk : hotkeys) {
            if (hk->pressed && hk->hotkey == VK_MBUTTON) {
                hk->pressed = false;
            }
        }
    default:
        return false;
    }
}
void HotkeysWindow::MapChanged() {
    if (map_change_triggered)
        return;
    if (!IsMapReady())
        return;
    GW::AgentLiving* p = GW::Agents::GetPlayerAsAgentLiving();
    if (!p) return;
    map_id = (uint32_t)GW::Map::GetMapID();
    prof_id = p->primary;
    GW::Constants::InstanceType mt = GW::Map::GetInstanceType();
    if (mt != GW::Constants::InstanceType::Loading) {
        for (TBHotkey* hk : hotkeys) {
            if (!block_hotkeys && hk->active
                && ((hk->trigger_on_explorable && mt == GW::Constants::InstanceType::Explorable)
                    || (hk->trigger_on_outpost && mt == GW::Constants::InstanceType::Outpost))
                && !hk->pressed
                && (hk->map_id == 0 || hk->map_id == static_cast<int>(map_id))
                && (hk->prof_id == 0 || hk->prof_id == static_cast<int>(prof_id))) {

                hk->pressed = true;
                current_hotkey = hk;
                hk->Execute();
                current_hotkey = nullptr;
                hk->pressed = false;
            }
        }
        map_change_triggered = true;
    }
}

void HotkeysWindow::Update(float delta) {
    UNREFERENCED_PARAMETER(delta);
    if (GW::Map::GetInstanceType() == GW::Constants::InstanceType::Loading) {
        if (map_change_triggered)
            map_change_triggered = false;
        return;
    }
    if (!map_change_triggered)
        MapChanged();
    if (clickerActive && TIMER_DIFF(clickerTimer) > 20) {
        clickerTimer = TIMER_INIT();
        INPUT input;
        input.type = INPUT_MOUSE;
        input.mi.dx = 0;
        input.mi.dy = 0;
        input.mi.mouseData = 0;
        input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
        input.mi.time = 0;
        input.mi.dwExtraInfo = NULL;

        SendInput(1, &input, sizeof(INPUT));
    }

    if (dropCoinsActive && TIMER_DIFF(dropCoinsTimer) > 500) {
        if (GW::Map::GetInstanceType() == GW::Constants::InstanceType::Explorable) {
            dropCoinsTimer = TIMER_INIT();
            GW::Items::DropGold(1);
        }
    }

    for (unsigned int i = 0; i < hotkeys.size(); ++i) {
        if (hotkeys[i]->ongoing)
            hotkeys[i]->Execute();
    }

    // TODO rupt?
}
