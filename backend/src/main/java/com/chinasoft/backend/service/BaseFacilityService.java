package com.chinasoft.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.BaseFacility;
import com.chinasoft.backend.model.request.BaseFacilityAddRequest;
import com.chinasoft.backend.model.request.BaseFacilityUpdateRequest;
import com.chinasoft.backend.model.request.BaseFilterRequest;
import com.chinasoft.backend.model.vo.BaseFacilityVO;

import java.util.List;

/**
 * @author 皎皎
 */
public interface BaseFacilityService extends IService<BaseFacility> {


    List<BaseFacilityVO> getBaseFacility(BaseFilterRequest baseFilterRequest);


    long add(BaseFacilityAddRequest baseFacilityAddRequest);

    Boolean update(BaseFacilityUpdateRequest baseFacilityUpdateRequest);

    void validParams(BaseFacility baseFacility, boolean add);
}
