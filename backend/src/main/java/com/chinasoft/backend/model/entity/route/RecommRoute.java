package com.chinasoft.backend.model.entity.route;

import com.baomidou.mybatisplus.annotation.*;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

/**
 * 推荐路线表
 *
 * @TableName recomm_route
 */
@TableName(value = "recomm_route")
@Data
public class RecommRoute implements Serializable {
    /**
     *
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 路线id
     */
    private Long routeId;

    /**
     * 设施id
     */
    private Long facilityId;

    /**
     * 路线顺序（按1-2-3-4从小到大的顺序）
     */
    private Integer priority;

    /**
     * 添加时间
     */
    private Date createTime;

    /**
     * 修改时间
     */
    private Date updateTime;

    /**
     * 逻辑删除 （0-未删除 1-已删除）
     */
    @TableLogic
    private Integer isDeleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}