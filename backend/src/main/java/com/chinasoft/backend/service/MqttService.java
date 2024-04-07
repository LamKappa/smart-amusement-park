package com.chinasoft.backend.service;

import com.chinasoft.backend.model.entity.IoTData;

public interface MqttService {

    void saveIoTData(IoTData ioTData);

    void handleIoTData();
}
