#include "stdafx.h"

#include <GWCA/Constants/Constants.h>
#include <GWCA/GameContainers/Array.h>

#include <GWCA/GameEntities/Map.h>

#include <GWCA/Managers/ChatMgr.h>
#include <GWCA/Managers/MapMgr.h>

#include <GuiUtils.h>
#include <GWToolbox.h>
#include <Logger.h>

#include <Windows/DailyQuestsWindow.h>
namespace{
static const char* vanguard_cycles[9] = {
    u8"�ݿ�",
    u8"������ ��������",
    u8"��˹��¡ ����",
    u8"������ ",
    u8"�����ħ ���˵�Ƭ",
    u8"ũ�� ��ķ����",
    u8"�Ķ�",
    u8"�������� �ȼ�",
    u8"�ʹ� ̩��"
};
static const char* nicholas_sandford_cycles[52] = {
    u8"Ѩ��������",
    u8"�ս����",
    u8"�����ֱ�",
    u8"�Źֵ�����",
    u8"��ħ��ʯ",
    u8"�۹���",
    u8"����ʯ",
    u8"ʯ���ͷ­",
    u8"�����ļ׿�",
    u8"�ս����",
    u8"��ɫ�β��",
    u8"֩����",
    u8"�����ֱ�",
    u8"�Ķ����Ʒ",
    u8"��ħ��ʯ",
    u8"Ѩ��������",
    u8"����ʯ",
    u8"�ƾɵ�����",
    u8"ʯ���ͷ­",
    u8"�Źֵ�����",
    u8"�۹���",
    u8"��ɫ�β��",
    u8"��ħ��ʯ",
    u8"�����ֱ�",
    u8"�Ķ����Ʒ",
    u8"֩����",
    u8"�ս����",
    u8"ʯ���ͷ­",
    u8"�Źֵ�����",
    u8"����ʯ",
    u8"Ѩ��������",
    u8"��ħ��ʯ",
    u8"�ƾɵ�����",
    u8"�����ļ׿�",
    u8"֩����",
    u8"ʯ���ͷ­",
    u8"����ʯ",
    u8"�Źֵ�����",
    u8"�ƾɵ�����",
    u8"Ѩ��������",
    u8"�ս����",
    u8"�����ֱ�",
    u8"��ɫ�β��",
    u8"�Ķ����Ʒ",
    u8"�۹���",
    u8"�����ļ׿�",
    u8"��ħ��ʯ",
    u8"�Ķ����Ʒ",
    u8"֩����",
    u8"��ɫ�β��",
    u8"�ƾɵ�����",
    u8"�����ļ׿�"
};
static const char* nicholas_region_cycles[137] = {
    u8"��˹��¡",
    u8"��ϯ��ɽ��",
    u8"����֮��",
    u8"�Ұ��ذ�",
    u8"������",
    u8"��ϯ��ɽ��",
    u8"���",
    u8"���е�",
    u8"��ᵵ�",
    u8"��˹̹",
    u8"�Ķ���԰",
    u8"������",
    u8"����",
    u8"÷�������",
    u8"��˹��¡",
    u8"����֮��",
    u8"������",
    u8"����",
    u8"ϯ��ɽ������",
    u8"�Ķ���԰",
    u8"��ᵵ�",
    u8"�߱�",
    u8"������",
    u8"��ϯ��ɽ��",
    u8"��˹��¡",
    u8"����",
    u8"ˮ��ɳĮ",
    u8"ϯ��ɽ������",
    u8"��ϯ��ɽ��",
    u8"���",
    u8"÷�������",
    u8"�߱�",
    u8"�Ʒ���ɭ��",
    u8"������",
    u8"�߱�",
    u8"��ᵵ�",
    u8"������",
    u8"ˮ��ɳĮ",
    u8"��ϯ��ɽ��",
    u8"��˹̹",
    u8"�Ʒ���ɭ��",
    u8"�Ұ��ذ�",
    u8"ˮ��ɳĮ",
    u8"��ϯ��ɽ��",
    u8"������",
    u8"��ϯ��ɽ��",
    u8"��˹̹",
    u8"�Ʒ���ɭ��",
    u8"������",
    u8"�Ұ��ذ�",
    u8"�߱�",
    u8"����֮��",
    u8"��ϯ��ɽ��",
    u8"������",
    u8"���",
    u8"ˮ��ɳĮ",
    u8"��ϯ��ɽ��",
    u8"����",
    u8"������",
    u8"ϯ��ɽ������",
    u8"��ᵵ�",
    u8"��ϯ��ɽ��",
    u8"��˹��¡",
    u8"�߱�",
    u8"������",
    u8"������",
    u8"�Ʒ���ɭ��",
    u8"ϯ��ɽ������",
    u8"÷�������",
    u8"��˹̹",
    u8"������",
    u8"������",
    u8"ˮ��ɳĮ",
    u8"÷�������",
    u8"�Ʒ���ɭ��",
    u8"��˹̹",
    u8"�߱�",
    u8"�Ұ��ذ�",
    u8"ˮ��ɳĮ",
    u8"������",
    u8"��ϯ��ɽ��",
    u8"����֮��",
    u8"������",
    u8"�߱�",
    u8"ˮ��ɳĮ",
    u8"��ᵵ�",
    u8"÷�������",
    u8"��ϯ��ɽ��",
    u8"���",
    u8"�߱�",
    u8"÷�������",
    u8"��ᵵ�",
    u8"��ϯ��ɽ��",
    u8"����֮��",
    u8"����",
    u8"���",
    u8"������",
    u8"������",
    u8"�Ʒ���ɭ��",
    u8"��˹��¡",
    u8"����֮��",
    u8"����",
    u8"���",
    u8"�Ұ��ذ�",
    u8"��ϯ��ɽ��",
    u8"÷�������",
    u8"��ᵵ�",
    u8"��˹̹",
    u8"������",
    u8"�߱�",
    u8"��˹��¡",
    u8"����",
    u8"ϯ��ɽ������",
    u8"���",
    u8"��ϯ��ɽ��",
    u8"������",
    u8"���",
    u8"��˹��¡",
    u8"�Ʒ���ɭ��",
    u8"��ϯ��ɽ��",
    u8"�Ұ��ذ�",
    u8"����֮��",
    u8"��ᵵ�",
    u8"����֮��",
    u8"������",
    u8"������",
    u8"�߱�",
    u8"�Ķ���԰",
    u8"��˹��¡",
    u8"����",
    u8"������",
    u8"���",
    u8"÷�������",
    u8"��˹̹",
    u8"ϯ��ɽ������",
    u8"��ϯ��ɽ��",
    u8"��˹��¡"
};
static const uint32_t nicholas_quantity_cycles[137] = {
    3,
    3,
    2,
    2,
    3,
    2,
    1,
    1,
    5,
    3,
    2,
    1,
    3,
    1,
    1,
    1,
    2,
    3,
    3,
    3,
    5,
    2,
    3,
    3,
    3,
    2,
    1,
    1,
    3,
    1,
    3,
    1,
    1,
    3,
    1,
    1,
    3,
    2,
    2,
    2,
    1,
    3,
    2,
    3,
    3,
    1,
    1,
    3,
    2,
    5,
    2,
    1,
    3,
    3,
    1,
    2,
    2,
    1,
    2,
    2,
    3,
    3,
    1,
    3,
    3,
    1,
    1,
    3,
    1,
    1,
    2,
    3,
    2,
    2,
    1,
    3,
    1,
    2,
    1,
    2,
    2,
    1,
    2,
    1,
    2,
    5,
    3,
    2,
    5,
    2,
    1,
    5,
    3,
    1,
    3,
    3,
    3,
    3,
    5,
    3,
    1,
    1,
    6,
    2,
    2,
    2,
    3,
    1,
    3,
    1,
    3,
    1,
    1,
    2,
    5,
    3,
    3,
    3,
    3,
    3,
    3,
    2,
    3,
    3,
    2,
    3,
    1,
    2,
    3,
    2,
    3,
    2,
    2,
    3,
    3,
    3,
    3
};
static const char* nicholas_item_cycles[137] = {
    u8"��ɫ�β��",
    u8"���ȿ���ͷƤ��ë",
    u8"��������",
    u8"��β��ë",
    u8"���߷���",
    u8"տ��������",
    u8"�����޸����",
    u8"�����צ",
    u8"�����",
    u8"֪������",
    u8"ʯѨ��������",
    u8"����",
    u8"������Ƥ",
    u8"�ڰ�����",
    u8"�Ķ�Ƥ",
    u8"�챦ʯ���龫��",
    u8"��̵ļ׿�",
    u8"���ƻ���",
    u8"Ī������ë",
    u8"�����Ķ����Ʒ",
    u8"��Ƥֽ��",
    u8"���������֮צ",
    u8"����֦",
    u8"ϯ��ɽ����ë",
    u8"����ļ׿�",
    u8"�±�",
    u8"�ִ�����",
    u8"Ⱦɫ����Ƭ",
    u8"�����ؼǺ�",
    u8"֪����ʯ",
    u8"���־�ħ���",
    u8"����ʯ���龫��",
    u8"ʯ��Ʒ",
    u8"��������ëͷƤ",
    u8"�Ӷ�Ļ�Ʒ",
    u8"��­���",
    u8"�������",
    u8"ţͷ�ֽ�",
    u8"�����ʨ���޳��",
    u8"������",
    u8"��¶",
    u8"˾��צ",
    u8"�������ͷ��צ",
    u8"����������",
    u8"��������",
    u8"��Ӱ������",
    u8"��������",
    u8"����",
    u8"���ȵ�����",
    u8"�����ͷ",
    u8"����Ƥ��",
    u8"������ʯͷ",
    u8"��ϸ��Ѩ��������",
    u8"Ӳ��",
    u8"��ű�",
    u8"�ɿݵ��۾�",
    u8"����ʯ",
    u8"����ϸ��צ",
    u8"Ӳ��",
    u8"Ԫ��֮��",
    u8"����Ƥ",
    u8"����֮��",
    u8"����֮��",
    u8"�������",
    u8"���ʯ",
    u8"��̵ľ���",
    u8"����",
    u8"��սʿ�Ľ�",
    u8"�����",
    u8"�۹�����",
    u8"ɭ��ţͷ�ֵĽ�",
    u8"���ܰ���",
    u8"�������",
    u8"÷������ë",
    u8"ͷ������",
    u8"�����޸�",
    u8"�߱����",
    u8"��֯�ߵ���",
    u8"�Ʊ�ʯ����",
    u8"���������",
    u8"˪�����",
    u8"��ħ��Ƭ",
    u8"���ε�����",
    u8"��ʯ���龫��",
    u8"������ͼ��",
    u8"��­ͭ��",
    u8"��̦����",
    u8"ū��ʯ",
    u8"������Ƥ��",
    u8"����֮צ",
    u8"÷����֩��˿",
    u8"��������С����",
    u8"��ѩ��",
    u8"��ɳ��ʯ",
    u8"������",
    u8"�׳��",
    u8"ʯ��ħͷ­",
    u8"���õ�ŷ������",
    u8"���Ϻ�Ĳ���ƿ",
    u8"�ս��Ĵ�ʯ",
    u8"ˮ���龫��",
    u8"�ػ���̦",
    u8"����ơ��",
    u8"˫���˵���ͷ",
    u8"��ľ����",
    u8"���������",
    u8"ʧ�ԵĲ���Ʒ",
    u8"������ɳ��",
    u8"������Ƥ��",
    u8"���������֮��",
    u8"ǿ�͵�צ",
    u8"տ�����",
    u8"�Ƕ�Ƥë",
    u8"��������",
    u8"ɽ��ħ���",
    u8"Сƿīˮ",
    u8"��骴���",
    u8"�ս���ʯ���ͷ­",
    u8"�ھ���֮��",
    u8"ʯ���־",
    u8"����Ƥ",
    u8"������Ƭ",
    u8"��ͷƤ",
    u8"ľ������ʬ��",
    u8"��Ӱ������",
    u8"�ź�ͯ��",
    u8"��ʯ",
    u8"��ά�����޸�",
    u8"���µ���ǻ",
    u8"����֮��",
    u8"���۹ֵ���",
    u8"֪������",
    u8"Զ��֮��",
    u8"ͭ����",
    u8"�������������",
    u8"���鲼",
    u8"�Ķ����Ʒ"
};
static const char* nicholas_location_cycles[137] = {
    u8"ͳ����֮��",
    u8"����ȪԴ",
    u8"�綾�ر�",
    u8"����֮��������",
    u8"�߽��г�",
    u8"�޾���֮��",
    u8"�����Ƽ����",
    u8"����ʯ",
    u8"����С��",
    u8"�ɳǣ�����Ŭ��",
    u8"ʥŵ��",
    u8"˫ͷ�ߺ���",
    u8"��ɽ",
    u8"������",
    u8"ȱ��",
    u8"�����F¯",
    u8"���߰�Ϣ��",
    u8"ϯ�ǻ�ɽ��",
    u8"ά������",
    u8"������ɽ��",
    u8"��̨��",
    u8"ϣ������԰",
    u8"�͵¿�С��",
    u8"������Ħ���",
    u8"�����߾�",
    u8"����������",
    u8"�ݽ�֮��",
    u8"�����ѿ�",
    u8"����",
    u8"�Ͷ�˶�Ѩ",
    u8"����֮��",
    u8"�ݻ�֮�ǣ�Ī��",
    u8"ͤʯ",
    u8"����������",
    u8"�ۿ��������",
    u8"����к��",
    u8"̷�ɿ����",
    u8"��֪֮·",
    u8"����",
    u8"÷̹��֮Կ",
    u8"Ħ������ͨ��",
    u8"ԭҰ�ٲ�",
    u8"����",
    u8"ǹ��ɽ",
    u8"������",
    u8"����˹ͨ��",
    u8"��˹ŵ��",
    u8"ާ��",
    u8"������",
    u8"˺�Ѵ��",
    u8"������Ұ",
    u8"ˮ���ߵ�",
    u8"�������Ĺ��콨��",
    u8"����ͨ��",
    u8"Ұ���Ӱ�",
    u8"�������",
    u8"����",
    u8"�ž�֮��",
    u8"�Ხɽ��",
    u8"׿����",
    u8"���ð뵺",
    u8"ʨ���ް���",
    u8"���ƽԭ",
    u8"�ߵع㳡",
    u8"���ջʹ�",
    u8"ս����֮��",
    u8"��������ľ��",
    u8"�Ǽ� Ħ����",
    u8"��ūк��",
    u8"��¡����",
    u8"��ϯ˹ɽ",
    u8"�ｭ������",
    u8"��̲",
    u8"����",
    u8"����֮��",
    u8"���������",
    u8"Ρ���Ͽ��",
    u8"���˹֮ʯ",
    u8"Ԥ����֮��",
    u8"������",
    u8"����ʯ",
    u8"����֮��",
    u8"ͻ��˻ĵ�",
    u8"����֮�ǣ�������",
    u8"ͺӥɳ��",
    u8"�������",
    u8"˫ͷ�������",
    u8"����˹���㼣",
    u8"�ĺ��ͱ�",
    u8"������",
    u8"«έ�����",
    u8"�ܴ�ׯ԰",
    u8"�����ɽ",
    u8"���ѹ���",
    u8"Ⱥ��",
    u8"��Ӻ���",
    u8"�t��������",
    u8"����֮��",
    u8"����֮Ĺ�ٲ�",
    u8"�ɰ�˹��¡",
    u8"���׳���",
    u8"����ɽ��",
    u8"����¡����ƽԭ",
    u8"����Ӭ����",
    u8"����ɭ��",
    u8"����ߵ�",
    u8"��Ұ����",
    u8"����ƽԭ",
    u8"�İ����",
    u8"��֮����",
    u8"��˹��¡����",
    u8"��˯֮ˮ",
    u8"�����߾�",
    u8"�����ׯ԰",
    u8"��ʯ�µ�",
    u8"����ͨ��",
    u8"����Ҫ�����ɴ���",
    u8"��Үɳ�͵�",
    u8"÷���������",
    u8"��˹��֮��",
    u8"������",
    u8"�ܸ����",
    u8"ɣȪ��",
    u8"��ǻĵ�",
    u8"��ɫ�Ļ",
    u8"���³�",
    u8"�����Ͽ��",
    u8"��˹��ս�������",
    u8"����Ͽ��",
    u8"���񺣴�",
    u8"��ɽ��",
    u8"�����߾�",
    u8"�������",
    u8"��������",
    u8"ŵ��֮�����",
    u8"���˹�",
    u8"�Ķ��������"
};
static const char* zaishen_bounty_cycles[DailyQuests::zb_cnt] = {
    u8"ɳ֮��ʦ ׿��ķ",
    u8"Ұ�޿���������",
    u8"����˫ͷ����",
    u8"����ţͷ��",
    u8"��ֺ ������",
    u8"����",
    u8"����",
    u8"ϣά��ķ",
    u8"Ī�� ���",
    u8"��������",
    u8"�������",
    u8"���� �䰵",
    u8"��ɱ�� ��Ѧ��",
    u8"��֮Ů ��ϣ",
    u8"���� ߱����",
    u8"���� ��ά",
    u8"����",
    u8"�޴�Ӱ",
    u8"TPS���ڸ���",
    u8"��֮�ƻ���",
    u8"��Ӱ(����֮Ĺ)",
    u8"�Ͻ� ����",
    u8"��� �ظ�˹",
    u8"��ұά��",
    u8"ŭ�� ����",
    u8"������ ׿��",
    u8"���� ʳ����",
    u8"�õ��� ��",
    u8"���� �ֶ�˹",
    u8"�ظ� �ɷ�˹ͼ",
    u8"��������",
    u8"���Ĭ˹",
    u8"��֮�� ħ��",
    u8"�п� ������",
    u8"��ɫ �ƿ�",
    u8"ͨ���� ����",
    u8"ڤ������",
    u8"�� ҫ����ŷ",
    u8"��ɫħ�� ������",
    u8"��������",
    u8"��ķ����ʿ",
    u8"����Ķ�Ƭ",
    u8"���֮�� �ǲ�",
    u8"�������",
    u8"�����ⱴ",
    u8"ǿ�� �ݵ�",
    u8"ޱ�ȵ�ף�� ɣ˹",
    u8"˾���� ������",
    u8"��ʯ����/����",
    u8"Ѹ�� ��ά��",
    u8"�����",
    u8"ħ���� ������",
    u8"��ϯ˹�ļ�˾",
    u8"��֮�� ��ɥ��",
    u8"�²����� ʳ����",
    u8"ָ�ӹ� ����",
    u8"������",
    u8"����˹",
    u8"ʯβ Ħ���з�",
    u8"ڤ������",
    u8"����֮��",
    u8"���� ��̾",
    u8"��֮���� ����",
    u8"ҹ֮Ů Ļ����",
    u8"֯���� ӲƤ",
    u8"���鷨ʦ ܽ���"
};
static const char* zaishen_combat_cycles[DailyQuests::zc_cnt] = {
    u8"����",
    u8"Codex ������",
    u8"Ӣ��֮·",
    u8"����ս",
    u8"ͬ��ս",
    u8"Ӣ��֮·",
    u8"����ս",
    u8"Codex ������",
    u8"��ľҪ��",
    u8"����",
    u8"���������",
    u8"Codex ������",
    u8"����ս",
    u8"����",
    u8"ͬ��ս",
    u8"Ӣ��֮·",
    u8"���������",
    u8"��ľҪ��",
    u8"����",
    u8"���������",
    u8"��ľҪ��",
    u8"Ӣ��֮·",
    u8"ͬ��ս",
    u8"����ս",
    u8"Codex ������",
    u8"���������",
    u8"��ľҪ��",
    u8"ͬ��ս"
};
static const char* zaishen_mission_cycles[DailyQuests::zm_cnt] = {
    u8"ռ��֮ʯ",
    u8"ϣ���˴�͢",
    u8"���˱���",
    u8"���ջʹ�",
    u8"����֮�����",
    u8"�ɿݺ���",
    u8"�ڳ�֮Ѩ",
    u8"�Կ��Ķ�",
    u8"��������",
    u8"�ɶ���",
    u8"��ʥ����",
    u8"��̨��",
    u8"������վ",
    u8"̷�ɿ����",
    u8"��������",
    u8"��ɳ����",
    u8"��ԭ",
    u8"��˯֮ˮ",
    u8"�ı��˴�ׯ",
    u8"ŷ�����ֵ�",
    u8"Ӣ�۵Ĺ�����",
    u8"���¹���ͷ",
    u8"��Ⱥ��",
    u8"������",
    u8"��Ѩ",
    u8"����ŵ�Ɽ",
    u8"����������",
    u8"Ϯ�����ݵ�",
    u8"����֮��",
    u8"��ʥɳ̲",
    u8"��ŷ������Ӫ",
    u8"ս������֮��",
    u8"����˹ͨ��",
    u8"�۹�ʥ��",
    u8"Ħ����ѷ�",
    u8"������ѧԺ",
    u8"�ƻ�����Ԩ",
    u8"����Ĺ��",
    u8"�����Ҫ��",
    u8"һ����Զ����",
    u8"�ܴ�ׯ԰",
    u8"����Ҫ��",
    u8"���ǿ��ֵ�",
    u8"Ѱ��Ѫʯ",
    u8"����ɳ��",
    u8"ޱ��㳡",
    u8"׿��ŵ�ھ��",
    u8"Ī���ǿ�ɽ",
    u8"�Ƶ�ŵ·��",
    u8"��Ųٿض�������ḻ���˵ı���",
    u8"ͤʯ",
    u8"����������",
    u8"���֮��",
    u8"������ʵ����",
    u8"���ϵش�",
    u8"���񺣴�",
    u8"Ħ������",
    u8"��������",
    u8"�����Ƿ���",
    u8"ŵ���ܵ�����",
    u8"�ｭ������",
    u8"����������",
    u8"�ͷ�֮��",
    u8"ѪծѪ��",
    u8"Ѫʯ����",
    u8"����Ⱥ��",
    u8"����������",
    u8"�ǰԶ�֮��",
    u8"��˪֮��"
};
static const char* zaishen_vanquish_cycles[DailyQuests::zv_cnt] = {
    u8"��Ұ����",
    u8"�ۿ��������",
    u8"�����ѿ�",
    u8"ʨ���ް���",
    u8"�������",
    u8"��˹ŵ��",
    u8"�Ǽ� Ħ����",
    u8"����",
    u8"����ɽ��",
    u8"�ĺ��ͱ�",
    u8"˺�Ѵ��",
    u8"����",
    u8"�ܴ�ׯ԰",
    u8"÷̹��֮Կ",
    u8"ʥŵ��",
    u8"�����ɽ",
    u8"Ħ������ͨ��",
    u8"����ƽԭ",
    u8"����Ӭ����",
    u8"��ϯ˹ɽ",
    u8"����֮Ĺ�ٲ�",
    u8"�����F¯",
    u8"ά������",
    u8"����˹ͨ��",
    u8"������",
    u8"����¡����ƽԭ",
    u8"ԭҰ�ٲ�",
    u8"���߰�Ϣ��",
    u8"���ջʹ�",
    u8"����֮�ǣ�������",
    u8"ϯ�ǻ�ɽ��",
    u8"��ūк��",
    u8"����ͨ��",
    u8"��֮����",
    u8"����С��",
    u8"�Ხɽ��",
    u8"����ͨ��",
    u8"����֮��",
    u8"��̲",
    u8"����������",
    u8"�ž�֮��",
    u8"���ѹ���",
    u8"��ɽ��",
    u8"�ɰ�˹��¡",
    u8"�ｭ������",
    u8"��ǻĵ�",
    u8"���˹֮ʯ",
    u8"����ʯ",
    u8"ɣȪ��",
    u8"���׳���",
    u8"ŵ��֮�����",
    u8"���ƽԭ",
    u8"̷�ɿ����",
    u8"������",
    u8"�綾�ر�",
    u8"��֪֮·",
    u8"����֮��",
    u8"��˹��֮��",
    u8"�ݻ�֮�ǣ�Ī��",
    u8"«έ�����",
    u8"��˯֮ˮ",
    u8"������",
    u8"�����߾�",
    u8"ͳ����֮��",
    u8"�߽��г�",
    u8"�����Ͽ��",
    u8"˫ͷ�ߺ���",
    u8"�������",
    u8"�İ����",
    u8"��¡����",
    u8"����֮��",
    u8"����",
    u8"��̨��",
    u8"�ݽ�֮��",
    u8"������",
    u8"�������",
    u8"����",
    u8"��ɫ�Ļ",
    u8"���ð뵺",
    u8"����",
    u8"���˹�",
    u8"ȱ��",
    u8"���������",
    u8"ǹ��ɽ",
    u8"��ɽ",
    u8"��Ӻ���",
    u8"÷���������",
    u8"������",
    u8"�ܸ����",
    u8"ͺӥɳ��",
    u8"������Ұ",
    u8"ͻ��˻ĵ�",
    u8"Ρ���Ͽ��",
    u8"��ʯ�µ�",
    u8"����ȪԴ",
    u8"����ʯ",
    u8"ͤʯ",
    u8"�������Ĺ��콨��",
    u8"�����Ƽ����",
    u8"��˹��¡����",
    u8"�Ͷ�˶�Ѩ",
    u8"����֮��",
    u8"����֮��������",
    u8"Ⱥ��",
    u8"�����߾�",
    u8"�����ׯ԰",
    u8"�t��������",
    u8"������",
    u8"Ұ���Ӱ�",
    u8"������Ħ���",
    u8"���񺣴�",
    u8"��������",
    u8"��Үɳ�͵�",
    u8"�͵¿�С��",
    u8"�����߾�",
    u8"ˮ���ߵ�",
    u8"Ԥ����֮��",
    u8"������ɽ��",
    u8"��������ľ��",
    u8"�ɳǣ�����Ŭ��",
    u8"����Ͽ��",
    u8"ާ��",
    u8"�ߵع㳡",
    u8"�޾���֮��",
    u8"׿����",
    u8"����ߵ�",
    u8"ս����֮��",
    u8"����������",
    u8"˫ͷ�������",
    u8"����Ҫ�����ɴ���",
    u8"��˹��ս�������",
    u8"�Ķ��������",
    u8"����к��",
    u8"����ɭ��",
    u8"ϣ������԰",
    u8"����˹���㼣"
};
static const char* wanted_by_shining_blade_cycles[DailyQuests::ws_cnt] = {
    u8"�й� ���",
    u8"��ƣ���ߵ�����",
    u8"�й� ϯ��",
    u8"ǿ�����߿�",
    u8"�п��߰�Ī�п�",
    u8"�����߿��ɿ�",
    u8"��Ⱶ�����˹",
    u8"ɪ��˿",
    u8"ȾѪ֮��������",
    u8"��ʵ׷Ѱ�ߴ�˹��",
    u8"ֱ����ɪ����˿",
    u8"�й� ������",
    u8"ϸ�ĵ���ϣ����",
    u8"���� ��",
    u8"����߰�ϣ��˹",
    u8"��������˹",
    u8"�����ĸ���˹",
    u8"���������ܽ",
    u8"�й� ��¡",
    u8"�й� ��ɺ׿��",
    u8"������ ޱ˿"
};
static const char* pve_weekly_bonus_cycles[DailyQuests::wbe_cnt] = {
    u8"����������",
    u8"֧Ԯ�����ǽ�",
    u8"ս�����ͽ�",
    u8"���¾�Ӣ��",
    u8"����֧Ԯ��",
    u8"ս�����߽�",
    u8"��",
    u8"����֧Ԯ��",
    u8"ս����ͼ��"
};
static const char* pve_weekly_bonus_descriptions[9] = {
    u8"���������������˼������˷֣����ı�������Կ�׺Ϳ������ߵ���",
    u8"��������������ꪺ͹�����",
    u8"ս����������Ʒ(ͭս�б�)����",
    u8"�ɴӿ����ǽ�����Ԩ�Ͷ���",
    u8"�����������İ�����, ������Ħ, ��̴�ȷ��, ��ŵ����",
    u8"ս����������Ʒ(ͭս�б�)����",
    u8"����ѽ�����º�����",
    u8"�Ͻ�����ʱ�������������Ŀ��ȿº��տ�ɭ��",
    u8"ս����ͼ����Ʒ(ͭս�б�)����"
};
static const char* pvp_weekly_bonus_cycles[DailyQuests::wbp_cnt] = {
    u8"���������",
    u8"����ս",
    u8"���¾���",
    u8"Ӣ��֮·",
    u8"Codex ������",
    u8"ͬ��ս"
};
static const char* pvp_weekly_bonus_descriptions[6] = {
    u8"����������� �� ���������� ������ �� ��ʿ ��",
    u8"����ս�ڻ� ���������� ������ �� �ھ� ��",
    u8"���� �� ��ľҪ���� �� ���������� ������ �� �۹� ��",
    u8"Ӣ��֮·�� �� ���������� ������ �� Ӣ�۳ƺ� ��",
    u8"Codex�������� �� ���������� ������ �� Codex�ƺ� ��",
    u8"ͬ��ս�� �� ���������� ������ �� �۹� ��"
};

bool subscriptions_changed = false;
bool checked_subscriptions = false;
time_t start_time;

const wchar_t* DateString(time_t* unix) {
    std::tm* now = std::localtime(unix);
    static wchar_t buf[12];
    swprintf(buf, sizeof(buf), L"%d-%02d-%02d", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday);
    return buf;
}
uint32_t GetZaishenBounty(time_t* unix) {
    return static_cast<uint32_t>((*unix - 1244736000) / 86400 % 66);
}
uint32_t GetWeeklyBonusPvE(time_t* unix) {
    return static_cast<uint32_t>((*unix - 1368457200) / 604800 % 9);
}
uint32_t GetWeeklyBonusPvP(time_t* unix) {
    return static_cast<uint32_t>((*unix - 1368457200) / 604800 % 6);
}
uint32_t GetZaishenCombat(time_t* unix) {
    return static_cast<uint32_t>((*unix - 1256227200) / 86400 % 28);
}
uint32_t GetZaishenMission(time_t* unix) {
    return static_cast<uint32_t>((*unix - 1299168000) / 86400 % 69);
}
uint32_t GetZaishenVanquish(time_t* unix) {
    return static_cast<uint32_t>((*unix - 1299168000) / 86400 % 136);
}
void PrintDaily(const wchar_t* label, const char* value, time_t unix, bool as_wiki_link = true) {
    bool show_date = unix != time(nullptr);
    wchar_t buf[139];
    if (show_date) {
        swprintf(buf, _countof(buf), as_wiki_link ? L"%s, %s: <a=1>\x200B%S</a>" : L"%s, %s: <a=1>%S</a>", label, DateString(&unix), value);
    }
    else {
        swprintf(buf, _countof(buf), as_wiki_link ? L"%s: <a=1>\x200B%S</a>" : L"%s: %S", label, value);
    }
    GW::Chat::WriteChat(GW::Chat::Channel::CHANNEL_GLOBAL, buf);
}
}

