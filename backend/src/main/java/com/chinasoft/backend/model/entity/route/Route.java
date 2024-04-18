package com.chinasoft.backend.model.entity.route;

import com.baomidou.mybatisplus.annotation.*;
import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

/**
 * 路线表
 *
 * @TableName route
 * @author 姜堂蕴之
 */
@TableName(value = "route")
@Data
@JsonInclude(JsonInclude.Include.NON_NULL)
public class Route implements Serializable {
    /**
     * 路线ID
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
     * 路线使用次数统计
     */
    private Integer useCount;

    /**
     * 添加时间
     */
    private Date createTime;

    /**
     * 修改时间
     */
    private Date updateTime;

    /**
     * 逻辑删除标志（0-未删除 1-已删除）
     */
    @TableLogic
    private Integer isDeleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}