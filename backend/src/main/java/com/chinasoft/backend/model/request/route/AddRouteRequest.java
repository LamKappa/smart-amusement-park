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
    private String name;
    private String imgUrl;
    List<Integer> facilityIdList;
}
