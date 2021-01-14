#include "stdafx.h"

#include <GWCA/Constants/Constants.h>
#include <GWCA/GameContainers/Array.h>
#include <GWCA/GameContainers/GamePos.h>

#include <GWCA/GameEntities/Camera.h>

#include <GWCA/GameEntities/Party.h>
#include <GWCA/GameEntities/Quest.h>
#include <GWCA/GameEntities/Skill.h>
#include <GWCA/GameEntities/Player.h>
#include <GWCA/GameEntities/Guild.h>
#include <GWCA/GameEntities/NPC.h>
#include <GWCA/GameEntities/Camera.h>

#include <GWCA/Context/GameContext.h>
#include <GWCA/Context/WorldContext.h>

#include <GWCA/Managers/MapMgr.h>
#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/ItemMgr.h>
#include <GWCA/Managers/StoCMgr.h>
#include <GWCA/Managers/AgentMgr.h>
#include <GWCA/Managers/PlayerMgr.h>
#include <GWCA/Managers/PartyMgr.h>
#include <GWCA/Managers/EffectMgr.h>
#include <GWCA/Managers/GuildMgr.h>
#include <GWCA/Managers/GameThreadMgr.h>
#include <GWCA/Managers/CameraMgr.h>

#include <GWToolbox.h>
#include <Logger.h>

#include <Modules/Resources.h>
#include <Widgets/AlcoholWidget.h>
#include <Widgets/BondsWidget.h>
#include <Widgets/ClockWidget.h>
#include <Widgets/DistanceWidget.h>
#include <Widgets/HealthWidget.h>
#include <Widgets/Minimap/Minimap.h>
#include <Widgets/PartyDamage.h>
#include <Widgets/TimerWidget.h>
#include <Windows/InfoWindow.h>
#include <Windows/NotepadWindow.h>
#include <Windows/StringDecoderWindow.h>

#include <Modules/ToolboxSettings.h>

void InfoWindow::Initialize() {
    ToolboxWindow::Initialize();

    GW::StoC::RegisterPacketCallback<GW::Packet::StoC::MessageCore>(&MessageCore_Entry,OnMessageCore);
    GW::StoC::RegisterPacketCallback<GW::Packet::StoC::QuotedItemPrice>(&InstanceLoadFile_Entry,
        [this](GW::HookStatus*, GW::Packet::StoC::QuotedItemPrice* packet) -> void {
            quoted_item_id = packet->itemid;
        });
    GW::StoC::RegisterPacketCallback<GW::Packet::StoC::DialogSender>(&OnDialog_Entry, [this](...) {
            ClearAvailableDialogs();
        });
    GW::StoC::RegisterPacketCallback<GW::Packet::StoC::DialogButton>(&OnDialog_Entry,
        [this](GW::HookStatus*, GW::Packet::StoC::DialogButton* packet) -> void {
            available_dialogs.push_back(new AvailableDialog(packet->message, packet->dialog_id));
        });
    GW::StoC::RegisterPacketCallback<GW::Packet::StoC::InstanceLoadFile>(&InstanceLoadFile_Entry,OnInstanceLoad);

    GW::Chat::CreateCommand(L"resignlog", CmdResignLog);
}

void InfoWindow::CmdResignLog(const wchar_t* cmd, int argc, wchar_t** argv) {
    UNREFERENCED_PARAMETER(cmd);
    UNREFERENCED_PARAMETER(argc);
    UNREFERENCED_PARAMETER(argv);
    if (GW::Map::GetInstanceType() != GW::Constants::InstanceType::Explorable) return;
    GW::PartyInfo* info = GW::PartyMgr::GetPartyInfo();
    if (info == nullptr) return;
    GW::PlayerPartyMemberArray partymembers = info->players;
    if (!partymembers.valid()) return;
    GW::PlayerArray players = GW::Agents::GetPlayerArray();
    if (!players.valid()) return;
    auto instance = &Instance();
    size_t index_max = std::min<size_t>(instance->status.size(), partymembers.size());
    for (size_t i = 0; i < index_max; ++i) {
        GW::PlayerPartyMember& partymember = partymembers[i];
        if (partymember.login_number >= players.size()) continue;
        GW::Player& player = players[partymember.login_number];

        wchar_t buffer[256];
        if (instance->status[i] == Connected) {
            instance->PrintResignStatus(buffer, 256, i, player.name);
            instance->send_queue.push(std::wstring(buffer));
        }
    }
}

void InfoWindow::OnInstanceLoad(GW::HookStatus*, GW::Packet::StoC::InstanceLoadFile* packet) {
    auto instance = &Instance();
    instance->quoted_item_id = 0;
    instance->mapfile = packet->map_fileID;
    for (unsigned i = 0; i < instance->status.size(); ++i) {
        instance->status[i] = NotYetConnected;
        instance->timestamp[i] = 0;
    }
}

