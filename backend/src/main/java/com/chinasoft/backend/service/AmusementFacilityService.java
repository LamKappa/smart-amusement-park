package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.AmusementFacility;
import com.baomidou.mybatisplus.extension.service.IService;

import java.util.List;

/**
* @author 皎皎
* @description 针对表【amusement_facility】的数据库操作Service
* @createDate 2024-04-05 09:47:45
*/
public interface AmusementFacilityService extends IService<AmusementFacility> {

    List<AmusementFacility> getAmusementName(String name);

    List<AmusementFacility> getAmusementType(String type);

    List<AmusementFacility> getAmusementHeight(Integer height);

    List<AmusementFacility> getAmusementCrowd(String crowd);
}