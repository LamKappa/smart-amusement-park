package com.chinasoft.backend.model.request.route;

import lombok.Data;

import java.util.List;

/**
 * 路线添加请求
 *
 * @author 姜堂蕴之
 */
@Data
public class AddRouteRequest {
    /**
     * 路线名称
     */
    private String name;
    /**
     * 路线图片
     */
    private String imgUrl;
    /**
     * 途径设施ID列表
     */
    List<Integer> facilityIdList;
}
