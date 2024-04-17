package com.chinasoft.backend.service.statistic;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.TotalHeadcount;
import com.chinasoft.backend.model.vo.statistic.TotalHeadCountVO;

import java.util.List;

/**
 * @author 姜堂蕴之
 * @description 针对表【total_headcount(总人数统计表)】的数据库操作Service
 * @createDate 2024-04-10 17:56:36
 */
public interface TotalHeadcountService extends IService<TotalHeadcount> {
    /**
     * 按日期返回日期当天的游玩总人数
     *
     * @return BaseResponse<List<TotalHeadCountVO>> 包含按日期排列的游玩总人数列表的响应对象
     * @author 姜堂蕴之
     */
    List<TotalHeadCountVO> getTotalCountArrangeByDate();
}
