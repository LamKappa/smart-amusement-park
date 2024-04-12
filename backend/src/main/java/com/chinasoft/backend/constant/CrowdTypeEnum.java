package com.chinasoft.backend.constant;

public enum CrowdTypeEnum {
    // 成人、老少皆宜、家长监护
    CHENG_REN("成人"),
    LAO_SHAO_JIE_YI("老少皆宜"),
    JIA_ZHANG_JIAN_HU("家长监护");

    private String crowdType;

    CrowdTypeEnum(String crowdType) {
        this.crowdType = crowdType;
    }

    public String getCrowdType() {
        return crowdType;
    }

    public void setCrowdType(String crowdType) {
        this.crowdType = crowdType;
    }

    /**
     * 判断是否在枚举类中
     */
    public static boolean existValidate(String value) {
        if (value == null || "".equals(value)) {
            return false;
        }
        for (CrowdTypeEnum testEnum : CrowdTypeEnum.values()) {
            if (testEnum.getCrowdType().equals(value)) {
                return true;
            }
        }
        return false;
    }
}
