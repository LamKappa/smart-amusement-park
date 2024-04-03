package com.chinasoft.backend.mapper;

import com.baomidou.mybatisplus.core.mapper.BaseMapper;
import com.chinasoft.backend.model.entity.User;
import org.apache.ibatis.annotations.Mapper;

/**
 * @author 86178
 * @description 针对表【user】的数据库操作Mapper
 * @createDate 2024-04-03 14:44:10
 * @Entity com.chinasoft.backend.model.entity.User
 */
@Mapper
public interface UserMapper extends BaseMapper<User> {

}




