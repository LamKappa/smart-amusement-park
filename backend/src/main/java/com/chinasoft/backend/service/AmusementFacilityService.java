package com.chinasoft.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.request.AmusementFilterRequest;
import com.chinasoft.backend.model.vo.AmusementFacilityVO;

import java.util.List;

/**
 * @author 皎皎
 * @description 针对表【amusement_facility】的数据库操作Service
 * @createDate 2024-04-05 09:47:45
 */
public interface AmusementFacilityService extends IService<AmusementFacility> {

    List<AmusementFacilityVO> getAmusementFacility(AmusementFilterRequest amusementFilterRequest);

    
}
