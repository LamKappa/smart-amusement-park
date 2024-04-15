package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.config.mqtt.MqttProperties;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.mapper.TotalHeadcountMapper;
import com.chinasoft.backend.model.entity.*;
import com.chinasoft.backend.model.vo.FacilityHeadCountVO;
import com.chinasoft.backend.mqtt.sendclient.Send_Client1;
import com.chinasoft.backend.service.*;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.data.redis.core.ValueOperations;
import org.springframework.stereotype.Service;
import org.springframework.util.CollectionUtils;

import java.util.*;
import java.util.stream.Collectors;

@Service
@Slf4j
public class MqttServiceImpl implements MqttService {

    /**
     * 暂存从IoT接收的游乐设施数据
     */
    private Map<Integer, List<IoTData>> amusementFacilityDataMap = new HashMap<>();

    /**
     * 暂存从IoT接收的餐厅数据
     */
    private Map<Integer, List<IoTData>> restaurantFacilityDataMap = new HashMap<>();

    /**
     * 暂存从IoT接收的基础设施数据
     */
    private Map<Integer, List<IoTData>> baseFacilityDataMap = new HashMap<>();

    /**
     * 每个硬件设备的检测范围
     */
    private static final Integer PER_DEVICE_DETECTION_LENGTH = 2;

    /**
     * 平均每个人的宽度
     */
    private static final Double PER_PERSON_LENGTH = 0.25;

    /**
     * 暂存游乐设施的总人数
     */
    private Integer totalCount = 0;
    private static final String TOTAL_COUNT_KEY = "totalCount";

    /**
     * 暂存各个设施的游玩人数
     */
    private Map<Integer, Integer> amusementFacilityHeadCountMap = new HashMap<Integer, Integer>();

    private String getRedisKey(Integer facilityId) {
        return "facility_head_count:" + facilityId;
    }

    // 定义阈值和时间
    final int THRESHOLD_LOW = 0;
    final int THRESHOLD_MEDIUM = 30;
    final int THRESHOLD_HIGH = 60;
    int night_threshold_hour = 18; // 假设夜晚开始时间为18点

    @Autowired
    AmusementFacilityService amusementFacilityService;

    @Autowired
    RestaurantFacilityService restaurantFacilityService;

    @Autowired
    BaseFacilityService baseFacilityService;

    @Autowired
    CrowdingLevelService crowdingLevelService;

    @Autowired
    TotalHeadcountMapper totalHeadcountMapper;

    @Autowired
    AmusementFacilityMapper amusementFacilityMapper;

    @Autowired
    FacilityHeadcountService facilityHeadcountService;

    @Autowired
    private RedisTemplate redisTemplate;

    @Autowired
    private Send_Client1 client1;

    @Autowired
    MqttProperties mqttProperties;

    @Autowired
    private CrowdingLevelMapper crowdingLevelMapper;

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

        if (facilityType == FacilityTypeConstant.AMUSEMENT_TYPE) {
            if (!amusementFacilityDataMap.containsKey(facilityId)) {
                amusementFacilityDataMap.put(facilityId, new ArrayList<>());
            }
            if (!amusementFacilityHeadCountMap.containsKey(facilityId)) {
                amusementFacilityHeadCountMap.put(facilityId, 0);
            }
            List<IoTData> ioTDataList = amusementFacilityDataMap.get(facilityId);
            ioTDataList.add(ioTData);

            if(deviceId == 0){
                String key = getRedisKey(facilityId);
                ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
                if (operations.get(key) == null) {
                    operations.set(key, 0);
                }
                // 更新facilityId在amusementFacilityHeadCountMap中的计数
                if(detection == 1){
//                int currentCount = amusementFacilityHeadCountMap.get(facilityId);
//                amusementFacilityHeadCountMap.put(facilityId, currentCount + 1);
                    Integer currentCount = operations.get(key);
                    currentCount++;
                    operations.set(key, currentCount);
                }
            }
        } else if (facilityType == FacilityTypeConstant.RESTAURANT_TYPE) {
            if (!restaurantFacilityDataMap.containsKey(facilityId)) {
                restaurantFacilityDataMap.put(facilityId, new ArrayList<>());
            }
            List<IoTData> ioTDataList = restaurantFacilityDataMap.get(facilityId);
            ioTDataList.add(ioTData);
        } else if (facilityType == FacilityTypeConstant.BASE_TYPE) {
            if (!baseFacilityDataMap.containsKey(facilityId)) {
                baseFacilityDataMap.put(facilityId, new ArrayList<>());
            }
            List<IoTData> ioTDataList = baseFacilityDataMap.get(facilityId);
            ioTDataList.add(ioTData);
        }else if(facilityType == FacilityTypeConstant.GATE_TYPE){
            // 统计总游玩人数
//            if(detection == 1){
//                totalCount++;
//            }
            ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
            if (getTotalCountFromRedis() == null) {
                operations.set(TOTAL_COUNT_KEY, 0);
            }
            if (detection == 1) {
                Integer currentTotalCount = getTotalCountFromRedis();
                currentTotalCount++;
                operations.set(TOTAL_COUNT_KEY, currentTotalCount);
            }
        }

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
        // 根据不同的设施获取预期等待时间对象的列表