// Find the "week start" for this timestamp.
time_t GetWeeklyRotationTime(time_t* unix) {
    return (time_t)(floor((*unix - 1368457200) / 604800) * 604800) + 1368457200;
}
time_t GetNextWeeklyRotationTime() {
    time_t unix = time(nullptr);
    return GetWeeklyRotationTime(&unix) + 604800;
}
const char* GetNicholasSandfordLocation(time_t* unix) {
    uint32_t cycle_index = static_cast<uint32_t>((*unix - 1239260400) / 86400 % 52);
    return nicholas_sandford_cycles[cycle_index];
}
uint32_t GetNicholasItemQuantity(time_t* unix) {
    uint32_t cycle_index = static_cast<uint32_t>((*unix - 1323097200) / 604800 % 137);
    return nicholas_quantity_cycles[cycle_index];
}
const char* GetNicholasLocation(time_t* unix) {
    uint32_t cycle_index = static_cast<uint32_t>((*unix - 1323097200) / 604800 % 137);
    return nicholas_location_cycles[cycle_index];
}
const char* GetNicholasItemName(time_t* unix) {
    uint32_t cycle_index = static_cast<uint32_t>((*unix - 1323097200) / 604800 % 137);
    return nicholas_item_cycles[cycle_index];
}
uint32_t GetWantedByShiningBlade(time_t* unix) {
    return static_cast<uint32_t>((*unix - 1276012800) / 86400 % 21);
}
const char* GetVanguardQuest(time_t* unix) {
    uint32_t cycle_index = static_cast<uint32_t>((*unix - 1299168000) / 86400 % 9);
    return vanguard_cycles[cycle_index];
}
bool GetIsPreSearing() {
    GW::AreaInfo* i = GW::Map::GetCurrentMapInfo();
    return i && i->region == GW::Region::Region_Presearing;
}
void DailyQuests::Draw(IDirect3DDevice9*) {
    if (!visible) return;

    ImGui::SetNextWindowCenter(ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(300, 250), ImGuiCond_FirstUseEver);
    if (!ImGui::Begin(Name(), GetVisiblePtr(), GetWinFlags()))
        return ImGui::End();
    float offset = 0.0f;
    const float short_text_width = 120.0f * ImGui::GetIO().FontGlobalScale;
    const float long_text_width = text_width * ImGui::GetIO().FontGlobalScale;
    const float zm_width = 170.0f * ImGui::GetIO().FontGlobalScale;
    const float zb_width = 185.0f * ImGui::GetIO().FontGlobalScale;
    const float zc_width = 135.0f * ImGui::GetIO().FontGlobalScale;
    const float zv_width = 200.0f * ImGui::GetIO().FontGlobalScale;
    const float ws_width = 180.0f * ImGui::GetIO().FontGlobalScale;
    const float nicholas_width = 180.0f * ImGui::GetIO().FontGlobalScale;
    const float wbe_width = 145.0f * ImGui::GetIO().FontGlobalScale;

    ImGui::Text(u8"����");
    ImGui::SameLine(offset += short_text_width);
    if (show_zaishen_missions_in_window) {
        ImGui::Text(u8"ս������");
        ImGui::SameLine(offset += zm_width);
    }
    if (show_zaishen_bounty_in_window) {
        ImGui::Text(u8"ս������");
        ImGui::SameLine(offset += zb_width);
    }
    if (show_zaishen_combat_in_window) {
        ImGui::Text(u8"ս�ж�ս");
        ImGui::SameLine(offset += zc_width);
    }
    if (show_zaishen_vanquishes_in_window) {
        ImGui::Text(u8"ս����ͼ");
        ImGui::SameLine(offset += zv_width);
    }
    if (show_wanted_quests_in_window) {
        ImGui::Text(u8"����ͨ��");
        ImGui::SameLine(offset += ws_width);
    }
    if (show_nicholas_in_window) {
        ImGui::Text(u8"����");
        ImGui::SameLine(offset += nicholas_width);
    }
    if (show_weekly_bonus_pve_in_window) {
        ImGui::Text(u8"ÿ�ܽ��� (��Ϸ����)");
        ImGui::SameLine(offset += wbe_width);
    }
    if (show_weekly_bonus_pvp_in_window) {
        ImGui::Text(u8"ÿ�ܽ��� (��������)");
        ImGui::SameLine(offset += long_text_width);
    }
    ImGui::NewLine();
    ImGui::Separator();
    ImGui::BeginChild("dailies_scroll", ImVec2(0, (-1 * (20.0f * ImGui::GetIO().FontGlobalScale)) - ImGui::GetStyle().ItemInnerSpacing.y));
    time_t unix = time(nullptr);
    uint32_t idx = 0;
    ImColor sCol(102, 187, 238, 255);
    ImColor wCol(255, 255, 255, 255);
    for (size_t i = 0; i < (size_t)daily_quest_window_count; i++) {
        offset = 0.0f;
        switch (i) {
        case 0:
            ImGui::Text(u8"����");
            break;
        case 1:
            ImGui::Text(u8"����");
            break;
        default:
            char mbstr[100];
            std::strftime(mbstr, sizeof(mbstr), "%a %d %b", std::localtime(&unix));
            ImGui::Text(mbstr);
            break;
        }

        ImGui::SameLine(offset += short_text_width);
        if (show_zaishen_missions_in_window) {
            idx = GetZaishenMission(&unix);
            ImGui::TextColored(subscribed_zaishen_missions[idx] ? sCol : wCol, zaishen_mission_cycles[idx]);
            if (ImGui::IsItemClicked())
                subscribed_zaishen_missions[idx] = !subscribed_zaishen_missions[idx];
            ImGui::SameLine(offset += zm_width);
        }
        if (show_zaishen_bounty_in_window) {
            idx = GetZaishenBounty(&unix);
            ImGui::TextColored(subscribed_zaishen_bounties[idx] ? sCol : wCol, zaishen_bounty_cycles[idx]);
            if (ImGui::IsItemClicked())
                subscribed_zaishen_bounties[idx] = !subscribed_zaishen_bounties[idx];
            ImGui::SameLine(offset += zb_width);
        }
        if (show_zaishen_combat_in_window) {
            idx = GetZaishenCombat(&unix);
            ImGui::TextColored(subscribed_zaishen_combats[idx] ? sCol : wCol, zaishen_combat_cycles[idx]);
            if (ImGui::IsItemClicked())
                subscribed_zaishen_combats[idx] = !subscribed_zaishen_combats[idx];
            ImGui::SameLine(offset += zc_width);
        }
        if (show_zaishen_vanquishes_in_window) {
            idx = GetZaishenVanquish(&unix);
            ImGui::TextColored(subscribed_zaishen_vanquishes[idx] ? sCol : wCol, zaishen_vanquish_cycles[idx]);
            if (ImGui::IsItemClicked())
                subscribed_zaishen_vanquishes[idx] = !subscribed_zaishen_vanquishes[idx];
            ImGui::SameLine(offset += zv_width);
        }
        if (show_wanted_quests_in_window) {
            idx = GetWantedByShiningBlade(&unix);
            ImGui::TextColored(subscribed_wanted_quests[idx] ? sCol : wCol, wanted_by_shining_blade_cycles[idx]);
            if (ImGui::IsItemClicked())
                subscribed_wanted_quests[idx] = !subscribed_wanted_quests[idx];
            ImGui::SameLine(offset += ws_width);
        }
        if (show_nicholas_in_window) {
            ImGui::Text("%d %s", GetNicholasItemQuantity(&unix), GetNicholasItemName(&unix));
            ImGui::SameLine(offset += nicholas_width);
        }
        if (show_weekly_bonus_pve_in_window) {
            idx = GetWeeklyBonusPvE(&unix);
            ImGui::TextColored(subscribed_weekly_bonus_pve[idx] ? sCol : wCol, pve_weekly_bonus_cycles[idx]);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(pve_weekly_bonus_descriptions[idx]);
            if (ImGui::IsItemClicked())
                subscribed_weekly_bonus_pve[idx] = !subscribed_weekly_bonus_pve[idx];
            ImGui::SameLine(offset += wbe_width);
        }
        if (show_weekly_bonus_pvp_in_window) {
            idx = GetWeeklyBonusPvP(&unix);
            ImGui::TextColored(subscribed_weekly_bonus_pvp[idx] ? sCol : wCol, pvp_weekly_bonus_cycles[idx]);
            if (ImGui::IsItemHovered())
                ImGui::SetTooltip(pvp_weekly_bonus_descriptions[idx]);
            if (ImGui::IsItemClicked())
                subscribed_weekly_bonus_pvp[idx] = !subscribed_weekly_bonus_pvp[idx];
            ImGui::SameLine(offset += long_text_width);
        }
        ImGui::NewLine();
        unix += 86400;
    }
    ImGui::EndChild();
    ImGui::TextDisabled("Click on a daily quest to get notified when its coming up. Subscribed quests are highlighted in ");
    ImGui::SameLine(0, 0);
    ImGui::TextColored(sCol, "blue");
    ImGui::SameLine(0, 0);
    ImGui::TextDisabled(".");
    return ImGui::End();
}
void DailyQuests::DrawHelp() {
    if (!ImGui::TreeNodeEx("Daily Quest Chat Commands", ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth))
        return;
    ImGui::Text("You can create a 'Send Chat' hotkey to perform any command.");
    ImGui::Bullet(); ImGui::Text("'/zb' prints current zaishen bounty.");
    ImGui::Bullet(); ImGui::Text("'/zm' prints current zaishen mission.");
    ImGui::Bullet(); ImGui::Text("'/zv' prints current zaishen vanquish.");
    ImGui::Bullet(); ImGui::Text("'/zc' prints current zaishen combat.");
    ImGui::Bullet(); ImGui::Text("'/vanguard' prints current pre-searing vanguard quest.");
    ImGui::Bullet(); ImGui::Text("'/wanted' prints current shining blade bounty.");
    ImGui::Bullet(); ImGui::Text("'/nicholas' prints current nicholas location.");
    ImGui::Bullet(); ImGui::Text("'/weekly' prints current weekly bonus.");
    ImGui::Bullet(); ImGui::Text("'/today' prints current daily activities.");
    ImGui::Bullet(); ImGui::Text("'/tomorrow' prints tomorrow's daily activities.");
    ImGui::TreePop();
}
void DailyQuests::DrawSettingInternal() {
    ToolboxWindow::DrawSettingInternal();
    float width = ImGui::GetContentRegionAvail().x / 2;
    ImGui::PushItemWidth(width);
    ImGui::InputInt("Show daily quests for the next ", &daily_quest_window_count);
    ImGui::PopItemWidth();
    ImGui::SameLine(0, 0);
    ImGui::Text("days");
    ImGui::Text("Quests to show in Daily Quests window:");
    ImGui::Indent();
    ImGui::PushItemWidth(width);
    ImGui::Checkbox("Zaishen Bounty", &show_zaishen_bounty_in_window);
    ImGui::SameLine();
    ImGui::Checkbox("Zaishen Combat", &show_zaishen_combat_in_window);
    ImGui::Checkbox("Zaishen Mission", &show_zaishen_missions_in_window);
    ImGui::SameLine();
    ImGui::Checkbox("Zaishen Vanquish", &show_zaishen_vanquishes_in_window);
    ImGui::Checkbox("Wanted by Shining Blade", &show_wanted_quests_in_window);
    ImGui::SameLine();
    ImGui::Checkbox("Nicholas The Traveler", &show_nicholas_in_window);
    ImGui::Checkbox("Weekly Bonus (PvE)", &show_weekly_bonus_pve_in_window);
    ImGui::SameLine();
    ImGui::Checkbox("Weekly Bonus (PvP)", &show_weekly_bonus_pvp_in_window);
    ImGui::InputFloat("Text spacing", &text_width);
    ImGui::PopItemWidth();
    ImGui::Unindent();
}
void DailyQuests::LoadSettings(CSimpleIni* ini) {
    ToolboxWindow::LoadSettings(ini);

    show_zaishen_bounty_in_window = ini->GetBoolValue(Name(), VAR_NAME(show_zaishen_bounty_in_window), show_zaishen_bounty_in_window);
    show_zaishen_combat_in_window = ini->GetBoolValue(Name(), VAR_NAME(show_zaishen_combat_in_window), show_zaishen_combat_in_window);
    show_zaishen_missions_in_window = ini->GetBoolValue(Name(), VAR_NAME(show_zaishen_missions_in_window), show_zaishen_missions_in_window);
    show_zaishen_vanquishes_in_window = ini->GetBoolValue(Name(), VAR_NAME(show_zaishen_vanquishes_in_window), show_zaishen_vanquishes_in_window);
    show_wanted_quests_in_window = ini->GetBoolValue(Name(), VAR_NAME(show_wanted_quests_in_window), show_wanted_quests_in_window);
    show_nicholas_in_window = ini->GetBoolValue(Name(), VAR_NAME(show_nicholas_in_window), show_nicholas_in_window);
    show_weekly_bonus_pve_in_window = ini->GetBoolValue(Name(), VAR_NAME(show_weekly_bonus_pve_in_window), show_weekly_bonus_pve_in_window);
    show_weekly_bonus_pvp_in_window = ini->GetBoolValue(Name(), VAR_NAME(show_weekly_bonus_pvp_in_window), show_weekly_bonus_pvp_in_window);

    const char* zms = ini->GetValue(Name(), VAR_NAME(subscribed_zaishen_missions), "0");
    std::bitset<zm_cnt> zmb(zms);
    for (unsigned int i = 0; i < zmb.size(); ++i) {
        subscribed_zaishen_missions[i] = zmb[i] == 1;
    }

    const char* zbs = ini->GetValue(Name(), VAR_NAME(subscribed_zaishen_bounties), "0");
    std::bitset<zb_cnt> zbb(zbs);
    for (unsigned int i = 0; i < zbb.size(); ++i) {
        subscribed_zaishen_bounties[i] = zbb[i] == 1;
    }

    const char* zcs = ini->GetValue(Name(), VAR_NAME(subscribed_zaishen_combats), "0");
    std::bitset<zc_cnt> zcb(zcs);
    for (unsigned int i = 0; i < zcb.size(); ++i) {
        subscribed_zaishen_combats[i] = zcb[i] == 1;
    }

    const char* zvs = ini->GetValue(Name(), VAR_NAME(subscribed_zaishen_vanquishes), "0");
    std::bitset<zv_cnt> zvb(zvs);
    for (unsigned int i = 0; i < zvb.size(); ++i) {
        subscribed_zaishen_vanquishes[i] = zvb[i] == 1;
    }

    const char* wss = ini->GetValue(Name(), VAR_NAME(subscribed_wanted_quests), "0");
    std::bitset<ws_cnt> wsb(wss);
    for (unsigned int i = 0; i < wsb.size(); ++i) {
        subscribed_wanted_quests[i] = wsb[i] == 1;
    }

    const char* wbes = ini->GetValue(Name(), VAR_NAME(subscribed_weekly_bonus_pve), "0");
    std::bitset<wbe_cnt> wbeb(wbes);
    for (unsigned int i = 0; i < wbeb.size(); ++i) {
        subscribed_weekly_bonus_pve[i] = wbeb[i] == 1;
    }

    const char* wbps = ini->GetValue(Name(), VAR_NAME(subscribed_weekly_bonus_pvp), "0");
    std::bitset<wbp_cnt> wbpb(wbps);
    for (unsigned int i = 0; i < wbpb.size(); ++i) {
        subscribed_weekly_bonus_pvp[i] = wbpb[i] == 1;
    }
}
void DailyQuests::SaveSettings(CSimpleIni* ini) {
    ToolboxWindow::SaveSettings(ini);

    ini->SetBoolValue(Name(), VAR_NAME(show_zaishen_bounty_in_window), show_zaishen_bounty_in_window);
    ini->SetBoolValue(Name(), VAR_NAME(show_zaishen_combat_in_window), show_zaishen_combat_in_window);
    ini->SetBoolValue(Name(), VAR_NAME(show_zaishen_missions_in_window), show_zaishen_missions_in_window);
    ini->SetBoolValue(Name(), VAR_NAME(show_zaishen_vanquishes_in_window), show_zaishen_vanquishes_in_window);
    ini->SetBoolValue(Name(), VAR_NAME(show_wanted_quests_in_window), show_wanted_quests_in_window);
    ini->SetBoolValue(Name(), VAR_NAME(show_nicholas_in_window), show_nicholas_in_window);
    ini->SetBoolValue(Name(), VAR_NAME(show_weekly_bonus_pve_in_window), show_weekly_bonus_pve_in_window);
    ini->SetBoolValue(Name(), VAR_NAME(show_weekly_bonus_pvp_in_window), show_weekly_bonus_pvp_in_window);
    std::bitset<zm_cnt> zmb;
    for (unsigned int i = 0; i < zmb.size(); ++i) {
        zmb[i] = subscribed_zaishen_missions[i] ? 1 : 0;
    }
    ini->SetValue(Name(), VAR_NAME(subscribed_zaishen_missions), zmb.to_string().c_str());

    std::bitset<zb_cnt> zbb;
    for (unsigned int i = 0; i < zbb.size(); ++i) {
        zbb[i] = subscribed_zaishen_bounties[i] ? 1 : 0;
    }
    ini->SetValue(Name(), VAR_NAME(subscribed_zaishen_bounties), zbb.to_string().c_str());

    std::bitset<zc_cnt> zcb;
    for (unsigned int i = 0; i < zcb.size(); ++i) {
        zcb[i] = subscribed_zaishen_combats[i] ? 1 : 0;
    }
    ini->SetValue(Name(), VAR_NAME(subscribed_zaishen_combats), zcb.to_string().c_str());

    std::bitset<zv_cnt> zvb;
    for (unsigned int i = 0; i < zvb.size(); ++i) {
        zvb[i] = subscribed_zaishen_vanquishes[i] ? 1 : 0;
    }
    ini->SetValue(Name(), VAR_NAME(subscribed_zaishen_vanquishes), zvb.to_string().c_str());

    std::bitset<ws_cnt> wsb;
    for (unsigned int i = 0; i < wsb.size(); ++i) {
        wsb[i] = subscribed_wanted_quests[i] ? 1 : 0;
    }
    ini->SetValue(Name(), VAR_NAME(subscribed_wanted_quests), wsb.to_string().c_str());

    std::bitset<wbe_cnt> wbeb;
    for (unsigned int i = 0; i < wbeb.size(); ++i) {
        wbeb[i] = subscribed_weekly_bonus_pve[i] ? 1 : 0;
    }
    ini->SetValue(Name(), VAR_NAME(subscribed_weekly_bonus_pve), wbeb.to_string().c_str());

    std::bitset<wbp_cnt> wbpb;
    for (unsigned int i = 0; i < wbpb.size(); ++i) {
        wbpb[i] = subscribed_weekly_bonus_pvp[i] ? 1 : 0;
    }
    ini->SetValue(Name(), VAR_NAME(subscribed_weekly_bonus_pvp), wbpb.to_string().c_str());
}

