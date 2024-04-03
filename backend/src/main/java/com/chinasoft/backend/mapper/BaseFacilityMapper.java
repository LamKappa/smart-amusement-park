package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.BaseFacility;
import org.apache.ibatis.annotations.Mapper;

/**
 * @author 86178
 * @description 针对表【base_facility(基础设施表)】的数据库操作Mapper
 * @createDate 2024-04-03 15:30:47
 * @Entity com.chinasoft.backend.model.entity.BaseFacility
 */
@Mapper
public interface BaseFacilityMapper extends BaseMapper<BaseFacility> {

}




