package com.chinasoft.backend.service;

import java.util.List;

/**
 * @author 姜堂蕴之
 * @description 针对搜索的数据库操作Service
 * @createDate 2024-04-05 16:57:10
 */
public interface SearchService {

    List<Object> search(String keyword);
}
