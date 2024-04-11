package com.chinasoft.backend.service;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.Visit;
import com.chinasoft.backend.model.request.VisitAndSubscribeAddRequest;
import com.chinasoft.backend.model.request.VisitAndSubscribeDeleteRequest;
import com.chinasoft.backend.model.vo.FacilityVisitCountVO;

import java.util.List;

/**
 * @author 皎皎
 * @description 针对表【visit(用户打卡记录表)】的数据库操作Service
 * @createDate 2024-04-09 11:20:30
 */
public interface VisitService extends IService<Visit> {

    Visit addVisit(VisitAndSubscribeAddRequest visitAndSubscribeAddRequest);

    Boolean deleteVisit(VisitAndSubscribeDeleteRequest visitAndSubscribeDeleteRequest);

    
    List<FacilityVisitCountVO> visitCount();
}
