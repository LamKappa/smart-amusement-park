package com.chinasoft.backend.service.visitsubscribe;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.exception.BusinessException;
import com.chinasoft.backend.model.entity.Subscribe;
import com.chinasoft.backend.model.request.visitsubscribe.VisitAndSubscribeAddRequest;

/**
 * 针对表【subscribe(用户订阅记录表)】的数据库操作Service
 *
 * @author 姜堂蕴之
 */
public interface SubscribeService extends IService<Subscribe> {
    /**
     * 根据用户手机号、设施id和设施类别进行订阅操作
     *
     * @param visitAndSubscribeAddRequest 包含用户手机号、设施id和设施类别的订阅请求对象
     * @return 包含新增订阅信息的Subscribe对象的BaseResponse响应
     * @throws BusinessException 如果请求对象为null，则抛出参数错误的业务异常
     */
    Subscribe addSubscribe(VisitAndSubscribeAddRequest visitAndSubscribeAddRequest);

}
