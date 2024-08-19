#ifndef __OPENRM_STRUCTURE_ENUMS_HPP__
#define __OPENRM_STRUCTURE_ENUMS_HPP__

namespace rm {

enum GrayScaleMethod {
    GRAY_SCALE_METHOD_HSV,
    GRAY_SCALE_METHOD_RGB,
    GRAY_SCALE_METHOD_CVT,
    GRAY_SCALE_METHOD_MIX,
    GRAY_SCALE_METHOD_SUB
};

enum BinaryMethod {
    BINARY_METHOD_AVERAGE_THRESHOLD,
    BINARY_METHOD_DIRECT_THRESHOLD,
    BINARY_METHOD_MAX_MIN_RATIO
};

enum FindPointMethod {
    FIND_POINT_METHOD_BARYCENTER,
    FIND_POINT_METHOD_RECT_CROSSPOINT
};

enum TeamColor {
    TEAM_COLOR_BLUE,
    TEAM_COLOR_RED
};

enum ArmorColor {
    ARMOR_COLOR_BLUE,
    ARMOR_COLOR_RED,
    ARMOR_COLOR_NONE,
    ARMOR_COLOR_PURPLE
};

enum ArmorSize {
    ARMOR_SIZE_UNKNOWN,
    ARMOR_SIZE_SMALL_ARMOR,
    ARMOR_SIZE_BIG_ARMOR
};

enum ArmorID {
    ARMOR_ID_SENTRY,
    ARMOR_ID_HERO,
    ARMOR_ID_ENGINEER,
    ARMOR_ID_INFANTRY_3,
    ARMOR_ID_INFANTRY_4,
    ARMOR_ID_INFANTRY_5,
    ARMOR_ID_TOWER,
    ARMOR_ID_RUNE,
    ARMOR_ID_DART,
    ARMOR_ID_UNKNOWN,
    ARMOR_ID_COUNT
};

enum ArmorElevation {
    ARMOR_ELEVATION_UP_15,
    ARMOR_ELEVATION_UP_75,
    ARMOR_ELEVATION_DOWN_15,
    ARMOR_ELEVATION_NONE
};

enum ArmorClass {
    ARMOR_TYPE_SENTRY_BLUE,
    ARMOR_TYPE_HERO_BLUE,
    ARMOR_TYPE_ENGINEER_BLUE,
    ARMOR_TYPE_INFANTRY_3_BLUE,
    ARMOR_TYPE_INFANTRY_4_BLUE,
    ARMOR_TYPE_INFANTRY_5_BLUE,
    ARMOR_TYPE_OUTPOST_BLUE,
    ARMOR_TYPE_BASE_BLUE,
    ARMOR_TYPE_BASE_BIG_BLUE,

    ARMOR_TYPE_SENTRY_RED,
    ARMOR_TYPE_HERO_RED,
    ARMOR_TYPE_ENGINEER_RED,
    ARMOR_TYPE_INFANTRY_3_RED,
    ARMOR_TYPE_INFANTRY_4_RED,
    ARMOR_TYPE_INFANTRY_5_RED,
    ARMOR_TYPE_OUTPOST_RED,
    ARMOR_TYPE_BASE_RED,
    ARMOR_TYPE_BASE_BIG_RED,

    ARMOR_TYPE_SENTRY_NONE,
    ARMOR_TYPE_HERO_NONE,
    ARMOR_TYPE_ENGINEER_NONE,
    ARMOR_TYPE_INFANTRY_3_NONE,
    ARMOR_TYPE_INFANTRY_4_NONE,
    ARMOR_TYPE_INFANTRY_5_NONE,
    ARMOR_TYPE_OUTPOST_NONE,
    ARMOR_TYPE_BASE_NONE,
    ARMOR_TYPE_BASE_BIG_NONE,

    ARMOR_TYPE_SENTRY_PURPLE,
    ARMOR_TYPE_HERO_PURPLE,
    ARMOR_TYPE_ENGINEER_PURPLE,
    ARMOR_TYPE_INFANTRY_3_PURPLE,
    ARMOR_TYPE_INFANTRY_4_PURPLE,
    ARMOR_TYPE_INFANTRY_5_PURPLE,
    ARMOR_TYPE_OUTPOST_PURPLE,
    ARMOR_TYPE_BASE_PURPLE,
    ARMOR_TYPE_BASE_BIG_PURPLE
};


// 0000 0x0    0001 0x1    0010 0x2    0011 0x3
// 0100 0x4    0101 0x5    0110 0x6    0111 0x7
// 1000 0x8    1001 0x9    1010 0xA    1011 0xB
// 1100 0xC    1101 0xD    1110 0xE    1111 0xF

enum Config {
    CONFIG_UNFIRE               = 0x00000000,       // 自瞄->电控  不开火
    CONFIG_FIRE                 = 0x00000001,       // 自瞄->电控  开火

