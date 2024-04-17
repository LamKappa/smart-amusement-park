package com.chinasoft.backend.controller.facility;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.service.facility.SearchService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

/**
 * 设施搜索接口
 *
 * @author 姜堂蕴之
 */
@RestController
public class SearchController {

    @Autowired
    private SearchService searchService;

    /**
     * 根据关键字对设施名称、设施简介、设施类型进行搜索
     * 如果搜索关键字为空，返回所有设施信息列表
     *
     * @param keyword 搜索关键字，可能包含设施名称、简介、类型等信息
     * @return BaseResponse<List<Object>> 包含设施信息的列表的响应对象
     * @throws BusinessException 当传入的参数为空时，抛出业务异常
     */
    @GetMapping("/search")
    public BaseResponse<List<Object>> search(@Param("keyword") String keyword) {
        // 判断参数是否为空
        if (keyword == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库获取设施信息列表
        List<Object> data = searchService.search(keyword);

        // 查询结果封装为成功的响应对象并返回
        return ResultUtils.success(data);
    }

}
