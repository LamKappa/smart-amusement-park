package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.TotalHeadcount;
import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.vo.TotalHeadCountVO;

import java.util.List;
import java.util.Map;

/**
* @author 皎皎
* @description 针对表【total_headcount(总人数统计表)】的数据库操作Service
* @createDate 2024-04-10 17:56:36
*/
public interface TotalHeadcountService extends IService<TotalHeadcount> {
    List<TotalHeadCountVO> getTotalCountArrangeByDate();
}
