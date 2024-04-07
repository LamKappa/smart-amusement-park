package com.chinasoft.backend.task;

import com.chinasoft.backend.service.MqttService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.util.Date;

@Slf4j
@Component
public class HandleIoTDataTask {

    @Autowired
    MqttService mqttService;

    // @Scheduled(cron = "* 0/5 * * * ? ")
    @Scheduled(cron = "0/10 * * * * ? ")
    public void handleIoTDataTask() {
        log.info("定时任务开始执行：{}", new Date());

        mqttService.handleIoTData();


    }
}
