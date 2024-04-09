package com.chinasoft.backend.model.vo;

import com.baomidou.mybatisplus.annotation.IdType;
import com.baomidou.mybatisplus.annotation.TableId;
import lombok.Data;

import java.util.Date;
import java.util.List;

@Data
public class VisitAndSubscribeVO {

    /**
     * 设施信息列表
     */
   List<Object> facilityList;

}
