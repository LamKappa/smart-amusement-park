package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.extension.service.impl.ServiceImpl;
import com.chinasoft.backend.model.entity.TotalHeadcount;
import com.chinasoft.backend.service.TotalHeadcountService;
import com.chinasoft.backend.mapper.TotalHeadcountMapper;
import org.springframework.stereotype.Service;

/**
* @author 皎皎
* @description 针对表【total_headcount(总人数统计表)】的数据库操作Service实现
* @createDate 2024-04-10 17:56:36
*/
@Service
public class TotalHeadcountServiceImpl extends ServiceImpl<TotalHeadcountMapper, TotalHeadcount>
    implements TotalHeadcountService{
    @Override
    public Integer getTotalCount() {
        return null;
    }

//    private volatile int totalCount = 0; // 使用volatile确保多线程环境下的可见性
//
//    @Autowired
//    private MqttClientService mqttClientService; // 假设你有一个MQTT客户端服务类来处理消息
//
//
//    // 每当MQTT接收到消息时，调用此方法
//    public void onMqttMessageReceived() {
//        totalCount++;
//    }
//
//    // 获取当前总数
//    public Integer getTotalCount() {
//        return totalCount;
//    }
//
//    // 定时任务，每天23:59执行
//    @Scheduled(cron = "0 59 23 * * ?") // 每天23:59:00执行
//    public void updateDatabaseWithTotalCount() {
//        totalHeadcountRepository.save(new TotalHeadcountEntity(totalCount)); // 假设你有一个对应的实体类
//        // 重置totalCount，如果你希望每天从0开始计数
//        totalCount = 0;
//    }
//
//    @Override
//    public Integer getTotalCount() {
//        return null;
//    }
}




