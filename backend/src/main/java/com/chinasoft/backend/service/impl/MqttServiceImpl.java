package com.chinasoft.backend.service.impl;

import com.baomidou.mybatisplus.core.conditions.query.QueryWrapper;
import com.baomidou.mybatisplus.core.toolkit.Wrappers;
import com.chinasoft.backend.config.mqtt.MqttProperties;
import com.chinasoft.backend.constant.CrowdingLevelConstant;
import com.chinasoft.backend.constant.FacilityTypeConstant;
import com.chinasoft.backend.mapper.AmusementFacilityMapper;
import com.chinasoft.backend.mapper.CrowdingLevelMapper;
import com.chinasoft.backend.mapper.TotalHeadcountMapper;
import com.chinasoft.backend.model.entity.CrowdingLevel;
import com.chinasoft.backend.model.entity.IoTData;
import com.chinasoft.backend.model.entity.TotalHeadcount;
import com.chinasoft.backend.model.entity.facility.AmusementFacility;
import com.chinasoft.backend.model.entity.facility.BaseFacility;
import com.chinasoft.backend.model.entity.facility.FacilityHeadcount;
import com.chinasoft.backend.model.entity.facility.RestaurantFacility;
import com.chinasoft.backend.model.vo.statistic.FacilityHeadCountVO;
import com.chinasoft.backend.service.mqtt.sendclient.Send_Client1;
import com.chinasoft.backend.service.MqttService;
import com.chinasoft.backend.service.facility.AmusementFacilityService;
import com.chinasoft.backend.service.facility.BaseFacilityService;
import com.chinasoft.backend.service.facility.CrowdingLevelService;
import com.chinasoft.backend.service.facility.RestaurantFacilityService;
import com.chinasoft.backend.service.statistic.FacilityHeadcountService;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.data.redis.core.RedisTemplate;
import org.springframework.data.redis.core.ValueOperations;
import org.springframework.stereotype.Service;
import org.springframework.util.CollectionUtils;

import java.util.*;
import java.util.stream.Collectors;

