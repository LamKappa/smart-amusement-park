package com.chinasoft.backend.service.visitsubscribe;

import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.entity.Subscribe;
import com.chinasoft.backend.model.request.visitsubscribe.VisitAndSubscribeAddRequest;

/**
 * @author 姜堂蕴之
 * @description 针对表【subscribe(用户订阅记录表)】的数据库操作Service
 * @createDate 2024-04-09 11:20:51
 */
public interface SubscribeService extends IService<Subscribe> {

    Subscribe addSubscribe(VisitAndSubscribeAddRequest visitAndSubscribeRequest);

}
