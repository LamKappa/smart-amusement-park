package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.conditions.update.UpdateWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.FacilityImage;
import com.chinasoft.backend.model.entity.Visit;
import com.chinasoft.backend.model.request.VisitAndSubscribeAddRequest;
import com.chinasoft.backend.model.request.VisitAndSubscribeDeleteRequest;
import com.chinasoft.backend.model.vo.VisitAndSubscribeVO;
import com.chinasoft.backend.service.VisitService;
import com.chinasoft.backend.mapper.VisitMapper;
import org.springframework.stereotype.Service;

@Service
public class VisitServiceImpl extends ServiceImpl<VisitMapper, Visit>
    implements VisitService{

    @Override
    public Visit addVisit(VisitAndSubscribeAddRequest visitAndSubscribeAddRequest) {
        Long userId = Long.valueOf(visitAndSubscribeAddRequest.getUserId());
        Long facilityId = Long.valueOf(visitAndSubscribeAddRequest.getFacility().getFacilityId());
        Integer facilityType = visitAndSubscribeAddRequest.getFacility().getFacilityType();

        // 检查是否已存在相同数据的记录
        QueryWrapper<Visit> queryWrapper = Wrappers.<Visit>query()
                .eq("user_id", userId)
                .eq("facility_id", facilityId)
                .eq("facility_type", facilityType);

        Visit existingVisit = this.baseMapper.selectOne(queryWrapper);
        if (existingVisit != null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "打卡重复");
        }

        // 不存在则正常插入
        Visit visit = new Visit();
        visit.setUserId(userId);
        visit.setFacilityId(facilityId);
        visit.setFacilityType(facilityType);

        this.baseMapper.insert(visit);

        return this.baseMapper.selectOne(Wrappers.<Visit>query().eq("id", visit.getId()));
    }

    @Override
    public Boolean deleteVisit(VisitAndSubscribeDeleteRequest visitAndSubscribeDeleteRequest) {
        Long id = visitAndSubscribeDeleteRequest.getId();

        // 设置is_deleted字段为1
        UpdateWrapper<Visit> updateWrapper = new UpdateWrapper<>();
        updateWrapper.eq("id", id).set("is_deleted", 1);

        // 执行更新操作
        int result = this.baseMapper.update(null, updateWrapper);

        // 判断是否更新成功
        return result > 0;

    }
}




