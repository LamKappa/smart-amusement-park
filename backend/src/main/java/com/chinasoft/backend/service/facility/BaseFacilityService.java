package com.chinasoft.backend.service.facility;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.facility.BaseFacility;
import com.chinasoft.backend.model.request.facility.BaseFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.BaseFacilityUpdateRequest;
import com.chinasoft.backend.model.request.facility.BaseFilterRequest;
import com.chinasoft.backend.model.vo.facility.BaseFacilityVO;

import java.util.List;

/**
 * @author 姜堂蕴之
 */
public interface BaseFacilityService extends IService<BaseFacility> {


    List<BaseFacilityVO> getBaseFacility(BaseFilterRequest baseFilterRequest);


    long add(BaseFacilityAddRequest baseFacilityAddRequest);

    Boolean update(BaseFacilityUpdateRequest baseFacilityUpdateRequest);

    void validParams(BaseFacility baseFacility, boolean add);
}
