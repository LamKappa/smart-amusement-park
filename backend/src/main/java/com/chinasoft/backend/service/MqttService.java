package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.IoTData;
import com.chinasoft.backend.model.vo.statistic.FacilityHeadCountVO;

import java.util.List;

/**
 * Mqtt Service
 *
 * @author 孟祥硕 姜堂蕴之
 */
public interface MqttService {
    /**
     * 存储硬件传输数据
     *
     * @author 孟祥硕
     */
    void saveIoTData(IoTData ioTData);

    /**
     * 处理硬件传输数据
     *
     * @author 孟祥硕
     */
    void handleIoTData();

    /**
     * 处理总游玩人数
     *
     * @author 姜堂蕴之
     */
    void handleTotalHeadCount();

    /**
     * 处理各设施游玩人数
     *
     * @author 姜堂蕴之
     */
    void handleFacilityHeadCount();

    /**
     * 返回当前总游玩人数
     *
     * @author 姜堂蕴之
     */
    Integer getTotalCountFromRedis();

    /**
     * 返回各个设施当前游玩人数
     *
     * @author 姜堂蕴之
     */
    List<FacilityHeadCountVO> getFacilityCount();

    /**
     * 基于拥挤度监测音乐调节
     *
     * @author 姜堂蕴之
     */
    void monitorMusic();

    /**
     * 基于拥挤度监测灯光调节
     *
     * @author 姜堂蕴之
     */
    void monitorLight();
}
