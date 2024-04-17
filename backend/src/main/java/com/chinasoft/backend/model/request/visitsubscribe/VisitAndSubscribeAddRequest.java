package com.chinasoft.backend.model.request.visitsubscribe;

import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import lombok.Data;

/**
 * 打卡订阅添加请求
 *
 * @author 姜堂蕴之
 */
@Data
public class VisitAndSubscribeAddRequest {
    /**
     * 用户ID
     */
    private Long userId;

    /**
     * 设施ID+设施名称
     */
    private FacilityIdType facility;
}
