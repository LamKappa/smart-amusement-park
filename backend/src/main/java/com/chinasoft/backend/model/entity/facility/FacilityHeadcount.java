package com.chinasoft.backend.model.entity.facility;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableField;
import com.baomidou.mybatisplus.annotation.TableId;
import com.baomidou.mybatisplus.annotation.TableName;
import lombok.Data;

import java.io.Serializable;
import java.util.Date;

/**
 * 各个游玩设施人数统计表
 *
 * @TableName facility_headcount
 * @author 姜堂蕴之
 */
@TableName(value = "facility_headcount")
@Data
public class FacilityHeadcount implements Serializable {
    /**
     * 主键ID
     */
    @TableId(type = IdType.AUTO)
    private Long id;

    /**
     * 设施ID
     */
    private Long facilityId;

    /**
     * 游玩人数
     */
    private Integer count;

    /**
     * 添加时间
     */
    private Date createTime;

    /**
     * 修改时间
     */
    private Date updateTime;

    /**
     * 是否删除（0-未删除 1-已删除）
     */
    private Integer isDeleted;

    @TableField(exist = false)
    private static final long serialVersionUID = 1L;
}