package com.chinasoft.backend.constant;

public enum AmusementFacilityTypeEnum {
    // 轨道、失重、水上、室内、旋转、鬼屋
    GUI_DAO("轨道"),
    SHI_ZHONG("失重"),
    SHUI_SHANG("水上"),
    SHI_NEI("室内"),
    XUAN_ZHUAN("旋转"),
    GUI_WU("鬼屋");

    private String type;

    AmusementFacilityTypeEnum(String type) {
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
        for (AmusementFacilityTypeEnum testEnum : AmusementFacilityTypeEnum.values()) {
            if (testEnum.getType().equals(value)) {
                return true;
            }
        }
        return false;
    }
}
