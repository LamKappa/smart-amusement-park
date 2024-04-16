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
 * @author 姜堂蕴之
 * @description 针对表【total_headcount(总人数统计表)】的数据库操作Service实现
 * @createDate 2024-04-10 17:56:36
 */
@Service
public class TotalHeadcountServiceImpl extends ServiceImpl<TotalHeadcountMapper, TotalHeadcount>
        implements TotalHeadcountService {

    @Override
    public List<TotalHeadCountVO> getTotalCountArrangeByDate() {

        List<TotalHeadCountVO> totalHeadCountVOList = new ArrayList<>();

        // 创建QueryWrapper对象并设置查询条件
        QueryWrapper<TotalHeadcount> queryWrapper = Wrappers.<TotalHeadcount>query()
                .orderByDesc("create_time") // 按create_time降序排序
                .last("LIMIT 10"); // 限制返回的记录数为10条

        // 执行查询并获取结果列表，每条记录都是一个Map，key为字段名，value为字段值
        List<TotalHeadcount> totalHeadcountList = this.baseMapper.selectList(queryWrapper);

        for (TotalHeadcount totalHeadcount : totalHeadcountList) {
            TotalHeadCountVO totalHeadCountVO = new TotalHeadCountVO();
            BeanUtil.copyProperties(totalHeadcount, totalHeadCountVO);
            totalHeadCountVOList.add(totalHeadCountVO);
        }


        return totalHeadCountVOList;
    }

}



