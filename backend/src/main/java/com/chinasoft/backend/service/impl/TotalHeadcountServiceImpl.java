package com.chinasoft.backend.service.impl;

import cn.hutool.core.bean.BeanUtil;
import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.mapper.TotalHeadcountMapper;
import com.chinasoft.backend.model.entity.TotalHeadcount;
import com.chinasoft.backend.model.vo.statistic.TotalHeadCountVO;
import com.chinasoft.backend.service.statistic.TotalHeadcountService;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.List;

/**
 * 针对表【total_headcount(总人数统计表)】的数据库操作Service实现
 *
 * @author 姜堂蕴之
 */
@Service
public class TotalHeadcountServiceImpl extends ServiceImpl<TotalHeadcountMapper, TotalHeadcount>
        implements TotalHeadcountService {

    /**
     * 按日期返回日期当天的游玩总人数
     *
     * @return BaseResponse<List<TotalHeadCountVO>> 包含按日期排列的游玩总人数列表的响应对象
     * @author 姜堂蕴之
     */
    @Override
    public List<TotalHeadCountVO> getTotalCountArrangeByDate() {

        List<TotalHeadCountVO> totalHeadCountVOList = new ArrayList<>();

        // 创建QueryWrapper对象并设置查询条件
        QueryWrapper<TotalHeadcount> queryWrapper = Wrappers.<TotalHeadcount>query()
                .orderByDesc("create_time") // 按create_time降序排序
                .last("LIMIT 10"); // 限制返回的记录数为10条
        List<TotalHeadcount> totalHeadcountList = this.baseMapper.selectList(queryWrapper);

        // 填入数据
        for (TotalHeadcount totalHeadcount : totalHeadcountList) {
            TotalHeadCountVO totalHeadCountVO = new TotalHeadCountVO();
            BeanUtil.copyProperties(totalHeadcount, totalHeadCountVO);
            totalHeadCountVOList.add(totalHeadCountVO);
        }


        return totalHeadCountVOList;
    }

}



