package com.chinasoft.backend.model.vo;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import com.fasterxml.jackson.annotation.JsonFormat;
import com.fasterxml.jackson.annotation.JsonInclude;
import lombok.Data;

import java.sql.Date;
import java.util.List;

@Data
public class TotalHeadCountVO {

    /**
     * 日期
     */
    @JsonFormat(pattern = "yyyy/MM/dd")
    private Date createTime;

    /**
     * 人数
     */
    private Integer count;

}
