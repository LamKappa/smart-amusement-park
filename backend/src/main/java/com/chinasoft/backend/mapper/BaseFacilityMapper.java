package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.facility.BaseFacility;
import org.apache.ibatis.annotations.Mapper;

/**
 * @author 姜堂蕴之
 * @description 针对表【base_facility(基础设施表)】的数据库操作Mapper
 * @createDate 2024-04-05 09:51:44
 * @Entity com.chinasoft.backend.model.entity.facility.BaseFacility
 */
@Mapper
public interface BaseFacilityMapper extends BaseMapper<BaseFacility> {

}




