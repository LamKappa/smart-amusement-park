package com.chinasoft.backend.model.request.route;

import lombok.Data;

import java.util.List;

/**
 * 路线更新请求
 *
 * @author 姜堂蕴之
 */
@Data
public class UpdateRouteRequest {
    private Long id;
    private String name;
    private String imgUrl;
    List<Integer> facilityIdList;
}
