package com.chinasoft.backend.model.vo;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityVO;
import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

import java.util.List;

@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
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
    private String imgUrl;

    /**
     * 途径设施信息列表
     */
    private List<AmusementFacilityVO> swiperList;
}
