package com.chinasoft.backend.model.request.facility;

import com.baomidou.mybatisplus.annotation.TableField;
import lombok.Data;

import java.sql.Time;
import java.util.List;

/**
 * 基础设施添加请求
 *
 * @author 孟祥硕
 */
@Data
public class BaseFacilityAddRequest {

    /**
     * 名称
     */
    private String name;

    /**
     * 经度
     */
    private String longitude;

    /**
     * 维度
     */
    private String latitude;

    /**
     * 最大容纳人数
     */
    private Integer maxCapacity;

    /**
     * 预计时间（以分钟为单位）
     */
    private Integer expectTime;

    /**
     * 开放时间
     */
    private Time startTime;

    /**
     * 关闭时间
     */
    private Time closeTime;


    /**
     * 设施照片列表
     */
    private List<String> imageUrls;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}
