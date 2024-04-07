package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.BaseFacility;
import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.request.BaseFilterRequest;
import com.chinasoft.backend.model.vo.BaseFacilityVO;

import java.util.List;

/**
* @author 皎皎
* @description 针对表【base_facility(基础设施表)】的数据库操作Service
* @createDate 2024-04-05 09:51:44
*/
public interface BaseFacilityService extends IService<BaseFacility> {


    List<BaseFacilityVO> getBaseFacility(BaseFilterRequest baseFilterRequest);
}
