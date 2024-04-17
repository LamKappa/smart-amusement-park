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
 * @author 孟祥硕 姜堂蕴之
 */
@Slf4j
@Component
public class HandleIoTDataTask {

    @Autowired
    MqttService mqttService;

    /**
     * 处理硬件传输的拥挤度数据（为便于演示，将5分钟一执行的间隔缩短）
     *
     * @author 孟祥硕
     */
//    @Scheduled(cron = "0 0/5 * * * ? ")
    @Scheduled(cron = "0/10 * * * * ? ")
    public void handleIoTDataTask() {
        log.info("处理硬件传输的拥挤度数据定时任务开始执行：{}", new Date());
        mqttService.handleIoTData();
    }

    /**
     * 基于拥挤度进行音乐调节（为便于演示，将5分钟一执行的间隔缩短）
     *
     * @author 姜堂蕴之
     */
    @Scheduled(cron = "0/6 * * * * ? ")
    public void monitorMusicTask() {
        log.info("检测音乐进行调节定时任务开始执行：{}", new Date());
        mqttService.monitorMusic();
    }

    /**
     * 基于拥挤度进行灯光调节（为便于演示，将5分钟一执行的间隔缩短）
     *
     * @author 姜堂蕴之
     */
    @Scheduled(cron = "0/6 * * * * ? ")
    public void monitorLightTask() {
        log.info("检测灯光进行调节定时任务开始执行：{}", new Date());
        mqttService.monitorLight();
    }

    /**
     * 记录今日游乐园游玩总人数
     *
     * @author 姜堂蕴之
     */
    @Scheduled(cron = "0 59 23 * * ?")
    public void handleTotalHeadTask() {
        log.info("记录总人数定时任务开始执行：{}", new Date());
        mqttService.handleTotalHeadCount();
    }


    /**
     * 记录今日各个设施游玩人数
     *
     * @author 姜堂蕴之
     */
    @Scheduled(cron = "0 59 23 * * ?")
    public void handleFacilityHeadTask() {
        log.info("记录各设施游玩人数定时任务开始执行：{}", new Date());
        mqttService.handleFacilityHeadCount();
    }


}
