package com.chinasoft.backend.controller.facility;

import com.chinasoft.backend.common.BaseResponse;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.common.ResultUtils;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.service.SearchService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.repository.query.Param;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.RestController;

import java.util.List;

@RestController
public class SearchController {

    @Autowired
    SearchService searchService;

    /**
     * 根据名称 简介 类型 查询各种设施
     *
     * @param keyword
     * @return
     */
    @GetMapping("/search")
    public BaseResponse<List<Object>> search(@Param("keyword") String keyword) {
        if (keyword == null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR);
        }

        // 查询数据库
        List<Object> data = searchService.search(keyword);

        // 返回响应
        return ResultUtils.success(data);
    }

}
