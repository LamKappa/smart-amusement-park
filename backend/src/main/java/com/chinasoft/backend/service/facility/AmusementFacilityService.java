package com.chinasoft.backend.service.facility;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.facility.AmusementFacility;
import com.chinasoft.backend.model.request.facility.AmusementFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.AmusementFacilityUpdateRequest;
import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityVO;

import java.util.List;

/**
 * @author 姜堂蕴之
 * @description 针对表【amusement_facility】的数据库操作Service
 * @createDate 2024-04-05 09:47:45
 */
public interface AmusementFacilityService extends IService<AmusementFacility> {

    List<AmusementFacilityVO> getAmusementFacility(AmusementFilterRequest amusementFilterRequest);


    /**
     * 参数校验
     */
    void validParams(AmusementFacility amusementFacility, boolean b);

    long add(AmusementFacilityAddRequest amusementFacilityAddRequest);

    /**
     * 修改
     */
    Boolean update(AmusementFacilityUpdateRequest amusementFacilityUpdateRequest);
}
