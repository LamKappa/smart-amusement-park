package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.CrowdingLevel;
import org.apache.ibatis.annotations.Mapper;

/**
 * @author 孟祥硕
 * @description 针对表【crowding_level(拥挤度表)】的数据库操作Mapper
 * @createDate 2024-04-07 16:11:50
 * @Entity com.chinasoft.backend.model.entity.CrowdingLevel
 */
@Mapper
public interface CrowdingLevelMapper extends BaseMapper<CrowdingLevel> {

}