        // Map<Integer, List<IoTData>>
        if (!CollectionUtils.isEmpty(amusementFacilityDataMap.keySet())) {
            crowdingLevelList.addAll(getCrowdingLevelList(amusementFacilityDataMap, FacilityTypeConstant.AMUSEMENT_TYPE));
        }

        if (!CollectionUtils.isEmpty(restaurantFacilityDataMap.keySet())) {
            crowdingLevelList.addAll(getCrowdingLevelList(restaurantFacilityDataMap, FacilityTypeConstant.RESTAURANT_TYPE));
        }
        if (!CollectionUtils.isEmpty(baseFacilityDataMap.keySet())) {
            crowdingLevelList.addAll(getCrowdingLevelList(baseFacilityDataMap, FacilityTypeConstant.BASE_TYPE));
        }

        // 清空map中暂存的数据
        amusementFacilityDataMap = new HashMap<>();
        restaurantFacilityDataMap = new HashMap<>();
        baseFacilityDataMap = new HashMap<>();

        // 批量插入减少数据库压力
        crowdingLevelService.saveBatch(crowdingLevelList);
    }

    /**
     * 根据不同的设施获取预期等待时间对象的列表
     */
    private List<CrowdingLevel> getCrowdingLevelList(Map<Integer, List<IoTData>> map, Integer facilityType) {

        List<CrowdingLevel> crowdingLevelList = new ArrayList<>();

        for (Integer facilityId : map.keySet()) {
            List<IoTData> dataList = map.get(facilityId);
            // 获取每一个device的detectionList  <deviceId, detectionList>
            Map<Integer, List<Integer>> deviceDetectionMap = new HashMap<>();

            for (IoTData data : dataList) {
                List<Integer> detections = deviceDetectionMap.getOrDefault(data.getDeviceId(), new ArrayList<>());
                detections.add(data.getDetection());
                deviceDetectionMap.put(data.getDeviceId(), detections);
            }

            // 根据这段时间内0多还是1多，来判断这段时间内是有人还是没人
            // 最后处理成一个设备对应一个detection <deviceId, detection>
            Map<Integer, Integer> handledDeviceDetectionMap = new HashMap<>();
            for (Integer deviceId : deviceDetectionMap.keySet()) {
                int[] count = new int[2];
                List<Integer> detections = deviceDetectionMap.get(deviceId);
                // 根据5分钟之内的0多还是1多来进行判断
                for (Integer detection : detections) {
                    count[detection]++;
                }
                // 0的信号是1的信号的两倍，才会认为该时间段是有人的
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
            // 排队人数统计
            int peopleCount = (int) (queueLength / PER_PERSON_LENGTH);

            Integer expectTime = 0;
            Integer expectWaitTime = 0;

            // 根据不同的设施类型来查询每个设施的一次预期使用时间
            if (Objects.equals(facilityType, FacilityTypeConstant.AMUSEMENT_TYPE)) {
                AmusementFacility amusementFacility = amusementFacilityService.getById(facilityId);
                Integer perUserCount = amusementFacility.getPerUserCount();
                expectTime = amusementFacility.getExpectTime();
                // 预期等待时间 = 预期排队时间 + 预计一次游玩时间
                expectWaitTime = expectTime;
                expectWaitTime += peopleCount / perUserCount * expectTime;
            } else if (Objects.equals(facilityType, FacilityTypeConstant.RESTAURANT_TYPE)) {
                if (peopleCount == 0) {
                    expectWaitTime = 0;
                } else {
                    RestaurantFacility restaurantFacility = restaurantFacilityService.getById(facilityId);
                    expectTime = restaurantFacility.getExpectTime();
                    Integer maxCapacity = restaurantFacility.getMaxCapacity();
                    // 伽马分布
                    // 每个人预计用餐时间10分钟，餐厅最大容纳20个人用餐，现在有5个人在排队，求第六个人预计等待时间
                    // 每分钟每个人出来的概率是1/10，那么每分钟预计出来两个人 20 * (1 / 10) = 2个人，那么第六个人要等 6 / 2 = 3分钟
                    // peopleCount / (maxCapacity * (1 / expectTime) )
                    expectWaitTime = (int) (peopleCount / (maxCapacity * 1.0 / expectTime));
                }
            } else if (facilityType == FacilityTypeConstant.BASE_TYPE) {
                BaseFacility baseFacility = baseFacilityService.getById(facilityId);
                expectTime = baseFacility.getExpectTime();
                Integer maxCapacity = baseFacility.getMaxCapacity();
                // 伽马分布
                // 每个人预计用餐时间10分钟，餐厅最大容纳20个人用餐，现在有5个人在排队，求第六个人预计等待时间
                // 每分钟每个人出来的概率是1/10，那么每分钟预计出来两个人 20 * (1 / 10) = 2个人，那么第六个人要等 6 / 2 = 3分钟
                // peopleCount / (maxCapacity * (1 / expectTime) )
                expectWaitTime = (int) (peopleCount / (maxCapacity * 1.0 / expectTime));
            }

            // 将预计等待时间插入数据库
            CrowdingLevel crowdingLevel = new CrowdingLevel();
            crowdingLevel.setFacilityId(Long.valueOf(facilityId));
            crowdingLevel.setFacilityType(facilityType);
            crowdingLevel.setExpectWaitTime(expectWaitTime);
            crowdingLevelList.add(crowdingLevel);
        }
        return crowdingLevelList;
    }

    /**
     * 存储总游玩人数
     */
//    public void handleTotalHeadCount(){
//        TotalHeadcount totalHeadcount = new TotalHeadcount();
//        totalHeadcount.setCount(totalCount);
//        totalHeadcountMapper.insert(totalHeadcount);
//        if(totalHeadcount.getId() > 0){
//            totalCount = 0;
//        }else{
//            log.info("定时任务2执行失败：{}，统计值为：{}", new Date(), totalCount);
//        }
//    }
    public void handleTotalHeadCount(){
        Integer totalCountFromRedis = getTotalCountFromRedis();
        TotalHeadcount totalHeadcount = new TotalHeadcount();
        if (totalCountFromRedis == null){
            totalHeadcount.setCount(0);
        }else{
            totalHeadcount.setCount(totalCountFromRedis);
        }
        totalHeadcountMapper.insert(totalHeadcount);
        if (totalHeadcount.getId() > 0) {
            // 如果插入成功，将Redis中的totalCount重置为0
            ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
            operations.set(TOTAL_COUNT_KEY, 0);
        } else {
            log.info("定时任务2执行失败：{}，统计值为：{}", new Date(), totalCountFromRedis);
        }
    }

    /**
     * 存储各个设施的游玩人数
     */
//    public void handleFacilityHeadCount() {
//        List<FacilityHeadcount> facilityHeadcountList = new ArrayList<>();
//        for (Map.Entry<Integer, Integer> entry : amusementFacilityHeadCountMap.entrySet()) {
//            Integer facilityId = entry.getKey();
//            int headCount = entry.getValue();
//
//            FacilityHeadcount facilityHeadcount = new FacilityHeadcount();
//            facilityHeadcount.setFacilityId(Long.valueOf(facilityId));
//            facilityHeadcount.setCount(headCount);
//
//            facilityHeadcountList.add(facilityHeadcount);
//        }
//
//        boolean isSuccess = facilityHeadcountService.saveBatch(facilityHeadcountList);
//        if(isSuccess){
//            amusementFacilityHeadCountMap = new HashMap<>();
//        }else{
//            log.info("定时任务3执行失败：{}", new Date());
//        }
//    }

    public void handleFacilityHeadCount() {
        // 从数据库中获取所有设施ID
        List<Integer> facilityIds = amusementFacilityMapper.selectAllFacilityIds();

        // 从Redis中获取每个设施的计数
        List<FacilityHeadcount> facilityHeadcountList = facilityIds.stream()
                .map(facilityId -> {
                    String key = getRedisKey(facilityId);
                    Integer count = (Integer) redisTemplate.opsForValue().get(key);
                    FacilityHeadcount facilityHeadcount = new FacilityHeadcount();
                    facilityHeadcount.setFacilityId(Long.valueOf(facilityId));

                    // 如果Redis中没有计数，count将为null，你可能需要处理这种情况
                    if (count == null) {
                        facilityHeadcount.setCount(0);
                    }else{
                        facilityHeadcount.setCount(count);
                    }

                    return facilityHeadcount;
                })
                .collect(Collectors.toList());

        boolean isSuccess = facilityHeadcountService.saveBatch(facilityHeadcountList);
        if(isSuccess){
            for (Integer facilityId : facilityIds) {
                redisTemplate.opsForValue().set(getRedisKey(facilityId), 0);
            }
        }else{
            log.info("定时任务3执行失败：{}", new Date());
        }
    }

    /**
     * 返回总游玩人数
     */
    @Override
//    public Integer getTotalCount() {
//        return totalCount;
//    }

    public Integer getTotalCountFromRedis() {
        ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
        return operations.get(TOTAL_COUNT_KEY);
    }

    /**
     * 返回总游玩人数
     */
//    @Override
//    public  List<FacilityHeadCountVO> getFacilityCount() {
//        List<FacilityHeadCountVO> facilityHeadCountList = new ArrayList<>();
//        for (Map.Entry<Integer, Integer> entry : amusementFacilityHeadCountMap.entrySet()) {
//            Integer facilityId = entry.getKey();
//            int headCount = entry.getValue();
//
//            AmusementFacility amusementFacility = amusementFacilityMapper.selectById(facilityId);
//
//            FacilityHeadCountVO facilityHeadCountVO = new FacilityHeadCountVO();
//            facilityHeadCountVO.setFacilityId(Long.valueOf(facilityId));
//            facilityHeadCountVO.setHeadCount(headCount);
//            facilityHeadCountVO.setFacilityName(amusementFacility.getName());
//
//            facilityHeadCountList.add(facilityHeadCountVO);
//        }
//
//        return facilityHeadCountList;
//    }

    @Override
    public List<FacilityHeadCountVO> getFacilityCount() {
        List<FacilityHeadCountVO> facilityHeadCountList = new ArrayList<>();
        // 获取所有设施的ID列表
        List<Integer> facilityIds = amusementFacilityMapper.selectAllFacilityIds();

        for (Integer facilityId : facilityIds) {
            String key = getRedisKey(facilityId);
            ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
            Integer headCount = operations.get(key);

            if (headCount != null && headCount != 0) {
                AmusementFacility amusementFacility = amusementFacilityMapper.selectById(facilityId);

                FacilityHeadCountVO facilityHeadCountVO = new FacilityHeadCountVO();
                facilityHeadCountVO.setFacilityId(Long.valueOf(facilityId));
                facilityHeadCountVO.setHeadCount(headCount);
                facilityHeadCountVO.setFacilityName(amusementFacility.getName());

                facilityHeadCountList.add(facilityHeadCountVO);
            }
        }

        return facilityHeadCountList;
    }

    @Override
    public void monitor() {
        // 获取当前时间
        Calendar current_time = Calendar.getInstance();
        int current_hour = current_time.get(Calendar.HOUR_OF_DAY);

        // 获取所有设施的id列表
        List<Integer> facilityIds = amusementFacilityMapper.selectAllFacilityIds();

        // 存放设施id和最新排队时间
        Map<Long, Integer> latestExpectWaitTimes = new HashMap<>();

        // 遍历每个facility_id
        for (Integer facilityId : facilityIds) {
            QueryWrapper<CrowdingLevel> queryWrapper = Wrappers.<CrowdingLevel>query()
                    .eq("facility_type", FacilityTypeConstant.AMUSEMENT_TYPE)
                    .eq("facility_id", facilityId)
                    .orderByDesc("create_time"); // 按照create_time降序排序

            // 获取最新的记录
            CrowdingLevel latestCrowdingLevel = crowdingLevelMapper.selectList(queryWrapper).get(0);

            if (latestCrowdingLevel != null) {
                latestExpectWaitTimes.put(latestCrowdingLevel.getFacilityId(), latestCrowdingLevel.getExpectWaitTime());
            }
        }

        // 遍历获取到的最新expect_wait_time
        for (Map.Entry<Long, Integer> entry : latestExpectWaitTimes.entrySet()) {
            Long facilityId = entry.getKey();
            Integer expectWaitTime = entry.getValue();

            // 初始化消息值
            Integer music = 0;
            Integer light = 0;

            // 根据拥挤度设置music
            if (expectWaitTime > THRESHOLD_LOW && expectWaitTime < THRESHOLD_MEDIUM) {
                music = 1;
            } else if (expectWaitTime >= THRESHOLD_MEDIUM && expectWaitTime < THRESHOLD_HIGH) {
                music = 2;
            } else if (expectWaitTime >= THRESHOLD_HIGH) {
                music = 3;
            }

            // 如果是夜晚并且需要设置light
            if (current_hour >= night_threshold_hour) {
                // 根据拥挤度设置light
                if (expectWaitTime > THRESHOLD_LOW && expectWaitTime < THRESHOLD_MEDIUM) {
                    light = 1;
                } else if (expectWaitTime >= THRESHOLD_MEDIUM && expectWaitTime < THRESHOLD_HIGH) {
                    light = 2;
                } else if (expectWaitTime >= THRESHOLD_HIGH) {
                    light = 3;
                }
            }

            // 设置MQTT主题
            String musicTopic = "M/" + facilityId;
            String lightTopic = "L/" + facilityId;

            // 发布消息
            try {
                client1.publish(false, musicTopic, String.valueOf(music));
                client1.publish(false, lightTopic, String.valueOf(light));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