void InfoWindow::OnMessageCore(GW::HookStatus*, GW::Packet::StoC::MessageCore* pak) {
    // 0x107 is the "start string" marker
    if (wmemcmp(pak->message, L"\x7BFF\xC9C4\xAEAA\x1B9B\x107", 5) != 0)
        return;

    // get all the data
    GW::PartyInfo* info = GW::PartyMgr::GetPartyInfo();
    if (info == nullptr) return;
    GW::PlayerPartyMemberArray partymembers = info->players;
    if (!partymembers.valid()) return;
    GW::PlayerArray players = GW::Agents::GetPlayerArray();
    if (!players.valid()) return;

    // Prepare the name
    const int offset = 5;
    wchar_t buf[256];
    {
        int i = 0;
        while (i < 255 && pak->message[i + offset] != 0x1 && pak->message[i + offset] != 0) {
            buf[i] = (pak->message[offset + i]);
            ++i;
        }
        buf[i] = '\0';
    }

    // set the right index in party
    auto instance = &Instance();
    for (unsigned i = 0; i < partymembers.size(); ++i) {
        if (i >= instance->status.size()) continue;
        if (instance->status[i] == Resigned) continue;
        if (partymembers[i].login_number >= players.size()) continue;
        if (GuiUtils::SanitizePlayerName(players[partymembers[i].login_number].name) == buf) {
            instance->status[i] = Resigned;
            Instance().timestamp[i] = GW::Map::GetInstanceTime();
            break;
        }
    }
}

