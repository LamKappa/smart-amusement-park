package com.chinasoft.backend.service.facility;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.CrowdingLevel;
import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import com.chinasoft.backend.model.vo.statistic.CrowingTimeCountVO;

import java.util.List;

/**
 * 针对表【crowding_level(拥挤度表)】的数据库操作Service
 *
 * @author 孟祥硕
 */
public interface CrowdingLevelService extends IService<CrowdingLevel> {
    /**
     * 根据设施ID和设施类型获取预计等待实践
     */
    Integer getExpectWaitTimeByIdType(FacilityIdType facilityIdType);

    /**
     * 以小时为单位统计当日的拥挤度
     */
    List<CrowingTimeCountVO> crowingTimeCount();
}
