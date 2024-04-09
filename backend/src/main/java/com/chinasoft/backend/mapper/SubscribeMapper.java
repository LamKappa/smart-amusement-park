package com.chinasoft.backend.mapper;

import com.chinasoft.backend.model.entity.Subscribe;
import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import org.apache.ibatis.annotations.Mapper;

/**
* @author 皎皎
* @description 针对表【subscribe(用户订阅记录表)】的数据库操作Mapper
* @createDate 2024-04-09 11:20:51
* @Entity com.chinasoft.backend.model.entity.Subscribe
*/
@Mapper
public interface SubscribeMapper extends BaseMapper<Subscribe> {

}