/**
 * MQTT相关Service实现
 *
 * @author 孟祥硕 姜堂蕴之
 */
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
     * 游乐设施总人数在Redis中的对应Key值
     */
    private static final String TOTAL_COUNT_KEY = "totalCount";

    /**
     * 基于拥挤度的音乐、灯光调节阈值
     */
    final int THRESHOLD_LOW = 0;
    final int THRESHOLD_MEDIUM = 30;
    final int THRESHOLD_HIGH = 60;

    /**
     * 预设的灯光开启时间点
     */
    int night_threshold_hour = 17;

    /**
     * MQTT音乐调节主题前缀
     */
    public static final String MUSIC_TOPIC_PREFIX = "M/";

    /**
     * MQTT灯光调节主题前缀
     */
    public static final String LIGHT_TOPIC_PREFIX = "L/";

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
     * 根据设施ID生成Redis中的统一Key值
     */
    private String getRedisKey(Integer facilityId) {
        return "facility_head_count:" + facilityId;
    }

    /**
     * 暂存IoT的数据
     *
     * @author 孟祥硕 姜堂蕴之
     */
    @Override
    public void saveIoTData(IoTData ioTData) {
        // 读取硬件传输数据
        Integer deviceId = ioTData.getDeviceId();
        Integer facilityId = ioTData.getFacilityId();
        Integer facilityType = ioTData.getFacilityType();
        Integer detection = ioTData.getDetection();

        if (facilityType == FacilityTypeConstant.AMUSEMENT_TYPE) { // 如果是游乐设施
            // 初始化游乐设施数据列表
            if (!amusementFacilityDataMap.containsKey(facilityId)) {
                amusementFacilityDataMap.put(facilityId, new ArrayList<>());
            }

            // 存储拥挤度数据
            List<IoTData> ioTDataList = amusementFacilityDataMap.get(facilityId);
            ioTDataList.add(ioTData);

            if (deviceId == 0) { // 如果数据传输硬件设备为大门处传感器
                // 获取Redis键值
                String key = getRedisKey(facilityId);
                ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
                if (operations.get(key) == null) {
                    // 若Redis中不存在该键值，则初始化为0
                    operations.set(key, 0);
                }
                // 更新Redis中的计数
                if (detection == 1) {
                    // 如果检测到人员进入，则增加计数
                    Integer currentCount = operations.get(key);
                    currentCount++;
                    operations.set(key, currentCount);
                }
            }
        } else if (facilityType == FacilityTypeConstant.RESTAURANT_TYPE) { // 如果是餐饮设施
            // 初始化餐饮设施数据列表
            if (!restaurantFacilityDataMap.containsKey(facilityId)) {
                restaurantFacilityDataMap.put(facilityId, new ArrayList<>());
            }
            // 存储餐饮设施数据
            List<IoTData> ioTDataList = restaurantFacilityDataMap.get(facilityId);
            ioTDataList.add(ioTData);
        } else if (facilityType == FacilityTypeConstant.BASE_TYPE) { // 如果是基础设施
            // 初始化基础设施数据列表
            if (!baseFacilityDataMap.containsKey(facilityId)) {
                baseFacilityDataMap.put(facilityId, new ArrayList<>());
            }
            // 存储基础设施数据
            List<IoTData> ioTDataList = baseFacilityDataMap.get(facilityId);
            ioTDataList.add(ioTData);
        } else if (facilityType == FacilityTypeConstant.GATE_TYPE) { // 如果是游乐园大门
            ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
            if (getTotalCountFromRedis() == null) {
                // 若Redis中不存在总计数键值，则初始化为0
                operations.set(TOTAL_COUNT_KEY, 0);
            }
            if (detection == 1) {
                // 如果检测到人员进入，则增加总计数
                Integer currentTotalCount = getTotalCountFromRedis();
                currentTotalCount++;
                operations.set(TOTAL_COUNT_KEY, currentTotalCount);
            }
        }
    }


    /**
     * 每五分钟处理一次暂存的IoT数据
     *
     * @author 孟祥硕
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
     *
     * @author 孟祥硕
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
     * 存储成功后将Redis中的计数重置为0
     *
     * @author 姜堂蕴之
     */
    public void handleTotalHeadCount() {
        // 从Redis中获取总游玩人数
        Integer totalCountFromRedis = getTotalCountFromRedis();

        // 创建新的TotalHeadcount对象
        TotalHeadcount totalHeadcount = new TotalHeadcount();

        // 判断从Redis中获取的总游玩人数是否为空
        if (totalCountFromRedis == null) {
            totalHeadcount.setCount(0);
        } else {
            totalHeadcount.setCount(totalCountFromRedis);
        }

        // 将总游玩人数信息插入数据库
        totalHeadcountMapper.insert(totalHeadcount);

        // 判断插入是否成功
        if (totalHeadcount.getId() > 0) {

            ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
            // 将Redis中的totalCount重置为0
            operations.set(TOTAL_COUNT_KEY, 0);
        } else {
            log.info("记录总人数定时任务执行失败：{}", new Date());
        }
    }

    /**
     * 存储各个设施的游玩人数
     * 存储成功后将Redis中的计数重置为0
     *
     * @author 姜堂蕴之
     */
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

                    // 如果Redis中没有计数，count设置为0
                    if (count == null) {
                        facilityHeadcount.setCount(0);
                    } else {
                        facilityHeadcount.setCount(count);
                    }

                    return facilityHeadcount;
                })
                .collect(Collectors.toList());

        // 将各个设施游玩人数数据存储入数据库
        boolean isSuccess = facilityHeadcountService.saveBatch(facilityHeadcountList);

        // 判断插入是否成功
        if (isSuccess) {
            for (Integer facilityId : facilityIds) {
                redisTemplate.opsForValue().set(getRedisKey(facilityId), 0);
            }
        } else {
            log.info("记录各设施游玩人数定时任务执行失败：{}", new Date());
        }
    }

    /**
     * 返回当前总游玩人数
     *
     * @author 姜堂蕴之
     */
    @Override
    public Integer getTotalCountFromRedis() {
        ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
        return operations.get(TOTAL_COUNT_KEY);
    }

    /**
     * 返回当前各个设施游玩人数
     *
     * @author 姜堂蕴之
     */
    @Override
    public List<FacilityHeadCountVO> getFacilityCount() {
        // 初始化一个设施游玩人数VO列表
        List<FacilityHeadCountVO> facilityHeadCountList = new ArrayList<>();

        // 获取所有设施的ID列表
        List<Integer> facilityIds = amusementFacilityMapper.selectAllFacilityIds();

        // 遍历每个设施的ID
        for (Integer facilityId : facilityIds) {
            // 根据设施ID生成Redis中的key
            String key = getRedisKey(facilityId);

            // 从Redis中获取该设施的游玩人数
            ValueOperations<String, Integer> operations = redisTemplate.opsForValue();
            Integer headCount = operations.get(key);

            // 判断游玩人数非空且不为0
            if (headCount != null && headCount != 0) {
                //  将该设施的基础信息添加至返回列表中
                AmusementFacility amusementFacility = amusementFacilityMapper.selectById(facilityId);
                FacilityHeadCountVO facilityHeadCountVO = new FacilityHeadCountVO();
                facilityHeadCountVO.setFacilityId(Long.valueOf(facilityId));
                facilityHeadCountVO.setHeadCount(headCount);
                facilityHeadCountVO.setFacilityName(amusementFacility.getName());

                facilityHeadCountList.add(facilityHeadCountVO);
            }
        }

        // 返回设施游玩人数VO列表
        return facilityHeadCountList;
    }

    /**
     * 监控音乐播放器
     * 通过检测设施的最新排队时间，为设施选择适当的音乐播放模式。
     * 音乐播放模式说明：
     *  - 拥挤度1档（0-30）：激情音乐，适用于低拥挤度的设施排队区域。
     *  - 拥挤度2档（30-60）：轻快音乐，适用于中等拥挤度的设施排队区域。
     *  - 拥挤度3档（60以上）：舒缓音乐，适用于高拥挤度的设施排队区域。
     *
     * @author 姜堂蕴之
     */
    @Override
    public void monitorMusic() {
        // 获取所有设施的ID列表
        List<Integer> facilityIds = amusementFacilityMapper.selectAllFacilityIds();

        // 存放设施ID和最新排队时间的映射关系
        Map<Long, Integer> latestExpectWaitTimes = new HashMap<>();

        // 遍历每个设施ID
        for (Integer facilityId : facilityIds) {
            // 构建查询条件
            QueryWrapper<CrowdingLevel> queryWrapper = Wrappers.<CrowdingLevel>query()
                    .eq("facility_type", FacilityTypeConstant.AMUSEMENT_TYPE)
                    .eq("facility_id", facilityId)
                    .orderByDesc("create_time"); // 按照create_time降序排序

            // 获取最新的拥挤度记录
            List<CrowdingLevel> crowdingLevelList = crowdingLevelMapper.selectList(queryWrapper);
            CrowdingLevel latestCrowdingLevel = null;
            if (!CollectionUtils.isEmpty(crowdingLevelList)) {
                latestCrowdingLevel = crowdingLevelList.get(0);
            }

            // 存储最新的排队时间
            if (latestCrowdingLevel != null) {
                latestExpectWaitTimes.put(latestCrowdingLevel.getFacilityId(), latestCrowdingLevel.getExpectWaitTime());
            }
        }

        // 遍历获取到的最新排队时间
        for (Map.Entry<Long, Integer> entry : latestExpectWaitTimes.entrySet()) {
            Long facilityId = entry.getKey();
            Integer expectWaitTime = entry.getValue();

            // 初始化音乐模式
            Integer music = 0;

            // 根据排队时间设置音乐模式
            if (expectWaitTime > THRESHOLD_LOW && expectWaitTime < THRESHOLD_MEDIUM) {
                music = CrowdingLevelConstant.LOW; // 1档
            } else if (expectWaitTime >= THRESHOLD_MEDIUM && expectWaitTime < THRESHOLD_HIGH) {
                music = CrowdingLevelConstant.MEDIUM; // 2档
            } else if (expectWaitTime >= THRESHOLD_HIGH) {
                music = CrowdingLevelConstant.HIGHT; // 3档
            }

            // 设置MQTT主题
            String musicTopic = MUSIC_TOPIC_PREFIX + facilityId;

            // 发布音乐模式消息
            try {
                client1.publish(false, musicTopic, String.valueOf(music));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }


    /**
     * 监控灯光控制器
     * 通过检测设施的最新排队时间，为设施选择适当的灯光模式（灯光只在夜晚进行控制）。
     * 灯光模式说明：
     *  - 拥挤度1档（0-30）：柔和灯光，适用于低拥挤度的设施排队区域。
     *  - 拥挤度2档（30-60）：适中灯光，适用于中等拥挤度的设施排队区域。
     *  - 拥挤度3档（60以上）：明亮灯光，适用于高拥挤度的设施排队区域。
     *
     * @author 姜堂蕴之
     */
    @Override
    public void monitorLight() {
        // 获取当前时间
        Calendar current_time = Calendar.getInstance();
        int current_hour = current_time.get(Calendar.HOUR_OF_DAY);

        // 获取所有设施的ID列表
        List<Integer> facilityIds = amusementFacilityMapper.selectAllFacilityIds();

        // 存放设施ID和最新排队时间的映射关系
        Map<Long, Integer> latestExpectWaitTimes = new HashMap<>();

        // 遍历每个设施ID
        for (Integer facilityId : facilityIds) {
            // 构建查询条件
            QueryWrapper<CrowdingLevel> queryWrapper = Wrappers.<CrowdingLevel>query()
                    .eq("facility_type", FacilityTypeConstant.AMUSEMENT_TYPE)
                    .eq("facility_id", facilityId)
                    .orderByDesc("create_time"); // 按照create_time降序排序

            // 获取最新的拥挤度记录
            List<CrowdingLevel> crowdingLevelList = crowdingLevelMapper.selectList(queryWrapper);
            CrowdingLevel latestCrowdingLevel = null;
            if (!CollectionUtils.isEmpty(crowdingLevelList)) {
                latestCrowdingLevel = crowdingLevelList.get(0);
            }

            // 存储最新的排队时间
            if (latestCrowdingLevel != null) {
                latestExpectWaitTimes.put(latestCrowdingLevel.getFacilityId(), latestCrowdingLevel.getExpectWaitTime());
            }
        }

        // 遍历获取到的最新排队时间
        for (Map.Entry<Long, Integer> entry : latestExpectWaitTimes.entrySet()) {
            Long facilityId = entry.getKey();
            Integer expectWaitTime = entry.getValue();

            // 初始化灯光模式
            Integer light = 0;

            // 为了便于项目展示，此处省略时间判断
//            if (current_hour >= night_threshold_hour)

            // 根据排队时间设置音乐模式
            if (expectWaitTime > THRESHOLD_LOW && expectWaitTime < THRESHOLD_MEDIUM) {
                light = CrowdingLevelConstant.LOW; // 1档
            } else if (expectWaitTime >= THRESHOLD_MEDIUM && expectWaitTime < THRESHOLD_HIGH) {
                light = CrowdingLevelConstant.MEDIUM; // 2档
            } else if (expectWaitTime >= THRESHOLD_HIGH) {
                light = CrowdingLevelConstant.HIGHT; // 3档
            }

            // 设置MQTT主题
            String lightTopic = LIGHT_TOPIC_PREFIX + facilityId;

            // 发布灯光模式消息
            try {
                client1.publish(false, lightTopic, String.valueOf(light));
            } catch (Exception e) {
                e.printStackTrace();
            }
        }
    }
}
