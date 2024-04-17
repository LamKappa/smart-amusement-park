package com.chinasoft.backend.service.facility;

import com.chinasoft.backend.exception.BusinessException;

import java.util.List;

/**
 * 设施搜索Service
 *
 * @author 姜堂蕴之
 */
public interface SearchService {

    /**
     * 根据关键字对设施名称、设施简介、设施类型进行搜索
     *
     * @param keyword 搜索关键字，可能包含设施名称、简介、类型等信息
     * @return BaseResponse<List<Object>> 包含设施信息的列表的响应对象
     * @throws BusinessException 当传入的参数为空时，抛出业务异常
     */
    List<Object> search(String keyword);
}
