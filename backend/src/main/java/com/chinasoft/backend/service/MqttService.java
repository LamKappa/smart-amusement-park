package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.FacilityHeadCount;
import com.chinasoft.backend.model.entity.IoTData;

import java.util.List;

public interface MqttService {

    void saveIoTData(IoTData ioTData);

    void handleIoTData();

    void addTotalCount(IoTData ioTData);

    void handleTotalHead();

    Integer getTotalCount();

    List<FacilityHeadCount> getFacilityCount();
}
