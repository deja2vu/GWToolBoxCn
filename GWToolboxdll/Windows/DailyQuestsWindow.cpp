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
    u8"草寇",
    u8"尤提尼 乌坡乌坡",
    u8"阿斯卡隆 贵族",
    u8"不死族 ",
    u8"火焰恶魔 悲伤刀片",
    u8"农民 哈姆奈特",
    u8"夏尔",
    u8"伯爵夫人 娜佳",
    u8"仆从 泰特"
};
static const char* nicholas_sandford_cycles[52] = {
    u8"穴居人项链",
    u8"烧焦外壳",
    u8"骷髅手臂",
    u8"古怪的种子",
    u8"附魔磁石",
    u8"鳞怪鳍",
    u8"冰磁石",
    u8"石像鬼头颅",
    u8"阴暗的甲壳",
    u8"烧焦外壳",
    u8"红色鸢尾花",
    u8"蜘蛛腿",
    u8"骷髅手臂",
    u8"夏尔雕刻品",
    u8"附魔磁石",
    u8"穴居人项链",
    u8"冰磁石",
    u8"破旧的腰带",
    u8"石像鬼头颅",
    u8"古怪的种子",
    u8"鳞怪鳍",
    u8"红色鸢尾花",
    u8"附魔磁石",
    u8"骷髅手臂",
    u8"夏尔雕刻品",
    u8"蜘蛛腿",
    u8"烧焦外壳",
    u8"石像鬼头颅",
    u8"古怪的种子",
    u8"冰磁石",
    u8"穴居人项链",
    u8"附魔磁石",
    u8"破旧的腰带",
    u8"阴暗的甲壳",
    u8"蜘蛛腿",
    u8"石像鬼头颅",
    u8"冰磁石",
    u8"古怪的种子",
    u8"破旧的腰带",
    u8"穴居人项链",
    u8"烧焦外壳",
    u8"骷髅手臂",
    u8"红色鸢尾花",
    u8"夏尔雕刻品",
    u8"鳞怪鳍",
    u8"阴暗的甲壳",
    u8"附魔磁石",
    u8"夏尔雕刻品",
    u8"蜘蛛腿",
    u8"红色鸢尾花",
    u8"破旧的腰带",
    u8"阴暗的甲壳"
};
static const char* nicholas_region_cycles[137] = {
    u8"阿斯卡隆",
    u8"南席娃山脉",
    u8"荒芜之地",
    u8"灰暗沿岸",
    u8"凯宁城",
    u8"南席娃山脉",
    u8"高楠",
    u8"火环列岛",
    u8"星岬岛",
    u8"艾斯坦",
    u8"夏尔家园",
    u8"科瑞塔",
    u8"碧玉海",
    u8"梅古玛丛林",
    u8"阿斯卡隆",
    u8"荒芜之地",
    u8"科瑞塔",
    u8"碧玉海",
    u8"席娃山脉极端",
    u8"夏尔家园",
    u8"星岬岛",
    u8"瓦贝",
    u8"凯宁城",
    u8"北席娃山脉",
    u8"阿斯卡隆",
    u8"碧玉海",
    u8"水晶沙漠",
    u8"席娃山脉极端",
    u8"南席娃山脉",
    u8"高楠",
    u8"梅古玛丛林",
    u8"瓦贝",
    u8"科凡德森林",
    u8"科瑞塔",
    u8"瓦贝",
    u8"星岬岛",
    u8"凯宁城",
    u8"水晶沙漠",
    u8"南席娃山脉",
    u8"艾斯坦",
    u8"科凡德森林",
    u8"灰暗沿岸",
    u8"水晶沙漠",
    u8"南席娃山脉",
    u8"凯宁城",
    u8"南席娃山脉",
    u8"艾斯坦",
    u8"科凡德森林",
    u8"科瑞塔",
    u8"灰暗沿岸",
    u8"瓦贝",
    u8"荒芜之地",
    u8"南席娃山脉",
    u8"凯宁城",
    u8"高楠",
    u8"水晶沙漠",
    u8"南席娃山脉",
    u8"碧玉海",
    u8"科瑞塔",
    u8"席娃山脉极端",
    u8"星岬岛",
    u8"北席娃山脉",
    u8"阿斯卡隆",
    u8"瓦贝",
    u8"凯宁城",
    u8"科瑞塔",
    u8"科凡德森林",
    u8"席娃山脉极端",
    u8"梅古玛丛林",
    u8"艾斯坦",
    u8"科瑞塔",
    u8"凯宁城",
    u8"水晶沙漠",
    u8"梅古玛丛林",
    u8"科凡德森林",
    u8"艾斯坦",
    u8"瓦贝",
    u8"灰暗沿岸",
    u8"水晶沙漠",
    u8"凯宁城",
    u8"北席娃山脉",
    u8"荒芜之地",
    u8"科瑞塔",
    u8"瓦贝",
    u8"水晶沙漠",
    u8"星岬岛",
    u8"梅古玛丛林",
    u8"南席娃山脉",
    u8"高楠",
    u8"瓦贝",
    u8"梅古玛丛林",
    u8"星岬岛",
    u8"北席娃山脉",
    u8"荒芜之地",
    u8"碧玉海",
    u8"高楠",
    u8"科瑞塔",
    u8"科瑞塔",
    u8"科凡德森林",
    u8"阿斯卡隆",
    u8"荒芜之地",
    u8"碧玉海",
    u8"高楠",
    u8"灰暗沿岸",
    u8"南席娃山脉",
    u8"梅古玛丛林",
    u8"星岬岛",
    u8"艾斯坦",
    u8"凯宁城",
    u8"瓦贝",
    u8"阿斯卡隆",
    u8"碧玉海",
    u8"席娃山脉极端",
    u8"高楠",
    u8"南席娃山脉",
    u8"凯宁城",
    u8"高楠",
    u8"阿斯卡隆",
    u8"科凡德森林",
    u8"南席娃山脉",
    u8"灰暗沿岸",
    u8"荒芜之地",
    u8"星岬岛",
    u8"荒芜之地",
    u8"科瑞塔",
    u8"凯宁城",
    u8"瓦贝",
    u8"夏尔家园",
    u8"阿斯卡隆",
    u8"碧玉海",
    u8"科瑞塔",
    u8"高楠",
    u8"梅古玛丛林",
    u8"艾斯坦",
    u8"席娃山脉极端",
    u8"北席娃山脉",
    u8"阿斯卡隆"
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
    u8"红色鸢尾花",
    u8"阿比卡拉头皮羽毛",
    u8"玛骨奈面具",
    u8"长尾冠毛",
    u8"瘟疫法器",
    u8"湛蓝残留物",
    u8"曼陀罗根糕点",
    u8"码果的爪",
    u8"螳螂镰",
    u8"知觉种子",
    u8"石穴居人项链",
    u8"鲱鱼",
    u8"迦纳外皮",
    u8"黑暗种子",
    u8"夏尔皮",
    u8"红宝石巨灵精华",
    u8"多刺的甲壳",
    u8"骨制护符",
    u8"莫得米鬃毛",
    u8"超级夏尔雕刻品",
    u8"羊皮纸卷",
    u8"苍穹咆哮者之爪",
    u8"柳树枝",
    u8"席娃山脉鬃毛",
    u8"恶臭的甲壳",
    u8"月贝",
    u8"粗大下颚骨",
    u8"染色的麟片",
    u8"马赛特记号",
    u8"知觉磁石",
    u8"丛林巨魔獠牙",
    u8"蓝宝石巨灵精华",
    u8"石雕品",
    u8"卡洛米羽毛头皮",
    u8"掠夺的货品",
    u8"红颅金币",
    u8"翡翠手镯",
    u8"牛头怪角",
    u8"冻结的狮鹫兽翅膀",
    u8"银铸币",
    u8"松露",
    u8"司怪爪",
    u8"干燥的三头龙爪",
    u8"冰冻的心脏",
    u8"天神质体",
    u8"幻影残留物",
    u8"烤龙兽肉",
    u8"琥珀",
    u8"灼热的心脏",
    u8"蜥蜴骨头",
    u8"巨兽皮革",
    u8"发亮的石头",
    u8"精细的穴居人项链",
    u8"硬玉",
    u8"金古币",
    u8"干枯的眼睛",
    u8"冰磁石",
    u8"锐利细鬼爪",
    u8"硬瘤",
    u8"元素之土",
    u8"纳迦皮",
    u8"心灵之板",
    u8"暴风之眼",
    u8"鸟妖翅膀",
    u8"灵魂石",
    u8"尖刺的颈脊",
    u8"龙根",
    u8"狂战士的角",
    u8"巨兽颚",
    u8"鳞怪鳍汤",
    u8"森林牛头怪的角",
    u8"腐败胞囊",
    u8"翡翠下颚骨",
    u8"梅古玛鬃毛",
    u8"头骨土符",
    u8"曼陀罗根",
    u8"瓦贝红酒",
    u8"编织者的腿",
    u8"黄宝石颈脊",
    u8"腐败兽獠牙",
    u8"霜火尖牙",
    u8"恶魔残片",
    u8"畸形的种子",
    u8"钻石巨灵精华",
    u8"遗忘者图章",
    u8"红颅铜币",
    u8"生苔下颚骨",
    u8"奴隶石",
    u8"伊洛那皮革",
    u8"深蓝之爪",
    u8"梅古玛蜘蛛丝",
    u8"被遗忘的小箱子",
    u8"冰雪瘤",
    u8"喷沙磁石",
    u8"黑珍珠",
    u8"甲虫壳",
    u8"石像魔头颅",
    u8"腐烂的欧尔纹章",
    u8"调合后的玻璃瓶",
    u8"烧焦的磁石",
    u8"水巨灵精华",
    u8"守护者苔",
    u8"矮人啤酒",
    u8"双面人的舌头",
    u8"柏木种子",
    u8"纠结的种子",
    u8"失窃的补给品",
    u8"伊波枷沙拉",
    u8"寄生虫皮革",
    u8"苍穹咆哮者之心",
    u8"强韧的爪",
    u8"湛蓝羽冠",
    u8"角顿皮毛",
    u8"哈克蛙舌",
    u8"山巨魔獠牙",
    u8"小瓶墨水",
    u8"高楠垂饰",
    u8"烧焦的石像鬼头颅",
    u8"挖掘者之牙",
    u8"石峰标志",
    u8"环蛇皮",
    u8"铭刻碎片",
    u8"羽头皮",
    u8"木乃伊裹尸布",
    u8"阴影残留物",
    u8"古河童壳",
    u8"晶石",
    u8"纤维曼陀罗根",
    u8"可怕的胸腔",
    u8"海妖之眼",
    u8"泥鳞怪的鳍",
    u8"知觉孢子",
    u8"远古之眼",
    u8"铜先令",
    u8"呆板曼陀罗外壳",
    u8"亚麻布",
    u8"夏尔雕刻品"
};
static const char* nicholas_location_cycles[137] = {
    u8"统治者之谷",
    u8"矿物泉源",
    u8"剧毒地表",
    u8"纠结之艾卡滋亚",
    u8"瓦江市场",
    u8"无惧者之丘",
    u8"阿尔科监禁区",
    u8"破灭石",
    u8"绍商小径",
    u8"旧城，法兰努尔",
    u8"圣诺谷",
    u8"双头蛇湖泊",
    u8"今凯山",
    u8"陷落区",
    u8"缺口",
    u8"金属镕炉",
    u8"王者安息处",
    u8"席亚火山口",
    u8"维拉戛阵地",
    u8"达拉达山地",
    u8"祯台郡",
    u8"希伯欣花园",
    u8"巴德克小径",
    u8"戴尔狄摩盆地",
    u8"东方边境",
    u8"盖拉孵化所",
    u8"枯竭之海",
    u8"冰崖裂口",
    u8"浮冰",
    u8"巴多克洞穴",
    u8"纠结之根",
    u8"奢华之城，莫肯",
    u8"亭石",
    u8"科瑞塔北部",
    u8"舟克辛卷藏室",
    u8"海幽泻湖",
    u8"谭纳凯神殿",
    u8"先知之路",
    u8"蛇舞",
    u8"梅坦尼之钥",
    u8"摩洛神秘通道",
    u8"原野瀑布",
    u8"断崖",
    u8"枪尖山",
    u8"纳普区",
    u8"洛拿斯通道",
    u8"伊斯诺岛",
    u8"蕨谷",
    u8"魟鱼湖滨",
    u8"撕裂大地",
    u8"巴萨荒野",
    u8"水晶高地",
    u8"威特曼的怪异建筑",
    u8"阴暗通道",
    u8"野蛮河岸",
    u8"天际流域",
    u8"冰顶",
    u8"寂静之浪",
    u8"尼伯山丘",
    u8"卓卡湖",
    u8"班让半岛",
    u8"狮鹫兽隘口",
    u8"麻点平原",
    u8"高地广场",
    u8"莱苏皇宫",
    u8"战死者之泪",
    u8"德瑞札灌木林",
    u8"亚加 摩瑞音",
    u8"玛奴泻湖",
    u8"列隆流域",
    u8"凯席斯山",
    u8"孙江行政区",
    u8"盐滩",
    u8"银树",
    u8"永恒之林",
    u8"洛天帝沼泽",
    u8"巍天帝峡谷",
    u8"玛古斯之石",
    u8"预言者之坡",
    u8"朋美谷",
    u8"铁砧石",
    u8"破裂之心",
    u8"突马克荒地",
    u8"隐藏之城，哈达辛",
    u8"秃鹰沙丘",
    u8"欣弥领地",
    u8"双头怪隐匿地",
    u8"古兰斯的足迹",
    u8"夏亥峭壁",
    u8"威金矿坑",
    u8"芦苇沼泽地",
    u8"周大臣庄园",
    u8"铁马矿山",
    u8"碎裂沟谷",
    u8"群岛",
    u8"马加海岸",
    u8"瞭望塔海岸",
    u8"诅咒之地",
    u8"哀伤之墓瀑布",
    u8"旧阿斯卡隆",
    u8"托雷长廊",
    u8"麦尚山丘",
    u8"曼克隆泛滥平原",
    u8"火星蝇沼泽",
    u8"冰冻森林",
    u8"干燥高地",
    u8"蛇野断崖",
    u8"贾林平原",
    u8"夏安便道",
    u8"丽之镜湖",
    u8"阿斯卡隆丘陵",
    u8"沉睡之水",
    u8"碧拉边境",
    u8"达贾林庄园",
    u8"碎石坡道",
    u8"申赞通道",
    u8"弦月要塞，干达拉",
    u8"底耶沙低地",
    u8"梅兰朵的盼望",
    u8"塔斯加之死",
    u8"安博湾",
    u8"杰格领地",
    u8"桑泉谷",
    u8"硫磺荒地",
    u8"黑色帷幕",
    u8"地下城",
    u8"亚天帝峡谷",
    u8"古斯玛战争丘陵地",
    u8"巨龙峡谷",
    u8"风神海床",
    u8"恶汉山丘",
    u8"向阳边境",
    u8"贤者领地",
    u8"杜夏悬崖",
    u8"诺恩之心领地",
    u8"旅人谷",
    u8"夏尔火焰神殿"
};
static const char* zaishen_bounty_cycles[DailyQuests::zb_cnt] = {
    u8"沙之法师 卓加姆",
    u8"野兽看守者罗彦",
    u8"怪异双头巨人",
    u8"复仇牛头怪",
    u8"铁趾 弗朗尼",
    u8"尔果",
    u8"芬瑞",
    u8"希维塔姆",
    u8"莫比 风喙",
    u8"暗黑幽灵",
    u8"恶臭骨龙",
    u8"不洁 咒暗",
    u8"宰杀者 科薛克",
    u8"湖之女 蜜希",
    u8"冻击 弑族者",
    u8"火翼 坤维",
    u8"晶蒙",
    u8"巨大暗影",
    u8"TPS调节高轮",
    u8"疫之破坏者",
    u8"暗影(先王之墓)",
    u8"上将 坎托",
    u8"风吼 柏格斯",
    u8"炼冶维特",
    u8"怒浪 保博",
    u8"缓和者 卓夫",
    u8"拉喀 食人者",
    u8"得道者 村",
    u8"霸王 贾多斯",
    u8"地缚 纳法斯图",
    u8"钢铁巨人",
    u8"麦格默斯",
    u8"碎之主 魔兵",
    u8"残酷 贾米里",
    u8"黑色 唐肯",
    u8"通灵者 魁嵩",
    u8"冥狱地王",
    u8"冻 耀尔伊欧",
    u8"黑色魔兽 阿尔古",
    u8"奥拉赫妮",
    u8"多姆四骑士",
    u8"残余的断片",
    u8"大地之唤 亚伯",
    u8"多彩乌兹",
    u8"领主库贝",
    u8"强者 捷地",
    u8"薇娜的祝福 桑斯",
    u8"司法官 汤米兹",
    u8"冰石哈恩/麦辛",
    u8"迅捷 派维特",
    u8"梵蒂宁",
    u8"魔法盒 牧格里",
    u8"曼席斯的祭司",
    u8"火之主 音丧多",
    u8"柯博海特 食髓者",
    u8"指挥官 瓦里",
    u8"加奈赛",
    u8"卡布斯",
    u8"石尾 摩洛托夫",
    u8"冥狱霸主",
    u8"巫妖之龙",
    u8"霍克 灵叹",
    u8"骨之舞者 葛西",
    u8"夜之女 幕兰凯",
    u8"织暴者 硬皮",
    u8"死灵法师 芙瑞达"
};
static const char* zaishen_combat_cycles[DailyQuests::zc_cnt] = {
    u8"翡翠矿场",
    u8"Codex 竞技场",
    u8"英雄之路",
    u8"公会战",
    u8"同盟战",
    u8"英雄之路",
    u8"公会战",
    u8"Codex 竞技场",
    u8"杨木要塞",
    u8"翡翠矿场",
    u8"随机竞技场",
    u8"Codex 竞技场",
    u8"公会战",
    u8"翡翠矿场",
    u8"同盟战",
    u8"英雄之路",
    u8"随机竞技场",
    u8"杨木要塞",
    u8"翡翠矿场",
    u8"随机竞技场",
    u8"杨木要塞",
    u8"英雄之路",
    u8"同盟战",
    u8"公会战",
    u8"Codex 竞技场",
    u8"随机竞技场",
    u8"杨木要塞",
    u8"同盟战"
};
static const char* zaishen_mission_cycles[DailyQuests::zm_cnt] = {
    u8"占卜之石",
    u8"希贝克大公廷",
    u8"悲伤冰谷",
    u8"莱苏皇宫",
    u8"荒芜之地入口",
    u8"干枯河流",
    u8"黑潮之穴",
    u8"对抗夏尔",
    u8"地狱隘口",
    u8"纳度湾",
    u8"神圣海岸",
    u8"祯台郡",
    u8"波甘驿站",
    u8"谭纳凯神殿",
    u8"北方长城",
    u8"达沙走廊",
    u8"荒原",
    u8"沉睡之水",
    u8"夏贝克村庄",
    u8"欧若拉林地",
    u8"英雄的光采年代",
    u8"领事馆码头",
    u8"火环群岛",
    u8"纳普区",
    u8"龙穴",
    u8"萨岗诺棱堡",
    u8"达雷西海滨",
    u8"袭击根据地",
    u8"永恒之林",
    u8"神圣沙滩",
    u8"里欧恩难民营",
    u8"战争军团之友",
    u8"柏里斯通道",
    u8"帝国圣所",
    u8"摩多克裂缝",
    u8"若拉尼学院",
    u8"破坏的深渊",
    u8"凡特墓地",
    u8"瑞尼克要塞",
    u8"一道过远的门",
    u8"周大臣庄园",
    u8"雷云要塞",
    u8"提亚克林地",
    u8"寻找血石",
    u8"绝望沙丘",
    u8"薇茹广场",
    u8"卓坎诺挖掘点",
    u8"莫拉登矿山",
    u8"科登诺路口",
    u8"天才操控而让生活丰富迷人的表现",
    u8"亭石",
    u8"科瑞塔关所",
    u8"疯狂之门",
    u8"乌啦的实验室",
    u8"河畔地带",
    u8"风神海床",
    u8"摩拉废墟",
    u8"地狱悬崖",
    u8"苏米亚废墟",
    u8"诺恩熊的诅咒",
    u8"孙江行政区",
    u8"伊洛那流域",
    u8"惩罚之门",
    u8"血债血偿",
    u8"血石沼泽",
    u8"征钠群落",
    u8"盖拉孵化所",
    u8"亚霸顿之门",
    u8"寒霜之门"
};
static const char* zaishen_vanquish_cycles[DailyQuests::zv_cnt] = {
    u8"蛇野断崖",
    u8"舟克辛卷藏室",
    u8"冰崖裂口",
    u8"狮鹫兽隘口",
    u8"欣弥领地",
    u8"伊斯诺岛",
    u8"亚加 摩瑞音",
    u8"浮冰",
    u8"麦尚山丘",
    u8"夏亥峭壁",
    u8"撕裂大地",
    u8"冰顶",
    u8"周大臣庄园",
    u8"梅坦尼之钥",
    u8"圣诺谷",
    u8"铁马矿山",
    u8"摩洛神秘通道",
    u8"贾林平原",
    u8"火星蝇沼泽",
    u8"凯席斯山",
    u8"哀伤之墓瀑布",
    u8"金属镕炉",
    u8"维拉戛阵地",
    u8"洛拿斯通道",
    u8"朋美谷",
    u8"曼克隆泛滥平原",
    u8"原野瀑布",
    u8"王者安息处",
    u8"莱苏皇宫",
    u8"隐藏之城，哈达辛",
    u8"席亚火山口",
    u8"玛奴泻湖",
    u8"阴暗通道",
    u8"丽之镜湖",
    u8"绍商小径",
    u8"尼伯山丘",
    u8"申赞通道",
    u8"破裂之心",
    u8"盐滩",
    u8"科瑞塔北部",
    u8"寂静之浪",
    u8"碎裂沟谷",
    u8"恶汉山丘",
    u8"旧阿斯卡隆",
    u8"孙江行政区",
    u8"硫磺荒地",
    u8"玛古斯之石",
    u8"破灭石",
    u8"桑泉谷",
    u8"托雷长廊",
    u8"诺恩之心领地",
    u8"麻点平原",
    u8"谭纳凯神殿",
    u8"威金矿坑",
    u8"剧毒地表",
    u8"先知之路",
    u8"永恒之林",
    u8"塔斯加之死",
    u8"奢华之城，莫肯",
    u8"芦苇沼泽地",
    u8"沉睡之水",
    u8"魟鱼湖滨",
    u8"向阳边境",
    u8"统治者之谷",
    u8"瓦江市场",
    u8"亚天帝峡谷",
    u8"双头蛇湖泊",
    u8"贤者领地",
    u8"夏安便道",
    u8"列隆流域",
    u8"纠结之根",
    u8"银树",
    u8"祯台郡",
    u8"枯竭之海",
    u8"纳普区",
    u8"天际流域",
    u8"断崖",
    u8"黑色帷幕",
    u8"班让半岛",
    u8"蛇舞",
    u8"旅人谷",
    u8"缺口",
    u8"洛天帝沼泽",
    u8"枪尖山",
    u8"今凯山",
    u8"马加海岸",
    u8"梅兰朵的盼望",
    u8"陷落区",
    u8"杰格领地",
    u8"秃鹰沙丘",
    u8"巴萨荒野",
    u8"突马克荒地",
    u8"巍天帝峡谷",
    u8"碎石坡道",
    u8"矿物泉源",
    u8"铁砧石",
    u8"亭石",
    u8"威特曼的怪异建筑",
    u8"阿尔科监禁区",
    u8"阿斯卡隆丘陵",
    u8"巴多克洞穴",
    u8"诅咒之地",
    u8"纠结之艾卡滋亚",
    u8"群岛",
    u8"东方边境",
    u8"达贾林庄园",
    u8"瞭望塔海岸",
    u8"安博湾",
    u8"野蛮河岸",
    u8"戴尔狄摩盆地",
    u8"风神海床",
    u8"杜夏悬崖",
    u8"底耶沙低地",
    u8"巴德克小径",
    u8"碧拉边境",
    u8"水晶高地",
    u8"预言者之坡",
    u8"达拉达山地",
    u8"德瑞札灌木林",
    u8"旧城，法兰努尔",
    u8"巨龙峡谷",
    u8"蕨谷",
    u8"高地广场",
    u8"无惧者之丘",
    u8"卓卡湖",
    u8"干燥高地",
    u8"战死者之泪",
    u8"盖拉孵化所",
    u8"双头怪隐匿地",
    u8"弦月要塞，干达拉",
    u8"古斯玛战争丘陵地",
    u8"夏尔火焰神殿",
    u8"海幽泻湖",
    u8"冰冻森林",
    u8"希伯欣花园",
    u8"古兰斯的足迹"
};
static const char* wanted_by_shining_blade_cycles[DailyQuests::ws_cnt] = {
    u8"判官 绮米",
    u8"精疲力竭的萨恩",
    u8"判官 席凡",
    u8"强欲者瓦卡",
    u8"残酷者阿莫列克",
    u8"饥饿者卡纳克",
    u8"猖獗的瓦里斯",
    u8"瑟瑞丝",
    u8"染血之手萨妮亚",
    u8"真实追寻者戴斯特",
    u8"直言者瑟列娜丝",
    u8"判官 亚蜜琳",
    u8"细心的马希米连",
    u8"敌意 舟",
    u8"算计者巴希穆斯",
    u8"卡拉米托斯",
    u8"傲慢的葛力斯",
    u8"被诅咒的莉芙",
    u8"判官 马隆",
    u8"判官 卡珊卓拉",
    u8"争论者 薇丝"
};
static const char* pve_weekly_bonus_cycles[DailyQuests::wbe_cnt] = {
    u8"份外运气奖",
    u8"支援伊洛那奖",
    u8"战承悬赏奖",
    u8"二章精英奖",
    u8"四章支援奖",
    u8"战承主线奖",
    u8"神殿奖",
    u8"二章支援奖",
    u8"战承清图奖"
};
static const char* pve_weekly_bonus_descriptions[9] = {
    u8"获以往两倍的幸运及不幸运分；获四倍以往的钥匙和开锁道具掉率",
    u8"获以往两倍的日戟和光明分",
    u8"战承悬赏任务奖品(铜战承币)翻倍",
    u8"可从凯宁城进入深渊和尔果",
    u8"获以往两倍的阿苏拉, 戴尔狄摩, 黑檀先锋队, 和诺恩分",
    u8"战承主线任务奖品(铜战承币)翻倍",
    u8"可免费进入地下和灾难",
    u8"上交分数时，获以往两倍的库兹柯和勒克森分",
    u8"战承清图任务奖品(铜战承币)翻倍"
};
static const char* pvp_weekly_bonus_cycles[DailyQuests::wbp_cnt] = {
    u8"随机竞技场",
    u8"公会战",
    u8"二章竞技",
    u8"英雄之路",
    u8"Codex 竞技场",
    u8"同盟战"
};
static const char* pvp_weekly_bonus_descriptions[6] = {
    u8"随机竞技场内 获 以往两倍的 巴萨泽 和 斗士 分",
    u8"公会战内获 以往两倍的 巴萨泽 和 冠军 分",
    u8"翡翠矿场 和 杨木要塞内 获 以往两倍的 巴萨泽 和 帝国 分",
    u8"英雄之路内 获 以往两倍的 巴萨泽 和 英雄称号 分",
    u8"Codex竞技场内 获 以往两倍的 巴萨泽 和 Codex称号 分",
    u8"同盟战内 获 以往两倍的 巴萨泽 和 帝国 分"
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

    ImGui::Text(u8"日期");
    ImGui::SameLine(offset += short_text_width);
    if (show_zaishen_missions_in_window) {
        ImGui::Text(u8"战承主线");
        ImGui::SameLine(offset += zm_width);
    }
    if (show_zaishen_bounty_in_window) {
        ImGui::Text(u8"战承悬赏");
        ImGui::SameLine(offset += zb_width);
    }
    if (show_zaishen_combat_in_window) {
        ImGui::Text(u8"战承对战");
        ImGui::SameLine(offset += zc_width);
    }
    if (show_zaishen_vanquishes_in_window) {
        ImGui::Text(u8"战承清图");
        ImGui::SameLine(offset += zv_width);
    }
    if (show_wanted_quests_in_window) {
        ImGui::Text(u8"光刃通缉");
        ImGui::SameLine(offset += ws_width);
    }
    if (show_nicholas_in_window) {
        ImGui::Text(u8"旅者");
        ImGui::SameLine(offset += nicholas_width);
    }
    if (show_weekly_bonus_pve_in_window) {
        ImGui::Text(u8"每周奖励 (游戏奖励)");
        ImGui::SameLine(offset += wbe_width);
    }
    if (show_weekly_bonus_pvp_in_window) {
        ImGui::Text(u8"每周奖励 (竞赛奖励)");
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
            ImGui::Text(u8"今日");
            break;
        case 1:
            ImGui::Text(u8"明日");
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
