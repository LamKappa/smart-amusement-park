package com.chinasoft.backend.constant;

public enum RestaurantFacilityTypeEnum {
    // 中式快餐/面点/饮品/小吃/西式快餐
    ZHONG_SHI_KUAI_CAN("中式快餐"),
    XI_SHI_KUAI_CAN("西式快餐"),
    MIAN_DIAN("面点"),
    YIN_PIN("饮品"),
    XIAO_CHI("小吃");

    private String type;

    RestaurantFacilityTypeEnum(String type) {
        this.type = type;
    }

    public String getType() {
        return type;
    }

    public void setType(String type) {
        this.type = type;
    }

    /**
     * 判断是否在枚举类中
     */
    public static boolean existValidate(String value) {
        if (value == null || "".equals(value)) {
            return false;
        }
        for (RestaurantFacilityTypeEnum testEnum : RestaurantFacilityTypeEnum.values()) {
            if (testEnum.getType().equals(value)) {
                return true;
            }
        }
        return false;
    }
}
