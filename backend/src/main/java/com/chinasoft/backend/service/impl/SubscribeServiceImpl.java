package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.conditions.update.UpdateWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.common.ErrorCode;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.Subscribe;
import com.chinasoft.backend.model.entity.Visit;
import com.chinasoft.backend.model.request.VisitAndSubscribeAddRequest;
import com.chinasoft.backend.model.request.VisitAndSubscribeDeleteRequest;
import com.chinasoft.backend.service.SubscribeService;
import com.chinasoft.backend.mapper.SubscribeMapper;
import org.springframework.stereotype.Service;

/**
* @author 皎皎
* @description 针对表【subscribe(用户订阅记录表)】的数据库操作Service实现
* @createDate 2024-04-09 11:20:51
*/
@Service
public class SubscribeServiceImpl extends ServiceImpl<SubscribeMapper, Subscribe>
    implements SubscribeService{

    @Override
    public Subscribe addSubscribe(VisitAndSubscribeAddRequest visitAndSubscribeAddRequest) {
        Long userId = Long.valueOf(visitAndSubscribeAddRequest.getUserId());
        Long facilityId = Long.valueOf(visitAndSubscribeAddRequest.getFacility().getFacilityId());
        Integer facilityType = visitAndSubscribeAddRequest.getFacility().getFacilityType();

        // 检查是否已存在相同数据的记录
        QueryWrapper<Subscribe> queryWrapper = Wrappers.<Subscribe>query()
                .eq("user_id", userId)
                .eq("facility_id", facilityId)
                .eq("facility_type", facilityType);

        Subscribe existingSubscribe = this.baseMapper.selectOne(queryWrapper);
        if (existingSubscribe != null) {
            throw new BusinessException(ErrorCode.PARAMS_ERROR, "订阅重复");
        }

        // 不存在则正常插入
        Subscribe subscribe = new Subscribe();
        subscribe.setUserId(userId);
        subscribe.setFacilityId(facilityId);
        subscribe.setFacilityType(facilityType);

        this.baseMapper.insert(subscribe);

        return this.baseMapper.selectOne(Wrappers.<Subscribe>query().eq("id", subscribe.getId()));
    }

    @Override
    public Boolean deleteSubscribe(VisitAndSubscribeDeleteRequest visitAndSubscribeDeleteRequest) {
        Long id = visitAndSubscribeDeleteRequest.getId();

        // 设置is_deleted字段为1
        UpdateWrapper<Subscribe> updateWrapper = new UpdateWrapper<>();
        updateWrapper.eq("id", id).set("is_deleted", 1);

        // 执行更新操作
        int result = this.baseMapper.update(null, updateWrapper);

        // 判断是否更新成功
        return result > 0;
    }
}




