package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.Visit;
import org.apache.ibatis.annotations.MapKey;
import org.apache.ibatis.annotations.Mapper;
import org.apache.ibatis.annotations.Select;

import java.util.List;
import java.util.Map;

/**
 * @author 姜堂蕴之
 * @description 针对表【visit(用户打卡记录表)】的数据库操作Mapper
 * @createDate 2024-04-09 11:20:30
 * @Entity com.chinasoft.backend.model.entity.Visit
 */
@Mapper
public interface VisitMapper extends BaseMapper<Visit> {

    @MapKey("facilityId")
    Map<Long, Map<String, Long>> visitCount();

    @Select("SELECT facility_id, COUNT(*) as visitCount " +
            "FROM visit " +
            "WHERE facility_type = 0 " +
            "GROUP BY facility_id " +
            "ORDER BY visitCount DESC " +
            "LIMIT 4")
    List<Map> getTopFourFacilitiesByVisitCount();
}




