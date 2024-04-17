package com.chinasoft.backend.service.facility;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.facility.AmusementFacility;
import com.chinasoft.backend.model.request.facility.AmusementFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.AmusementFacilityUpdateRequest;
import com.chinasoft.backend.model.request.facility.AmusementFilterRequest;
import com.chinasoft.backend.model.vo.facility.AmusementFacilityVO;

import java.util.List;

/**
 * 针对表【amusement_facility】的数据库操作Service
 *
 * @author 姜堂蕴之 孟祥硕
 */
public interface AmusementFacilityService extends IService<AmusementFacility> {
    /**
     * 游乐设施筛选
     *
     * @author 姜堂蕴之
     */
    List<AmusementFacilityVO> getAmusementFacility(AmusementFilterRequest amusementFilterRequest);

    /**
     * 游乐设施查询
     *
     * @author 姜堂蕴之
     */
    List<AmusementFacilityVO> searchAmusementFacility(String keyword);


    /**
     * 参数校验
     *
     * @author 孟祥硕
     */
    void validParams(AmusementFacility amusementFacility, boolean b);

    /**
     * 增加
     *
     * @author 孟祥硕
     */
    long add(AmusementFacilityAddRequest amusementFacilityAddRequest);

    /**
     * 修改
     *
     * @author 孟祥硕
     */
    Boolean update(AmusementFacilityUpdateRequest amusementFacilityUpdateRequest);
}