    CONFIG_TARGET_COLOR_RED     = 0x00000000,       // 电控->自瞄  敌方颜色：红色
    CONFIG_TARGET_COLOR_BLUE    = 0x00000002,       // 电控->自瞄  敌方颜色：蓝色

    CONFIG_AUTOAIM_STANDBY      = 0x00000000,       // 电控->自瞄  自瞄状态：待机
    CONGIF_AOTOAIM_ARMOR        = 0x00000004,       // 电控->自瞄  自瞄状态：装甲板
    CONFIG_AOTOAIM_RUNE         = 0x00000008,       // 电控->自瞄  自瞄状态：打符
    
    CONFIG_TARGET_ID_SENTRY     = 0x00000000,       // 电控->自瞄  目标ID哨兵  (哨兵用)
    CONFIG_TARGET_ID_HERO       = 0x00000010,       // 电控->自瞄  目标ID英雄  (哨兵用)
    CONFIG_TARGET_ID_ENGINEER   = 0x00000020,       // 电控->自瞄  目标ID工程  (哨兵用)
    CONFIG_TARGET_ID_INFANTRY_3 = 0x00000030,       // 电控->自瞄  目标ID步兵3 (哨兵用)
    CONFIG_TARGET_ID_INFANTRY_4 = 0x00000040,       // 电控->自瞄  目标ID步兵4 (哨兵用)
    CONFIG_TARGET_ID_INFANTRY_5 = 0x00000050,       // 电控->自瞄  目标ID步兵5 (哨兵用)
    CONFIG_TARGET_ID_TOWER      = 0x00000060,       // 电控->自瞄  目标ID哨塔  (哨兵用)
    CONFIG_TARGET_ID_RUNE       = 0x00000070,       // 电控->自瞄  目标ID符文  (哨兵用)
    CONFIG_TARGET_ID_OUTPOST    = 0x00000080,       // 电控->自瞄  目标ID前哨  (哨兵用)
    CONFIG_TARGET_ID_BASE       = 0x00000090,       // 电控->自瞄  目标ID基地  (哨兵用)

    CONFIG_BALANCE_INFANTRY_3   = 0x00000100,       // 电控<->自瞄  步兵3为平衡 (哨兵用)
    CONFIG_BALANCE_INFANTRY_4   = 0x00000200,       // 电控<->自瞄  步兵4为平衡 (哨兵用)
    CONFIG_BALANCE_INFANTRY_5   = 0x00000400,       // 电控<->自瞄  步兵5为平衡 (哨兵用)
    
    CONFIG_FIRE_MODE_CONTINUOUS = 0x00000000,       // 电控->自瞄  连发模式  (哨兵用)
    CONFIG_FIRE_MODE_STABLE     = 0x00000800,       // 电控->自瞄  稳定模式  (哨兵用)
    
    CONFIG_ERROR_NO_CAMERA      = 0x10000000,       // 自瞄->电控  无相机错误
};

enum ConfigMask {
    CONFIG_MASK_FIRE            = 0x00000001,       // 开火掩码
    CONFIG_MASK_TARGET_COLOR    = 0x00000002,       // 敌方颜色掩码
    CONFIG_MASK_AUTOAIM_MODE    = 0x0000000C,       // 自瞄模式掩码
    CONFIG_MASK_TARGET_ID       = 0x000000F0,       // 目标ID掩码
    CONFIG_MASK_BALANCE         = 0x00000700,       // 步兵平衡掩码
    CONFIG_MASK_FIRE_MODE       = 0x00000800,       // 射击模式掩码
    CONFIG_MASK_WARNING         = 0x0F000000,       // 警告掩码
    CONFIG_MASK_ERROR           = 0xF0000000        // 错误掩码
};

enum ConfigShift {
    CONFIG_SHIFT_FIRE           = 0,                // 开火位移
    CONFIG_SHIFT_TARGET_COLOR   = 1,                // 敌方颜色位移
    CONFIG_SHIFT_AUTOAIM_MODE   = 2,                // 自瞄模式位移
    CONFIG_SHIFT_TARGET_ID      = 4,                // 目标ID位移
    CONFIG_SHIFT_BALANCE        = 8,                // 步兵平衡位移
    CONFIG_SHIFT_FIRE_MODE      = 11,               // 射击模式位移
    CONFIG_SHIFT_WARNING        = 24,               // 警告位移
    CONFIG_SHIFT_ERROR          = 28                // 错误位移
};

}

#endif