package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.IoTData;
import com.chinasoft.backend.model.vo.statistic.FacilityHeadCountVO;

import java.util.List;

/**
 * Mqtt Service
 *
 * @author 孟祥硕
 */
public interface MqttService {

    void saveIoTData(IoTData ioTData);

    void handleIoTData();

    void handleTotalHeadCount();

    void handleFacilityHeadCount();

//    Integer getTotalCount();

    Integer getTotalCountFromRedis();

    List<FacilityHeadCountVO> getFacilityCount();

    void monitorMusic();

    void monitorLight();
}