void DailyQuests::Initialize() {
    ToolboxWindow::Initialize();

    GW::Chat::CreateCommand(L"zm", DailyQuests::CmdZaishenMission);
    GW::Chat::CreateCommand(L"zb", DailyQuests::CmdZaishenBounty);
    GW::Chat::CreateCommand(L"zc", DailyQuests::CmdZaishenCombat);
    GW::Chat::CreateCommand(L"zv", DailyQuests::CmdZaishenVanquish);
    GW::Chat::CreateCommand(L"vanguard", DailyQuests::CmdVanguard);
    GW::Chat::CreateCommand(L"wanted", DailyQuests::CmdWantedByShiningBlade);
    GW::Chat::CreateCommand(L"nicholas", DailyQuests::CmdNicholas);
    GW::Chat::CreateCommand(L"weekly", DailyQuests::CmdWeeklyBonus);
    GW::Chat::CreateCommand(L"today", [](const wchar_t* message, int argc, LPWSTR* argv) -> void {
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(argc);
        UNREFERENCED_PARAMETER(argv);
        if (GetIsPreSearing()) {
            GW::Chat::SendChat('/', "vanguard");
            GW::Chat::SendChat('/', "nicholas");
            return;
        }
        GW::Chat::SendChat('/', "zm");
        GW::Chat::SendChat('/', "zb");
        GW::Chat::SendChat('/', "zc");
        GW::Chat::SendChat('/', "zv");
        GW::Chat::SendChat('/', "wanted");
        GW::Chat::SendChat('/', "nicholas");
        GW::Chat::SendChat('/', "weekly");
        });
    GW::Chat::CreateCommand(L"daily", [](const wchar_t* message, int argc, LPWSTR* argv) -> void {
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(argc);
        UNREFERENCED_PARAMETER(argv);
        GW::Chat::SendChat('/', "today");
        });
    GW::Chat::CreateCommand(L"dailies", [](const wchar_t* message, int argc, LPWSTR* argv) -> void {
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(argc);
        UNREFERENCED_PARAMETER(argv);
        GW::Chat::SendChat('/', "today");
        });
    GW::Chat::CreateCommand(L"tomorrow", [](const wchar_t* message, int argc, LPWSTR* argv) -> void {
        UNREFERENCED_PARAMETER(message);
        UNREFERENCED_PARAMETER(argc);
        UNREFERENCED_PARAMETER(argv);
        if (GetIsPreSearing()) {
            GW::Chat::SendChat('/', "vanguard tomorrow");
            GW::Chat::SendChat('/', "nicholas tomorrow");
            return;
        }
        GW::Chat::SendChat('/', "zm tomorrow");
        GW::Chat::SendChat('/', "zb tomorrow");
        GW::Chat::SendChat('/', "zc tomorrow");
        GW::Chat::SendChat('/', "zv tomorrow");
        GW::Chat::SendChat('/', "wanted tomorrow");
        GW::Chat::SendChat('/', "nicholas tomorrow");
        });
}

