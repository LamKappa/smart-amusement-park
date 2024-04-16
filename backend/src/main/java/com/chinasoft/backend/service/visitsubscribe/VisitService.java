package com.chinasoft.backend.service.visitsubscribe;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.Visit;
import com.chinasoft.backend.model.request.visitsubscribe.VisitAndSubscribeAddRequest;
import com.chinasoft.backend.model.vo.statistic.FacilityVisitCountVO;

import java.util.List;

/**
 * @author 姜堂蕴之
 * @description 针对表【visit(用户打卡记录表)】的数据库操作Service
 * @createDate 2024-04-09 11:20:30
 */
public interface VisitService extends IService<Visit> {

    Visit addVisit(VisitAndSubscribeAddRequest visitAndSubscribeAddRequest);

    List<FacilityVisitCountVO> visitCount();
}
