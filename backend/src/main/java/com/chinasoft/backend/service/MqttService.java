package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.IoTData;

public interface MqttService {

    void saveIoTData(IoTData ioTData);

    void handleIoTData();

    void addTotalCount(IoTData ioTData);

    void handleTotalHead();

    Integer getTotalCount();

    Integer getFacilityCount(Integer facilityId);
}