void DailyQuests::Update(float delta) {
    UNREFERENCED_PARAMETER(delta);
    if (subscriptions_changed)
        checked_subscriptions = false;
    if (checked_subscriptions)
        return;
    subscriptions_changed = false;
    if (!start_time)
        start_time = time(nullptr);
    if (GW::Map::GetIsMapLoaded() && (time(nullptr) - start_time) > 1) {
        checked_subscriptions = true;
        // Check daily quests for the next 6 days, and send a message if found. Only runs once when TB is opened.
        time_t now = time(nullptr);
        time_t unix = now + 0;
        uint32_t quest_idx;
        for (unsigned int i = 0; i < subscriptions_lookahead_days; i++) {
            char date_str[32];
            switch (i) {
            case 0:
                sprintf(date_str, "today");
                break;
            case 1:
                sprintf(date_str, "tomorrow");
                break;
            default:
                std::strftime(date_str, 32, "on %A", std::localtime(&unix));
                break;
            }
            if (subscribed_zaishen_missions[quest_idx = GetZaishenMission(&unix)])
                Log::Info("%s is the Zaishen Mission %s", zaishen_mission_cycles[quest_idx], date_str);
            if (subscribed_zaishen_bounties[quest_idx = GetZaishenBounty(&unix)])
                Log::Info("%s is the Zaishen Bounty %s", zaishen_bounty_cycles[quest_idx], date_str);
            if (subscribed_zaishen_combats[quest_idx = GetZaishenCombat(&unix)])
                Log::Info("%s is the Zaishen Combat %s", zaishen_combat_cycles[quest_idx], date_str);
            if (subscribed_zaishen_vanquishes[quest_idx = GetZaishenVanquish(&unix)])
                Log::Info("%s is the Zaishen Vanquish %s", zaishen_vanquish_cycles[quest_idx], date_str);
            if (subscribed_wanted_quests[quest_idx = GetWantedByShiningBlade(&unix)])
                Log::Info("%s is Wanted by the Shining Blade %s", wanted_by_shining_blade_cycles[quest_idx], date_str);
            unix += 86400;
        }

        // Check weekly bonuses / special events
        unix = GetWeeklyRotationTime(&now);
        for (unsigned int i = 0; i < 2; i++) {
            char date_str[32];
            switch (i) {
            case 0: std::strftime(date_str, 32, "until %R on %A", std::localtime(&unix)); break;
            default: std::strftime(date_str, 32, "on %A at %R", std::localtime(&unix)); break;
            }
            if (subscribed_weekly_bonus_pve[quest_idx = GetWeeklyBonusPvE(&unix)])
                Log::Info("%s is the Weekly PvE Bonus %s", pve_weekly_bonus_cycles[quest_idx], date_str);
            if (subscribed_weekly_bonus_pvp[quest_idx = GetWeeklyBonusPvP(&unix)])
                Log::Info("%s is the Weekly PvP Bonus %s", pvp_weekly_bonus_cycles[quest_idx], date_str);
            unix += 604800;
        }
    }
}

