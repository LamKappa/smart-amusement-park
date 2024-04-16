package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.TotalHeadcount;
import org.apache.ibatis.annotations.Mapper;

/**
 * @author 姜堂蕴之
 * @description 针对表【total_headcount(总人数统计表)】的数据库操作Mapper
 * @createDate 2024-04-10 17:56:36
 * @Entity com.chinasoft.backend.model.entity.TotalHeadcount
 */
@Mapper
public interface TotalHeadcountMapper extends BaseMapper<TotalHeadcount> {

}




