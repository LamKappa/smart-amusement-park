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

    private Long userId;

    private FacilityIdType facility;
}
