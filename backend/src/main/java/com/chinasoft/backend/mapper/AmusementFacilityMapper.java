package com.chinasoft.backend.mapper;

import com.chinasoft.backend.model.entity.AmusementFacility;
import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import org.apache.ibatis.annotations.Mapper;

import java.util.List;

/**
* @author 皎皎
* @description 针对表【amusement_facility】的数据库操作Mapper
* @createDate 2024-04-05 09:47:45
* @Entity com.chinasoft.backend.model.entity.AmusementFacility
*/
@Mapper
public interface AmusementFacilityMapper extends BaseMapper<AmusementFacility> {
    List<Integer> selectAllFacilityIds();
}




