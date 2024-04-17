package com.chinasoft.backend.service.facility;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.facility.BaseFacility;
import com.chinasoft.backend.model.request.facility.BaseFacilityAddRequest;
import com.chinasoft.backend.model.request.facility.BaseFacilityUpdateRequest;
import com.chinasoft.backend.model.request.facility.BaseFilterRequest;
import com.chinasoft.backend.model.vo.facility.BaseFacilityVO;

import java.util.List;

/**
 * 针对表【base_facility】的数据库操作Service
 *
 * @author 姜堂蕴之 孟祥硕
 */
public interface BaseFacilityService extends IService<BaseFacility> {
    /**
     * 基础设施筛选
     *
     * @author 姜堂蕴之
     */
    List<BaseFacilityVO> getBaseFacility(BaseFilterRequest baseFilterRequest);

    /**
     * 基础设施查询
     *
     * @author 姜堂蕴之
     */
    List<BaseFacilityVO> searchBaseFacility(String keyword);

    /**
     * 增加
     *
     * @author 孟祥硕
     */
    long add(BaseFacilityAddRequest baseFacilityAddRequest);

    /**
     * 修改
     *
     * @author 孟祥硕
     */
    Boolean update(BaseFacilityUpdateRequest baseFacilityUpdateRequest);

    /**
     * 参数校验
     *
     * @author 孟祥硕
     */
    void validParams(BaseFacility baseFacility, boolean add);


}
