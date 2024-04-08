package com.chinasoft.backend.service.impl;

import com.chinasoft.backend.model.entity.AmusementFacility;
import com.chinasoft.backend.model.entity.CrowdingLevel;
import com.chinasoft.backend.model.entity.IoTData;
import com.chinasoft.backend.service.AmusementFacilityService;
import com.chinasoft.backend.service.CrowdingLevelService;
import com.chinasoft.backend.service.MqttService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

@Service
@Slf4j
public class MqttServiceImpl implements MqttService {

    /**
     * 暂存从IoT接收的数据
     */
    private Map<Integer, List<IoTData>> map = new HashMap<>();

    private static final Integer PER_DEVICE_DETECTION_LENGTH = 1;
    private static final Double PER_PERSON_LENGTH = 0.5;

    @Autowired
    AmusementFacilityService amusementFacilityService;

    @Autowired
    CrowdingLevelService crowdingLevelService;

    /**
     * 暂存IoT的数据
     */
    @Override
    public void saveIoTData(IoTData ioTData) {
        // Map<Integer, List<IoTData>> map = IoTDataMapContext.getMap();

        Integer deviceId = ioTData.getDeviceId();
        Integer facilityId = ioTData.getFacilityId();
        Integer facilityType = ioTData.getFacilityType();
        Integer detection = ioTData.getDetection();

        if (!map.containsKey(facilityId)) {
            map.put(facilityId, new ArrayList<>());
        }
        List<IoTData> ioTDataList = map.get(facilityId);
        ioTDataList.add(ioTData);

    }

    /**
     * 每五分钟处理一次暂存的IoT数据
     */
    @Override
    public void handleIoTData() {
        // Integer deviceId = ioTData.getDeviceId();
        // Integer facilityId = ioTData.getFacilityId();
        // Integer facilityType = ioTData.getFacilityType();
        // Integer detection = ioTData.getDetection();


        // 获取每一个设施五分钟的数据，并计算预计等待时间和存储到数据库
        List<CrowdingLevel> crowdingLevelList = new ArrayList<>();
        for (Integer facilityId : map.keySet()) {
            List<IoTData> dataList = map.get(facilityId);
            // 获取每一个device的detectionList
            Map<Integer, List<Integer>> deviceDetectionMap = new HashMap<>();

            for (IoTData data : dataList) {
                List<Integer> detections = deviceDetectionMap.getOrDefault(data.getDeviceId(), new ArrayList<>());
                detections.add(data.getDetection());
                deviceDetectionMap.put(data.getDeviceId(), detections);
            }

            // 处理成最后<deviceId, detection>
            Map<Integer, Integer> handledDeviceDetectionMap = new HashMap<>();
            for (Integer deviceId : deviceDetectionMap.keySet()) {
                int[] count = new int[2];
                List<Integer> detections = deviceDetectionMap.get(deviceId);
                // 根据5分钟之内的0多还是1多来进行判断
                for (Integer detection : detections) {
                    count[detection]++;
                }
                if (count[0] > count[1] * 2) {
                    handledDeviceDetectionMap.put(deviceId, 0);
                } else {
                    handledDeviceDetectionMap.put(deviceId, 1);
                }
            }

            // 根据每个设备检测的是否有人来进行计算预计等待时间
            //（队列的长度（通过传感器算出） / 一个人的长度 ）/ 每个设施游玩人数 * 每个设施游玩时长
            int queueLength = 0;
            for (Integer detection : handledDeviceDetectionMap.values()) {
                if (detection == 1) {
                    queueLength += PER_DEVICE_DETECTION_LENGTH;
                }
            }
            int peopleCount = (int) (queueLength / PER_PERSON_LENGTH);

            AmusementFacility amusementFacility = amusementFacilityService.getById(facilityId);
            Integer perUserCount = amusementFacility.getPerUserCount();
            Integer expectTime = amusementFacility.getExpectTime();

            Integer expectWaitTime = peopleCount / perUserCount * expectTime;
            if (expectWaitTime == 0) {
                expectWaitTime = expectTime;
            }
            // 将预计等待时间插入数据库
            CrowdingLevel crowdingLevel = new CrowdingLevel();
            crowdingLevel.setFacilityId(Long.valueOf(facilityId));
            crowdingLevel.setFacilityType(0);
            crowdingLevel.setExpectWaitTime(expectWaitTime);
            crowdingLevelList.add(crowdingLevel);
        }
        // 批量插入减少数据库压力
        crowdingLevelService.saveBatch(crowdingLevelList);

        // 清空map中暂存的数据
        map = new HashMap<>();
    }

}
