package com.chinasoft.backend.service.facility;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.CrowdingLevel;
import com.chinasoft.backend.model.entity.facility.FacilityIdType;
import com.chinasoft.backend.model.vo.statistic.CrowingTimeCountVO;

import java.util.List;

/**
 * @author 孟祥硕
 * @description 针对表【crowding_level(拥挤度表)】的数据库操作Service
 * @createDate 2024-04-07 16:11:50
 */
public interface CrowdingLevelService extends IService<CrowdingLevel> {

    Integer getExpectWaitTimeByIdType(FacilityIdType facilityIdType);

    List<CrowingTimeCountVO> crowingTimeCount();
}
