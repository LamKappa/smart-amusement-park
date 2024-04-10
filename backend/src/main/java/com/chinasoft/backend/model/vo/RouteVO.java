package com.chinasoft.backend.model.vo;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import com.chinasoft.backend.model.entity.Swiper;
import lombok.Data;

import java.util.Date;
import java.util.List;

@Data
public class RouteVO {
    /**
     * 路线id
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 路线名称
     */
    private String name;

    /**
     * 路线图片URL
     */
    private String imgurl;

    /**
     * 途径设施信息列表
     */
    private List<AmusementFacilityVO> facilityVOList;
}
