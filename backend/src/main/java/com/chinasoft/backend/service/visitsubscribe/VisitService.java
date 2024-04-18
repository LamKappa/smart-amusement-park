package com.chinasoft.backend.service.visitsubscribe;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.exception.BusinessException;
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
    /**
     * 添加用户打卡记录
     *
     * @param visitAndSubscribeAddRequest 包含用户id、设施id和设施类别的打卡请求对象
     * @return 包含新增打卡记录的Visit对象的BaseResponse响应
     * @throws BusinessException 如果请求对象为null，则抛出参数错误的业务异常
     */
    Visit addVisit(VisitAndSubscribeAddRequest visitAndSubscribeAddRequest);

    /**
     * 统计打卡次数
     */
    List<FacilityVisitCountVO> visitCount();
}
