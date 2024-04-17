package com.chinasoft.backend.task;

import com.chinasoft.backend.service.MqttService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Component;

import java.util.Date;

/**
 * 定时任务
 *
 * @authro 孟祥硕
 */
@Slf4j
@Component
public class HandleIoTDataTask {

    @Autowired
    MqttService mqttService;

    // @Scheduled(cron = "0 0/5 * * * ? ")
    @Scheduled(cron = "0/10 * * * * ? ")
    public void handleIoTDataTask() {
        log.info("处理硬件传输的拥挤度数据定时任务开始执行：{}", new Date());
        mqttService.handleIoTData();
    }

    /**
     * 检查音乐如何调节
     */
    @Scheduled(cron = "0/6 * * * * ? ")
    // @Scheduled(cron = "0 0/5 * * * ? ")
    public void monitorMusicTask() {
        log.info("检测音乐进行调节定时任务开始执行：{}", new Date());
        mqttService.monitorMusic();
    }

    /**
     * 检查灯光如何调节
     */
    @Scheduled(cron = "0/6 * * * * ? ")
    // @Scheduled(cron = "0/6 * * * * ? ")
    public void monitorLightTask() {
        log.info("检测灯光进行调节定时任务开始执行：{}", new Date());
        mqttService.monitorLight();
    }

    /**
     * 统计总人数
     */
    @Scheduled(cron = "0 59 23 * * ?")
    public void handleTotalHeadTask() {
        log.info("记录总人数定时任务开始执行：{}", new Date());
        mqttService.handleTotalHeadCount();
    }


    @Scheduled(cron = "0 59 23 * * ?")
    public void handleFacilityHeadTask() {
        log.info("记录设施人数定时任务开始执行：{}", new Date());
        mqttService.handleFacilityHeadCount();
    }


}