void InfoWindow::DrawItemInfo(GW::Item* item, ForDecode* name) {
    if (!item) return;
    name->init(item->single_item_name);
    static char modelid[32] = "";
    static char slot[12] = "";
    static char encname_buf[32] = "";
    static char encdesc_buf[512] = "";
    strcpy_s(modelid, "-");
    strcpy_s(slot, "-");
    strcpy_s(encname_buf, "-");
    strcpy_s(encdesc_buf, "-");
    if (snprintf(modelid, 32, "%d", item->model_id) < 0)
        return;
    if (item->bag) {
        if (snprintf(slot, 12, "%d/%d", item->bag->index + 1, item->slot + 1) < 0)
            return;
    }

    ImGui::PushItemWidth(-80.0f);
    ImGui::LabelText(u8"��Ʒ��ID", slot, 12, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputText(u8"ģ��ID", modelid, 32, ImGuiInputTextFlags_ReadOnly);
    ImGui::InputText(u8"����", name->str(), 64, ImGuiInputTextFlags_ReadOnly);
    //ImGui::InputText("ItemID", itemid, 32, ImGuiInputTextFlags_ReadOnly);
    ImGui::PopItemWidth();
    if (ImGui::TreeNodeEx(u8"�߼�����##item", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth)) {
        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2);
        if (item) {
            ImGui::LabelText(u8"�ڴ��ַ", "%p", item);
            ImGui::LabelText(u8"Id", "%d", item->item_id);
            ImGui::LabelText(u8"����", "%d", item->type);
            ImGui::LabelText(u8"��Ⱦ", "0x%X", item->interaction);
            ImGui::LabelText(u8"ģ�͵��ļ���ID", "0x%X", item->model_file_id);
            if (item->name_enc) {
                size_t offset = 0;
                for (size_t i = 0; item->name_enc[i]; i++) {
                    offset += sprintf(encname_buf + offset, "0x%X ", item->name_enc[i]);
                }
            }
            ImGui::InputText(u8"����������", item->name_enc ? encname_buf : "-", 32, ImGuiInputTextFlags_ReadOnly);
            if (item->info_string) {
                size_t offset = 0;
                for (size_t i = 0; item->info_string[i]; i++) {
                    offset += sprintf(encdesc_buf + offset, "0x%X ", item->info_string[i]);
                }
            }
            ImGui::InputText(u8"����������", item->info_string ? encdesc_buf : "-", 512, ImGuiInputTextFlags_ReadOnly);
            if (item->mod_struct_size) {
                ImGui::Text("Mod Struct (identifier, arg1, arg2)");
            }
            char mod_struct_label[] = "###Mod Struct 1";
            char mod_struct_buf[64];
            for (size_t i = 0; i < item->mod_struct_size; i++) {
                GW::ItemModifier* mod = &item->mod_struct[i];
                mod_struct_label[14] = static_cast<char>(i + 1) + '0';
                sprintf(mod_struct_buf, "0x%X (%d %d %d)", mod->mod, mod->identifier(), mod->arg1(), mod->arg2());
                ImGui::InputText(mod_struct_label, mod_struct_buf, 64, ImGuiInputTextFlags_ReadOnly);
            }
        }
        ImGui::PopItemWidth();
        ImGui::TreePop();
    }
}
void InfoWindow::Draw(IDirect3DDevice9* pDevice) {
    UNREFERENCED_PARAMETER(pDevice);
    if (!visible) return;
    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver, ImVec2(.5f, .5f));
    ImGui::SetNextWindowSize(ImVec2(300, 0), ImGuiCond_FirstUseEver);
    if (ImGui::Begin(Name(), GetVisiblePtr(), GetWinFlags())) {
        if (show_widgets) {
            const std::vector<ToolboxModule *> &optional_modules = ToolboxSettings::Instance().GetOptionalModules();
            std::vector<ToolboxUIElement *> widgets;
            widgets.reserve(optional_modules.size());
            for (ToolboxModule *module : optional_modules) {
                ToolboxUIElement *widget = dynamic_cast<ToolboxUIElement *>(module);
                if (!widget || !widget->IsWidget())
                    continue;
                widgets.push_back(widget);
            }
            std::sort(widgets.begin(), widgets.end(), [](auto *a, auto *b) { return std::strcmp(a->Name(), b->Name()) < 0; });
            const unsigned cols = static_cast<unsigned>(ceil(ImGui::GetWindowSize().x / 200.f));
            ImGui::PushID("info_enable_widget_items");
            ImGui::Columns(static_cast<int>(cols), "info_enable_widgets", false);
            const size_t items_per_col = static_cast<size_t>(ceil(static_cast<float>(widgets.size()) / cols));
            size_t col_count = 0u;
            for (ToolboxUIElement* widget : widgets) {
                ImGui::Checkbox(widget->Name(), &widget->visible);
                if (++col_count == items_per_col) {
                    ImGui::NextColumn();
                    col_count = 0u;
                }
            }
            ImGui::Columns(1);
            ImGui::PopID();
        }

        if (show_open_chest) {
            if (ImGui::Button(u8"��ɣ��������", ImVec2(-1.0f, 0))) {
                GW::GameThread::Enqueue([]() {
                    GW::Items::OpenXunlaiWindow();
                });
            }
        }
        
        if (ImGui::CollapsingHeader(u8"�������")) {
            static char pos_buf[32];
            static char target_buf[32];
            static char angle_buf[32];
            static GW::Camera* cam;
            if ((cam = GW::CameraMgr::GetCamera()) != nullptr) {
                snprintf(pos_buf, 32, "%.2f, %.2f, %.2f", cam->position.x, cam->position.y, cam->position.z);
                snprintf(target_buf, 32, "%.2f, %.2f, %.2f", cam->look_at_target.x, cam->look_at_target.y, cam->look_at_target.z);
                snprintf(angle_buf, 32, "%.2f, %.2f", cam->GetCurrentYaw(), cam->pitch);
            }
            ImGui::PushItemWidth(-80.0f);
            ImGui::InputText(u8"����##cam_pos", pos_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"Ŀ��##cam_target", target_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText("Yaw/Pitch##cam_angle", angle_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopItemWidth();
        }
        if (show_player && ImGui::CollapsingHeader(u8"��ɫ����")) {
            static char x_buf[32] = "";
            static char y_buf[32] = "";
            static char s_buf[32] = "";
            static char agentid_buf[32] = "";
            static char modelid_buf[32] = "";
            GW::AgentLiving* player = GW::Agents::GetPlayerAsAgentLiving();
            if (player) {
                snprintf(x_buf, 32, "%.2f", player->pos.x);
                snprintf(y_buf, 32, "%.2f", player->pos.y);
                float s = sqrtf(player->move_x * player->move_x + player->move_y * player->move_y);
                snprintf(s_buf, 32, "%.3f", s / 288.0f);
                snprintf(agentid_buf, 32, "%d", player->agent_id);
                snprintf(modelid_buf, 32, "%d", player->player_number);
            }
            ImGui::PushItemWidth(-80.0f);
            ImGui::InputText(u8"X ����##player", x_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"Y ����##player", y_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"�˶��ٶ�##player", s_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"������ ID##player", agentid_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::ShowHelp(u8"��Ϸ�ﴴ����ÿ�������壬���ǵ�ID���ǲ�һ����,\n�������ID���ڹ��촴����ʱ�򣬴�����һ������");
            ImGui::InputText(u8"��ɫ ID##player", modelid_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::ShowHelp(u8"��ɫҲ���ڹ����壬��Ϊ�������ID���ظ��ģ����Խ�ɫ��IDҲ��Ψһ��");
            ImGui::PopItemWidth();
            if (ImGui::TreeNodeEx(u8"����ЧӦ##player", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth)) {
                GW::EffectArray effects = GW::Effects::GetPlayerEffectArray();
                if (effects.valid()) {
                    for (DWORD i = 0; i < effects.size(); ++i) {
                        ImGui::Text("id: %d", effects[i].skill_id);
                        uint32_t time = effects[i].GetTimeRemaining();
                        ImGui::SameLine();
                        ImGui::Text(u8" ʣ��ʱ��: %u", time / 1000);
                    }
                }
                ImGui::TreePop();
            }
            if (ImGui::TreeNodeEx(u8"�ӳ�##player", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth)) {
                GW::BuffArray effects = GW::Effects::GetPlayerBuffArray();
                if (effects.valid()) {
                    for (DWORD i = 0; i < effects.size(); ++i) {
                        ImGui::Text("id: %d", effects[i].skill_id);
                        if (effects[i].target_agent_id) {
                            ImGui::SameLine();
                            ImGui::Text(u8" Ŀ��ID: %d", effects[i].target_agent_id);
                        }
                    }
                }
                ImGui::TreePop();
            }

        }
        if (show_target && ImGui::CollapsingHeader(u8"Ŀ�����")) {
            static char x_buf[32] = "";
            static char y_buf[32] = "";
            static char s_buf[32] = "";
            static char agentid_buf[32] = "";
            static char modelid_buf[32] = "";
            static char encname_buf[64] = "";
            GW::Agent* target = GW::Agents::GetTarget();
            GW::AgentLiving* target_living = target ? target->GetAsAgentLiving() : nullptr;
            GW::AgentItem* target_item = target ? target->GetAsAgentItem() : nullptr;
            GW::AgentGadget* target_gadget = target ? target->GetAsAgentGadget() : nullptr;
            if (target) {
                snprintf(x_buf, 32, "%.2f", target->pos.x);
                snprintf(y_buf, 32, "%.2f", target->pos.y);
                float s = sqrtf(target->move_x * target->move_x + target->move_y * target->move_y);
                snprintf(s_buf, 32, "%.3f", s / 288.0f);
                
                if (target->GetIsItemType())
                    snprintf(modelid_buf, 32, "%d", target_item ? GW::Items::GetItemById(target_item->item_id)->model_id : 0);
                else
                    snprintf(modelid_buf, 32, "%d", target_living ? target_living->player_number : 0);
                snprintf(agentid_buf, 32, "%d", target->agent_id);
                wchar_t* enc_name = GW::Agents::GetAgentEncName(target);
                if (enc_name) {
                    size_t offset = 0;
                    for (size_t i = 0; enc_name[i]; i++) {
                        offset += sprintf(encname_buf + offset, "0x%X ", enc_name[i]);
                    }
                }
            } else {
                snprintf(x_buf, 32, "-");
                snprintf(y_buf, 32, "-");
                snprintf(s_buf, 32, "-");
                snprintf(agentid_buf, 32, "-");
                snprintf(modelid_buf, 32, "-");
                snprintf(encname_buf, 64, "-");
            }
            ImGui::PushItemWidth(-80.0f);
            ImGui::InputText(u8"X ����##target", x_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"Y ����##target", y_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"�˶��ٶ�##target", s_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"������ ID##target", agentid_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::ShowHelp(u8"��Ϸ�ﴴ����ÿ�������壬���ǵ�ID���ǲ�һ����,\n�������ID���ڹ��촴����ʱ�򣬴�����һ������");
            ImGui::InputText(u8"ģ�� ID##target", modelid_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::ShowHelp(u8"ģ�� ID ����ͬһ��Ĺ����壬����һ����.\nģ��ID��ͬһ�๹���干���һ������.\n��Ŀ������ҽ�ɫ��ʱ�����ID����ҽ�ɫ�Ĵ���.\n���ģ��ID��Ҫ������ѡ��һ���ض������.");
            ImGui::InputText("Agent Enc Name##target", encname_buf, 64, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopItemWidth();
            GW::Player* player = nullptr;
            GW::Guild* guild = nullptr;
            GW::NPC* npc = nullptr;
            if (target_living && target_living->IsPlayer()) {
                player = GW::PlayerMgr::GetPlayerByID(target_living->player_number);
                if(target_living->transmog_npc_id & 0x20000000)
                    npc = GW::Agents::GetNPCByID(target_living->transmog_npc_id ^ 0x20000000);
            }
            if (target_living && target_living->IsNPC()) {
                npc = GW::Agents::GetNPCByID(target_living->player_number);
            }
            if (target_living && target_living->tags->guild_id) {
                GW::GuildArray guilds = GW::GuildMgr::GetGuildArray();
                if (guilds.valid() && target_living->tags->guild_id < guilds.size())
                    guild = guilds[target_living->tags->guild_id];
            }
            if (target) {
                if (ImGui::TreeNodeEx(u8"Ŀ��ĸ߼�����##target", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth)) {
                    GW::Agent *me = GW::Agents::GetPlayer();
                    
                    ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2);
                    ImGui::LabelText(u8"�ڴ��ַ", "%p", target);
                    ImGui::LabelText("Id", "%d", target->agent_id);
                    ImGui::LabelText(u8"Z����", "%f", target->z);
                    ImGui::LabelText(u8"�߶�", "%f", target->width1);
                    ImGui::LabelText(u8"���", "%f", target->height1);
                    ImGui::LabelText(u8"�״��еĽǶ�", "%f", target->rotation_angle);
                    ImGui::LabelText("NameProperties", "0x%X", target->name_properties);
                    ImGui::LabelText(u8"X����", "%f", target->pos.x);
                    ImGui::LabelText(u8"Y����", "%f", target->pos.y);
                    if (me != nullptr) {
                        ImGui::LabelText(u8"����", "%.0f", GW::GetDistance(me->pos, target->pos));
                    }
                    ImGui::LabelText(u8"����", "%d", target->plane);
                    ImGui::LabelText(u8"����", "0x%X", target->type);
                    if (target_item) {
                        ImGui::LabelText(u8"���������", "%d", target_item->owner);
                        ImGui::LabelText(u8"���Id", "%d", target_item->item_id);
                        ImGui::LabelText(u8"����ĸ�������", "%d", target_item->extra_type);
                    }
                    if (target_gadget) {
                        ImGui::LabelText(u8"��װ�õ�ID", "%d", target_gadget->gadget_id);
                        ImGui::LabelText(u8"��װ�ö��������", "%d", target_gadget->extra_type);
                    }
                    if (target_living) {
                        ImGui::LabelText(u8"�����Ĺ����ٶ�", "%f", target_living->weapon_attack_speed);
                        ImGui::LabelText(u8"������Ĺ����ٶ�", "%f", target_living->attack_speed_modifier);
                        ImGui::LabelText(u8"��ҽ�ɫ��ID", "%d", target_living->player_number);
                        ImGui::LabelText(u8"�����ְҵ��ID", "%d", target_living->primary);
                        ImGui::LabelText(u8"��Ҹ�ְҵ��ID", "%d", target_living->secondary);
                        ImGui::LabelText(u8"����ȼ�", "%d", target_living->level);
                        ImGui::LabelText(u8"���ڶ����ID", "%d", target_living->team_id);
                        ImGui::LabelText(u8"�յ���ЧӦ", "0x%X", target_living->effects);
                        ImGui::LabelText(u8"ģ�͵�״̬", "0x%X", target_living->model_state);
                        ImGui::LabelText(u8"���ڵĵ�ͼ����", "0x%X", target_living->type_map);
                        ImGui::LabelText(u8"�˽�ɫ�����", "%d", target_living->login_number);
                        ImGui::LabelText(u8"��Ӫ", "%d", target_living->allegiance);
                        ImGui::LabelText(u8"���õ���������", "%d", target_living->weapon_type);
                        ImGui::LabelText(u8"����ʹ�õļ���ID", "%d", target_living->skill);
                    }
                    if (npc) {
                        ImGui::LabelText(u8"NPC��ID", "%d", target_living->player_number);
                        ImGui::LabelText(u8"NPC��ModelFileID", "%d", npc->model_file_id);
                        if(npc->files_count)
                            ImGui::LabelText(u8"NPC��ModelFile", "%d", npc->model_files[0]);
                        ImGui::LabelText(u8"NPC�Ĳ���", "%d", npc->npc_flags);
                    }
                    ImGui::PopItemWidth();
                    ImGui::TreePop();
                }
                if (player) {
                    if (ImGui::TreeNodeEx(u8"�˽�ɫ������##target", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth)) {
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2);
                        ImGui::LabelText(u8"�ڴ��ַ", "%p", player);
                        ImGui::LabelText(u8"����", "%s", GuiUtils::WStringToString(player->name).c_str());
                        ImGui::PopItemWidth();
                        ImGui::TreePop();
                    }
                }
                if (guild) {
                    if (ImGui::TreeNodeEx("�˹��������##target", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth)) {
                        ImGui::PushItemWidth(ImGui::GetContentRegionAvail().x / 2);
                        ImGui::LabelText(u8"�ڴ��ַ", "%p", guild);
                        ImGui::LabelText(u8"����", "%s [%s]", GuiUtils::WStringToString(guild->name).c_str(), GuiUtils::WStringToString(guild->tag).c_str());
                        ImGui::LabelText(u8"��Ӫ��������-", "%d (%s)", guild->faction_point, guild->faction ? u8"�տ�ɭ" : u8"���ȿ�");
                        if (ImGui::Button(u8"���͵��˹���")) {
                            GW::GuildMgr::TravelGH(guild->key);
                        }
                        ImGui::PopItemWidth();
                        ImGui::TreePop();
                    }
                }
            }
        }
        if (show_map && ImGui::CollapsingHeader(u8"���ڵ�ͼ�Ĳ���")) {
            static char id_buf[32] = "";
            char* type = "";
            static char file_buf[32] = "";
            static char region_buf[32] = "";
            static char district_buf[32] = "";
            snprintf(id_buf, 32, "%d", GW::Map::GetMapID());
            switch (GW::Map::GetInstanceType()) {
            case GW::Constants::InstanceType::Outpost: type = u8"����\0\0\0"; break;
            case GW::Constants::InstanceType::Explorable: type = u8"̽����"; break;
            case GW::Constants::InstanceType::Loading: type = u8"��ͼ��\0\0\0"; break;
            }
            snprintf(file_buf, 32, "%lu", mapfile);
            snprintf(region_buf, 32, "%d", GW::Map::GetRegion());
            snprintf(district_buf, 32, "%d", GW::Map::GetDistrict());
            ImGui::PushItemWidth(-80.0f);
            ImGui::InputText(u8"��ͼ ID", id_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::ShowHelp(u8"���������ڵĵ�ͼ�����ǵ�ID�������ǵĴ��ţ���û���ظ���");
            ImGui::InputText(u8"��ͼ ���ڵĹ�������", region_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"�˹����������· ", district_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"��ͼ������", type, 11, ImGuiInputTextFlags_ReadOnly);
            ImGui::InputText(u8"��ͼ���ļ������", file_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::ShowHelp(u8"��ͼ���ļ�������˼�ս���������ͼ.\n��ͼ��ֻ���������ͼ��һ���ֿ飬�ܶ�С��ͼ������ͬһ����ͼ����");
            ImGui::PopItemWidth();
        }
        if (show_dialog && ImGui::CollapsingHeader(u8"����Ի�")) {
            static char id_buf[32] = "";
            snprintf(id_buf, 32, "0x%X", GW::Agents::GetLastDialogId());
            ImGui::PushItemWidth(-80.0f);
            ImGui::InputText(u8"��һ���Ի���ID", id_buf, 32, ImGuiInputTextFlags_ReadOnly);
            ImGui::PopItemWidth();
            ImGui::Text(u8"��NPCû�п��õĶԻ�:");
            ImGui::ShowHelp(u8"����п��öԻ���NPC��̸.");
            ImGui::PushItemWidth(140.0f * ImGui::GetIO().FontGlobalScale);
            for (auto dialog : available_dialogs) {
                if (dialog->msg_s.empty() && !dialog->msg_ws.empty())
                    dialog->msg_s = GuiUtils::WStringToString(dialog->msg_ws);
                ImGui::InputText(dialog->msg_s.c_str(), dialog->dialog_buf, 11, ImGuiInputTextFlags_ReadOnly);
            }
            ImGui::PopItemWidth();
        }
        if (show_item && ImGui::CollapsingHeader(u8"��Ʒ������")) {
            ImGui::Text(u8"��Ʒ���еĵ�һ����Ʒ");
            static ForDecode item_name;
            DrawItemInfo(GW::Items::GetItemBySlot(GW::Constants::Bag::Backpack, 1),&item_name);
        }
        #ifdef _DEBUG
        if (show_item && ImGui::CollapsingHeader("Quoted Item")) {
            ImGui::Text("Most recently quoted item (buy or sell) from trader");
            static ForDecode quoted_name;
            DrawItemInfo(GW::Items::GetItemById(quoted_item_id),&quoted_name);
        }
        #endif
        if (show_quest && ImGui::CollapsingHeader(u8"̽��������")) {
            GW::QuestLog qlog = GW::GameContext::instance()->world->quest_log;
            DWORD qid = GW::GameContext::instance()->world->active_quest_id;
            if (qid && qlog.valid()) {
                for (unsigned int i = 0; i < qlog.size(); ++i) {
                    GW::Quest q = qlog[i];
                    if (q.quest_id == qid) {
                        ImGui::Text(u8"����ID: 0x%X", q.quest_id);
                        ImGui::Text(u8"������Ҫȥ������: (%.0f, %.0f)", q.marker.x, q.marker.y);
                        break;
                    }
                }
            }
        }
        if (show_mobcount && ImGui::CollapsingHeader(u8"�з�����ͳ��")) {
            const float sqr_soul_range = 1400.0f * 1400.0f;
            int soul_count = 0;
            int cast_count = 0;
            int spirit_count = 0;
            int compass_count = 0;
            GW::AgentArray agents = GW::Agents::GetAgentArray();
            GW::Agent* player = GW::Agents::GetPlayer();
            if (GW::Map::GetInstanceType() != GW::Constants::InstanceType::Loading
                && agents.valid()
                && player != nullptr) {

                for (unsigned int i = 0; i < agents.size(); ++i) {
                    if (agents[i] == nullptr) continue; // ignore nothings
                    GW::AgentLiving* agent = agents[i]->GetAsAgentLiving();
                    if (agent == nullptr) continue;
                    if (agent->allegiance != 0x3) continue; // ignore non-hostiles
                    if (agent->GetIsDead()) continue; // ignore dead 
                    float sqrd = GW::GetSquareDistance(player->pos, agent->pos);
                    if (agent->player_number == GW::Constants::ModelID::DoA::SoulTormentor
                        || agent->player_number == GW::Constants::ModelID::DoA::VeilSoulTormentor) {
                        if (GW::Map::GetMapID() == GW::Constants::MapID::Domain_of_Anguish
                            && sqrd < sqr_soul_range) {
                            ++soul_count;
                        }
                    }
                    if (sqrd < GW::Constants::SqrRange::Spellcast) ++cast_count;
                    if (sqrd < GW::Constants::SqrRange::Spirit) ++spirit_count;
                    ++compass_count;
                }
            }

            if (GW::Map::GetMapID() == GW::Constants::MapID::Domain_of_Anguish) {
                ImGui::Text("%d Soul Tormentors", soul_count);
                ImGui::ShowHelp("Only in Domain of Anguish, within 1400 range");
            }
            ImGui::Text(u8"ʩ����Χ�ڵĵ�������:%d", cast_count);
            ImGui::Text(u8"���鷶Χ�ڵĵ�������:%d", spirit_count);
            ImGui::Text(u8"�״ﷶΧ�ڵĵ�������:%d", compass_count);
        }
        if (show_resignlog && ImGui::CollapsingHeader(u8"˭û�����˳�/resign")) {
            DrawResignlog();
        }
    }
    ImGui::End();
}

void InfoWindow::Update(float delta) {
    UNREFERENCED_PARAMETER(delta);
    if (!send_queue.empty() && TIMER_DIFF(send_timer) > 600) {
        send_timer = TIMER_INIT();
        if (GW::Map::GetInstanceType() != GW::Constants::InstanceType::Loading
            && GW::Agents::GetPlayer()) {

            GW::Chat::SendChat('#', send_queue.front().c_str());
            send_queue.pop();
        }
    }

    if (show_resignlog
        && GW::Map::GetInstanceType() != GW::Constants::InstanceType::Loading
        && GW::PartyMgr::GetPartyInfo()) {
        GW::PlayerPartyMemberArray partymembers = GW::PartyMgr::GetPartyInfo()->players;
        if (partymembers.valid()) {
            if (partymembers.size() != status.size()) {
                status.resize(partymembers.size(), Unknown);
                timestamp.resize(partymembers.size(), 0);
            }
        }
        for (unsigned i = 0; i < partymembers.size(); ++i) {
            GW::PlayerPartyMember& partymember = partymembers[i];
            if (partymember.connected()) {
                if (status[i] == NotYetConnected || status[i] == Unknown) {
                    status[i] = Connected;
                    timestamp[i] = GW::Map::GetInstanceTime();
                }
            } else {
                if (status[i] == Connected || status[i] == Resigned) {
                    status[i] = Left;
                    timestamp[i] = GW::Map::GetInstanceTime();
                }
            }
        }
    }
}

const char* InfoWindow::GetStatusStr(Status status) {
    switch (status) {
    case InfoWindow::Unknown: return u8"�޷���֤-Unknown";
    case InfoWindow::NotYetConnected: return u8"û������-Not connected";
    case InfoWindow::Connected: return u8"��������-Connected";
    case InfoWindow::Resigned: return u8"�Ѵ��˳�-Resigned";
    case InfoWindow::Left: return u8"�뿪����-Left";
    default: return "";
    }
}

void InfoWindow::PrintResignStatus(wchar_t *buffer, size_t size, size_t index, const wchar_t *player_name) {
    ASSERT(index < status.size());
    Status player_status = status[index];
    const char* status_str = GetStatusStr(player_status);
    _snwprintf(buffer, size, L"%zu. %s - %S", index + 1, player_name,
        (player_status == Connected
            && GW::Map::GetInstanceType() == GW::Constants::InstanceType::Explorable)
        ? u8"��û���˳�!-Connected (not resigned)" : status_str);
}

void InfoWindow::DrawResignlog() {
    if (GW::Map::GetInstanceType() == GW::Constants::InstanceType::Loading) return;
    GW::PartyInfo* info = GW::PartyMgr::GetPartyInfo();
    if (info == nullptr) return;
    GW::PlayerPartyMemberArray partymembers = info->players;
    if (!partymembers.valid()) return;
    GW::PlayerArray players = GW::Agents::GetPlayerArray();
    if (!players.valid()) return;
    for (size_t i = 0; i < partymembers.size(); ++i) {
        GW::PlayerPartyMember& partymember = partymembers[i];
        if (partymember.login_number >= players.size()) continue;
        GW::Player& player = players[partymember.login_number];
        ImGui::PushID(static_cast<int>(i));
        if (ImGui::Button(u8"���͸�����")) {
            // Todo: wording probably needs improvement
            wchar_t buf[256];
            PrintResignStatus(buf, 256, i, player.name);
            GW::Chat::SendChat('#', buf);
        }
        ImGui::SameLine();
        const char* status_str = GetStatusStr(status[i]);
        ImGui::Text("%d. %S - %s", i + 1, player.name, status_str);
        if (status[i] != Unknown) {
            ImGui::SameLine();
            ImGui::TextDisabled("[%d:%02d:%02d.%03d]",
                (timestamp[i] / (60 * 60 * 1000)),
                (timestamp[i] / (60 * 1000)) % 60,
                (timestamp[i] / (1000)) % 60,
                (timestamp[i]) % 1000);
        }
        ImGui::PopID();
    }
}

void InfoWindow::DrawSettingInternal() {
    ImGui::Checkbox("Show widget toggles", &show_widgets);
    ImGui::Checkbox("Show 'Open Xunlai Chest' button", &show_open_chest);
    ImGui::Checkbox("Show Player", &show_player);
    ImGui::Checkbox("Show Target", &show_target);
    ImGui::Checkbox("Show Map", &show_map);
    ImGui::Checkbox("Show Dialog", &show_dialog);
    ImGui::Checkbox("Show Item", &show_item);
    ImGui::Checkbox("Show Quest", &show_quest);
    ImGui::Checkbox("Show Enemy Count", &show_mobcount);
    ImGui::Checkbox("Show Resign Log", &show_resignlog);
}

void InfoWindow::LoadSettings(CSimpleIni* ini) {
    ToolboxWindow::LoadSettings(ini);
    show_menubutton = ini->GetBoolValue(Name(), VAR_NAME(show_menubutton), true);

    show_widgets = ini->GetBoolValue(Name(), VAR_NAME(show_widgets), true);
    show_open_chest = ini->GetBoolValue(Name(), VAR_NAME(show_open_chest), true);
    show_player = ini->GetBoolValue(Name(), VAR_NAME(show_player), true);
    show_target = ini->GetBoolValue(Name(), VAR_NAME(show_target), true);
    show_map = ini->GetBoolValue(Name(), VAR_NAME(show_map), true);
    show_dialog = ini->GetBoolValue(Name(), VAR_NAME(show_dialog), true);
    show_item = ini->GetBoolValue(Name(), VAR_NAME(show_item), true);
    show_quest = ini->GetBoolValue(Name(), VAR_NAME(show_quest), true);
    show_mobcount = ini->GetBoolValue(Name(), VAR_NAME(show_enemycount), true);
    show_resignlog = ini->GetBoolValue(Name(), VAR_NAME(show_resignlog), true);
}

void InfoWindow::SaveSettings(CSimpleIni* ini) {
    ToolboxWindow::SaveSettings(ini);
    ini->SetBoolValue(Name(), VAR_NAME(show_widgets), show_widgets);
    ini->SetBoolValue(Name(), VAR_NAME(show_open_chest), show_open_chest);
    ini->SetBoolValue(Name(), VAR_NAME(show_player), show_player);
    ini->SetBoolValue(Name(), VAR_NAME(show_target), show_target);
    ini->SetBoolValue(Name(), VAR_NAME(show_map), show_map);
    ini->SetBoolValue(Name(), VAR_NAME(show_dialog), show_dialog);
    ini->SetBoolValue(Name(), VAR_NAME(show_item), show_item);
    ini->SetBoolValue(Name(), VAR_NAME(show_quest), show_quest);
    ini->SetBoolValue(Name(), VAR_NAME(show_enemycount), show_mobcount);
    ini->SetBoolValue(Name(), VAR_NAME(show_resignlog), show_resignlog);
}