void DailyQuests::CmdWeeklyBonus(const wchar_t*, int argc, LPWSTR* argv) {
    time_t now = time(nullptr);
    if (argc > 1 && !wcscmp(argv[1], L"tomorrow"))
        now += 86400;
    PrintDaily(L"Weekly Bonus PvE", pve_weekly_bonus_cycles[GetWeeklyBonusPvE(&now)], now);
    PrintDaily(L"Weekly Bonus PvP", pvp_weekly_bonus_cycles[GetWeeklyBonusPvP(&now)], now);
}
void DailyQuests::CmdZaishenBounty(const wchar_t*, int argc, LPWSTR* argv) {
    time_t now = time(nullptr);
    if (argc > 1 && !wcscmp(argv[1], L"tomorrow"))
        now += 86400;
    PrintDaily(L"Zaishen Bounty", zaishen_bounty_cycles[GetZaishenBounty(&now)], now);
}
void DailyQuests::CmdZaishenMission(const wchar_t*, int argc, LPWSTR* argv) {
    time_t now = time(nullptr);
    if (argc > 1 && !wcscmp(argv[1], L"tomorrow"))
        now += 86400;
    PrintDaily(L"Zaishen Mission", zaishen_mission_cycles[GetZaishenMission(&now)], now);
}
void DailyQuests::CmdZaishenVanquish(const wchar_t*, int argc, LPWSTR* argv) {
    time_t now = time(nullptr);
    if (argc > 1 && !wcscmp(argv[1], L"tomorrow"))
        now += 86400;
    PrintDaily(L"Zaishen Vanquish", zaishen_vanquish_cycles[GetZaishenVanquish(&now)], now);
}
void DailyQuests::CmdZaishenCombat(const wchar_t*, int argc, LPWSTR* argv) {
    time_t now = time(nullptr);
    if (argc > 1 && !wcscmp(argv[1], L"tomorrow"))
        now += 86400;
    PrintDaily(L"Zaishen Combat", zaishen_combat_cycles[GetZaishenCombat(&now)], now);
}
void DailyQuests::CmdWantedByShiningBlade(const wchar_t*, int argc, LPWSTR* argv) {
    time_t now = time(nullptr);
    if (argc > 1 && !wcscmp(argv[1], L"tomorrow"))
        now += 86400;
    PrintDaily(L"Wanted", wanted_by_shining_blade_cycles[GetWantedByShiningBlade(&now)], now);
}
void DailyQuests::CmdVanguard(const wchar_t*, int argc, LPWSTR* argv) {
    time_t now = time(nullptr);
    if (argc > 1 && !wcscmp(argv[1], L"tomorrow"))
        now += 86400;
    PrintDaily(L"Vanguard Quest", GetVanguardQuest(&now), now);
}
void DailyQuests::CmdNicholas(const wchar_t*, int argc, LPWSTR* argv) {
    time_t now = time(nullptr);
    if (argc > 1 && !wcscmp(argv[1], L"tomorrow"))
        now += 86400;
    char buf[128];
    if (GetIsPreSearing()) {
        snprintf(buf, _countof(buf), "5 %s", GetNicholasSandfordLocation(&now));
        PrintDaily(L"Nicholas Sandford", buf, now, false);
    }
    else {
        snprintf(buf, _countof(buf), "%d %s in %s", GetNicholasItemQuantity(&now), GetNicholasItemName(&now), GetNicholasLocation(&now));
        PrintDaily(L"Nicholas the Traveler", buf, now, false);
    }
}
