package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.mapper.FacilityHeadcountMapper;
import com.chinasoft.backend.model.entity.facility.FacilityHeadcount;
import com.chinasoft.backend.service.statistic.FacilityHeadcountService;
import org.springframework.stereotype.Service;

/**
 * 针对表【facility_headcount(各个游玩设施人数统计表)】的数据库操作Service实现
 *
 * @author 姜堂蕴之
 */
@Service
public class FacilityHeadcountServiceImpl extends ServiceImpl<FacilityHeadcountMapper, FacilityHeadcount>
        implements FacilityHeadcountService {

}




