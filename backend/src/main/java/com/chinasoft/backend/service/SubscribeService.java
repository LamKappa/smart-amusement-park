package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.Subscribe;
import com.baomidou.mybatisplus.extension.service.IService;
import com.chinasoft.backend.model.request.VisitAndSubscribeAddRequest;
import com.chinasoft.backend.model.request.VisitAndSubscribeDeleteRequest;

/**
* @author 皎皎
* @description 针对表【subscribe(用户订阅记录表)】的数据库操作Service
* @createDate 2024-04-09 11:20:51
*/
public interface SubscribeService extends IService<Subscribe> {

    Subscribe addSubscribe(VisitAndSubscribeAddRequest visitAndSubscribeRequest);

    Boolean deleteSubscribe(VisitAndSubscribeDeleteRequest visitAndSubscribeDeleteRequest);
}
