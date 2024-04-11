package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.IoTData;
import com.chinasoft.backend.model.vo.FacilityHeadCountVO;

import java.util.List;

public interface MqttService {

    void saveIoTData(IoTData ioTData);

    void handleIoTData();

    void addTotalCount(IoTData ioTData);

    void handleTotalHeadCount();

    void handleFacilityHeadCount();

    Integer getTotalCount();

    List<FacilityHeadCountVO> getFacilityCount();
}
