/*
 Navicat Premium Data Transfer

 Source Server         : 中软项目组
 Source Server Type    : MySQL
 Source Server Version : 80300 (8.3.0)
 Source Host           : 120.46.211.213:3306
 Source Schema         : chinasoft

 Target Server Type    : MySQL
 Target Server Version : 80300 (8.3.0)
 File Encoding         : 65001

 Date: 03/04/2024 16:04:06
*/

SET NAMES utf8mb4;
SET FOREIGN_KEY_CHECKS = 0;

-- ----------------------------
-- Table structure for amusement_facility
-- ----------------------------
DROP TABLE IF EXISTS `amusement_facility`;
CREATE TABLE `amusement_facility`  (
  `id` bigint NOT NULL AUTO_INCREMENT,
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '名称',
  `introduction` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '介绍',
  `longitude` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '纬度',
  `latitude` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '经度',
  `per_user_count` int NULL DEFAULT NULL COMMENT '一次游玩的人数\n',
  `expect_time` int NULL DEFAULT NULL COMMENT '预计游玩时间（以分钟为单位）',
  `type` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '项目类型 可多选（过山车、轨道、失重、水上、室内、旋转、鬼屋）',
  `crowd_type` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '适合人群（成人、老少皆宜、家长监护）',
  `image_url` varchar(512) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '设施照片',
  `start_time` time NULL DEFAULT NULL COMMENT '开放时间',
  `close_time` time NULL DEFAULT NULL COMMENT '关闭时间',
  `status` int NULL DEFAULT NULL COMMENT '状态 0-正常 1-异常（如果是在检修的时候status为1，注意夜晚闭馆未开放的时候status为0）',
  `create_time` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '添加时间',
  `update_time` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '修改时间',
  `is_deleted` int NULL DEFAULT NULL COMMENT '逻辑删除(0-未删除，1-已删除)',
  `instruction` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '游玩须知',
  `height_low` int NULL DEFAULT NULL COMMENT '身高下限',
  `height_up` int NULL DEFAULT NULL COMMENT '身高上限',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 11 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of amusement_facility
-- ----------------------------
INSERT INTO `amusement_facility` VALUES (1, '重庆之眼', '西南地区最大的摩天轮，达到120米的高度，如在云端，俯瞰大地时的特殊角度，更能启迪思维，别有一番滋味。高空览胜，重庆山城美景，都市万般风情，尽收眼底。和心爱的人一起去享受浪漫甜蜜的氛围，感受童年的乐趣，在绚丽多姿、变幻无穷的彩灯系统点缀下，摩天轮成为欢乐谷最耀眼的明珠。', '29.667114', '106.515024', 112, 15, '旋转', '成人/老少皆宜/家长监护', 'https://imgo2o.51sole.com/0/image/20160926/c5b5148b-e161-4f4d-ba56-2be62116acf2.jpg', '16:30:00', '22:00:00', 1, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '120cm（含120cm）以下须成人陪同', 0, 300);
INSERT INTO `amusement_facility` VALUES (2, '翼飞冲天', '侏罗纪时期的翼龙突然再现地球，摆动着巨大的双翼，穿梭在丛林峡谷之间。勇敢的人类，爬上翼龙的双翼，上下无遮挡，与其一道凌空飞行。伴随着变化莫测的飞行轨迹，翼龙从山谷最高点俯冲直下，又攀升翻滚，人们体验着如同战斗机俯冲时的速度与激情，风驰电掣，呼啸而行。', '29.668583', '106.520137', 36, 5, '过山车/轨道/失重', '成人', 'https://pic.rmb.bdstatic.com/acf6a6caa954380ecd35013b0d5771bb.png', '10:30:00', '18:00:00', 0, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '60周岁以下游客乘坐', 135, 190);
INSERT INTO `amusement_facility` VALUES (3, '激流勇进', '从27米的高度俯冲而下的游船，激起的浪花也将是前所未有的一种新奇体验，滔天巨浪，惊心动魄。宏伟、壮观、惊险、刺激。游客从站台上船出发，出站后慢慢提升到轨道的最高峰，沿着弯轨滑行，然后顺着轨道迅速下滑冲浪，完成冲浪后顺水漂流回到站台，全航程完毕。游客在游玩过程中，体会高速冲浪、失重、惊恐、离心的各种刺激感受，欣赏漫天浪花水帘的奇景，其乐无穷。毫无疑问，这是一项极度吸引游人的游乐设备。', '29.668591', '106.517368', 20, 5, '轨道/水上/失重', '成人', 'https://m.tuniucdn.com/fb2/t1/G5/M00/87/CC/Cii-tFqk99aIRWCwAAZnUf1dD4cAAEGlwMiFs0ABmdp412.jpg', '11:00:00', '18:00:00', 0, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '130cm-140cm（含140cm）须成人陪同，65周岁以下', 130, 190);
INSERT INTO `amusement_facility` VALUES (4, '尖峰时刻', '适合全年龄段的低空跳楼机。缓慢的抬升，给游客带来期待无限；攀至顶峰，从高空眺望远处，俯瞰无限风光，感受园区欢乐；而后在地心引力的作用下急速坠落，众人在惊慌中尖叫，就好像要跌得粉身碎骨一样，忽然又再次升空，惊心还在继续。', '29.666280', '106.515250', 16, 3, '垂直/高空/失重', '成人', 'https://img2.baidu.com/it/u=2270091954,977945280&fm=253&fmt=auto&app=138&f=PNG?w=529&h=500', '12:00:00', '18:00:00', 0, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '65周岁以下', 140, 190);
INSERT INTO `amusement_facility` VALUES (5, '高空飞翔', '满足你对飞翔的幻想，进入座椅，随着转盘旋转上升，感受离心力和运行高度的无穷乐趣，尖叫声不断，在观光上也是不错的选择。', '29.669792', '106.519623', 36, 5, '高空/旋转', '成人', 'https://www.51pla.com/uploadpic/20190423/1043/15559874318095076.jpg', '10:30:00', '18:00:00', 1, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '65周岁以下游客乘坐', 130, 190);
INSERT INTO `amusement_facility` VALUES (6, '欢乐对对碰', ' 碰碰车可爱多样的造型是每个人童年的回忆，疯狂的驾驶、激烈的碰撞、巧妙的躲避，欢乐对对碰不单是儿童的专属，还承载了每一个成年人的童年记忆，是一家人的欢乐聚集地。', '29.666732', '106.515511', 30, 5, '室内', '成人/老少皆宜/家长监护', 'https://t12.baidu.com/it/u=1342178684,204997447&fm=30&app=106&f=JPEG?w=640&h=427&s=FF9697AE32420FE55C2251300300C018', '10:00:00', '18:00:00', 0, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '120cm-140cm（不含140cm）须成人陪同且坐副驾驶', 120, 300);
INSERT INTO `amusement_facility` VALUES (7, '旋转木马', '在辽阔的欧洲大陆，流传着一个古老而美丽的童话，童话里有王子骑着白马，到遥远的国度迎接他的新娘。新娘不是公主，而是纯朴、善良、美丽的灰姑娘！童话里流浪汉坐上马车就能回到温暖的家，老人在高高的马背上看见逝去的青春年华，孩子们骑着小马玩耍，不用再那么辛苦的长大。后来，人们发明了旋转木马，五彩斑斓的灯光，充满梦幻色彩的绚丽装饰，营造出一个美丽的童话世界，人们相信，当木马开始转动时，心中所有的童话都拥有魔法，让所有的美好愿望得到实现。旋转的木马，会让你忘了忧伤，带着你进入欢笑与梦幻的天堂，虽然它没有翅膀，但却能够带着你飞翔。当您乘坐着造型逼真、神态各异的木马或厢车，在光影和音乐中旋转、游走时，仿佛置身于童话般的伊甸园，享受着凭海临风的浪漫与优雅。', '29.666635', '106.518684', 68, 5, '旋转', '成人/老少皆宜/家长监护', 'https://gimg2.baidu.com/image_search/src=http%3A%2F%2Fss2.meipian.me%2Fuser%2F8474064%2Fc814f918f8900001c0c7eeb01e571d53.jpg%3Fmeipian-raw%2Fbucket%2Fivwen%2Fkey%2FdXNlci84NDc0MDY0L2M4MTRmOTE4Zjg5MDAwMDFjMGM3ZWViMDFlNTcxZDUzLmpwZw%3D%3D%2Fsign%2Feae52e08c1b1d8a3adb49168c463761c.jpg&refer=http%3A%2F%2Fss2.meipian.me&app=2002&size=f9999,10000&q=a80&n=0&g=0n&fmt=auto?sec=1714704417&t=404736a3c1b9f432d719baf10c23a3ff', '10:30:00', '18:00:00', 0, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '身高120cm以下的儿童需成人陪同', 0, 300);
INSERT INTO `amusement_facility` VALUES (8, '飞跃重庆', '随着座椅缓缓上升至高处，7层楼高的巨型球幕将100位游客包围其中，全方位视角呈现重庆壮丽山川美景，伴随着座椅角度的变化，让你身临其境，如鸟儿般自由飞翔。10分钟内，高空尽揽绝美巴渝山水，体验凌空俯瞰的气势磅礴!', '29.670265', '106.519488', 110, 10, '室内/剧场', '成人/家长监护', 'https://img2.baidu.com/it/u=3304460745,537067766&fm=253&fmt=auto&app=138&f=JPEG?w=864&h=486', '10:30:00', '18:00:00', 0, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '60周岁以下游客乘坐', 120, 190);
INSERT INTO `amusement_facility` VALUES (9, '海盗船', '启动后从缓慢摆动慢慢地到急速摆动——犹如乘船出海遇到大风骇浪，时而冲上浪涛之颠，时而跌落波澜的谷底惊心动魄，既有趣又刺激，感受乘风破浪之险，挑战心理承受能力的极限。游客坐在海盗船上绕水平轴来回摆动，此起彼伏，交替而至的失重和超重感受，无限的惊险、刺激，同时又让人回味无穷，写意无限。', '29.667503', '106.519496', 20, 3, '失重', '成人/家长监护', 'https://koss.iyong.com/swift/v1/iyong_public/iyong_2603641690030592/image/20201229/1609223191530056904.jpg', '10:30:00', '18:00:00', 0, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '110cm-140cm（不含140cm）须有成人陪同', 110, 190);
INSERT INTO `amusement_facility` VALUES (10, '灵异马戏团', '恐怖的场景，惊悚的鬼屋，真实世界的对立面。立体声效与恐怖特技造型完美结合，配合动感的特效灯光、道具等营造出黑暗深处的鬼魅，一个个令人心跳不已的场景，游客将体验身临其境的胆战心惊。', '29.666583', '106.514772', 8, 8, '鬼屋', '成人/家长监护', 'https://pic.vjshi.com/2015-10-13/1444674771626_371/00004.jpg?x-oss-process=style/watermark', '11:00:00', '18:00:00', 0, '2024-04-03 03:01:48', '2024-04-03 03:01:48', 0, '120cm以下儿童须有监护能力的成人陪同', 0, 300);

-- ----------------------------
-- Table structure for base_facility
-- ----------------------------
DROP TABLE IF EXISTS `base_facility`;
CREATE TABLE `base_facility`  (
  `id` bigint NOT NULL AUTO_INCREMENT COMMENT '设施ID',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '设施名称',
  `longitude` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '纬度',
  `latitude` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '经度',
  `image_url` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '照片URL',
  `start_time` time NULL DEFAULT NULL COMMENT '开放开始时间',
  `close_time` time NULL DEFAULT NULL COMMENT '开放结束时间',
  `status` int NOT NULL DEFAULT 0 COMMENT '状态（0-正常，1-异常）',
  `create_time` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '添加时间',
  `update_time` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '修改时间',
  `is_deleted` int NOT NULL DEFAULT 0 COMMENT '逻辑删除标志（0-未删除，1-已删除）',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 6 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci COMMENT = '基础设施表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of base_facility
-- ----------------------------
INSERT INTO `base_facility` VALUES (1, '游客中心洗手间', '29.666737', '106.519651', 'https://img10.360buyimg.com/imgzone/jfs/t1/159038/22/5398/44172/6013cf77Ec7959164/4d6c18e10ae2069d.jpg', '10:00:00', '22:00:00', 0, '2024-04-03 06:48:43', '2024-04-03 06:48:43', 0);
INSERT INTO `base_facility` VALUES (2, '超级飞侠剧院洗手间', '29.665317', '106.517376', 'https://img10.360buyimg.com/imgzone/jfs/t1/159038/22/5398/44172/6013cf77Ec7959164/4d6c18e10ae2069d.jpg', '10:00:00', '22:00:00', 0, '2024-04-03 06:48:43', '2024-04-03 06:48:43', 0);
INSERT INTO `base_facility` VALUES (3, '前广场洗手间', '29.666722', '106.514277', 'https://img10.360buyimg.com/imgzone/jfs/t1/159038/22/5398/44172/6013cf77Ec7959164/4d6c18e10ae2069d.jpg', '10:00:00', '22:00:00', 0, '2024-04-03 06:48:43', '2024-04-03 06:48:43', 0);
INSERT INTO `base_facility` VALUES (4, '滨海湾区舞台洗手间', '29.668748', '106.517250', 'https://img10.360buyimg.com/imgzone/jfs/t1/159038/22/5398/44172/6013cf77Ec7959164/4d6c18e10ae2069d.jpg', '10:00:00', '22:00:00', 0, '2024-04-03 06:48:43', '2024-04-03 06:48:43', 0);
INSERT INTO `base_facility` VALUES (5, '恐龙森林洗手间', '29.668068', '106.520570', 'https://img10.360buyimg.com/imgzone/jfs/t1/159038/22/5398/44172/6013cf77Ec7959164/4d6c18e10ae2069d.jpg', '10:00:00', '22:00:00', 0, '2024-04-03 06:48:43', '2024-04-03 06:48:43', 0);

-- ----------------------------
-- Table structure for restaurant_facility
-- ----------------------------
DROP TABLE IF EXISTS `restaurant_facility`;
CREATE TABLE `restaurant_facility`  (
  `id` bigint NOT NULL AUTO_INCREMENT COMMENT '设施ID',
  `name` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '设施名称',
  `introduction` varchar(1024) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '设施介绍',
  `longitude` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '纬度',
  `latitude` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '经度',
  `type` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NOT NULL COMMENT '设施类型（中式快餐、西式快餐、面点、饮品、小吃）',
  `image_url` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '照片URL',
  `start_time` time NULL DEFAULT NULL COMMENT '开放开始时间',
  `close_time` time NULL DEFAULT NULL COMMENT '开放结束时间',
  `status` int NOT NULL DEFAULT 0 COMMENT '状态（0-正常，1-异常）',
  `create_time` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '添加时间',
  `update_time` timestamp NULL DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP COMMENT '修改时间',
  `is_deleted` int NOT NULL DEFAULT 0 COMMENT '逻辑删除标志（0-未删除，1-已删除）',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 6 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci COMMENT = '餐饮设施表' ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of restaurant_facility
-- ----------------------------
INSERT INTO `restaurant_facility` VALUES (1, '船长餐厅', '大工业时代码头风的滨海湾区的湖岸边坐落着加勒比海风格的船长餐厅。在这家浓郁航海味道的餐厅游客不仅能感受到热情好客的服务，还能享受到地道的粤式烧腊美味。坐在餐厅的户外用餐区，斜倚着美丽的人工湖的栏杆，视线前方除了风轻轻吹动的湖水外，还有那座浓郁工业风情的钢架大桥。旁边被餐厅遮挡的除了让很多人害怕体验的大摆锤外，还有餐厅另一侧小山上的直插天际的天地双雄。挡住的是视线，挡不住的是人们的热情。', '29.667593', '106.519117', '中式快餐/面点/饮品/小吃', 'https://img0.baidu.com/it/u=1642438583,2493003820&fm=253&fmt=auto&app=138&f=JPEG?w=667&h=500', '10:00:00', '18:00:00', 0, '2024-04-03 03:23:29', '2024-04-03 03:23:29', 0);
INSERT INTO `restaurant_facility` VALUES (2, '欢乐驿站', '游乐园的欢乐驿站餐厅，是每位游客在畅游之余，享受美食与休憩的理想之地。这里汇聚了多样化的美食选择，既有适合大人小孩的各类套餐，也有满足各种口味的特色小吃和饮品。餐厅的环境温馨舒适，装修风格充满童趣，与游乐园的整体氛围相得益彰。', '29.667422', '106.512782', '饮品/小吃', 'https://img0.baidu.com/it/u=1103514578,428959848&fm=253&fmt=auto&app=138&f=JPEG?w=728&h=500', '10:00:00', '18:00:00', 0, '2024-04-03 03:23:31', '2024-04-03 03:23:31', 0);
INSERT INTO `restaurant_facility` VALUES (3, '丛林探险餐厅', '丛林探险餐厅位于欢乐谷的森林恐龙区，这家开放式档口餐厅不仅是全园区所有餐厅中提供产品种类最多的餐厅，同时因为紧邻园区的镇园之宝“宽翼过山车”，它的人流量也是可以预见的庞大。坐在餐桌旁用餐，过山车就在眼前呼啸而过，相信这种独特的用餐体验能停留在你的记忆中很久。', '29.668933', '106.520470', '中式快餐/饮品/小吃', 'https://p6.itc.cn/images01/20201202/9cef4dc74fd641b5a428f94c2986c09e.jpeg', '10:00:00', '18:00:00', 0, '2024-04-03 06:35:52', '2024-04-03 06:35:52', 0);
INSERT INTO `restaurant_facility` VALUES (4, '维多利亚花园餐厅', '在浓郁欧式风情的欢乐时光区隐藏着一幢内部满是怀旧工业风情的建筑，它就是位于4D影院顶楼的维多利亚花园餐厅。这家以中式为主同时兼有中西不同口味的快餐厅提供各式主食、凉菜、饮品小吃等供游客自行选取。餐厅的户外用餐区紧邻观景露台，站在这里可以自由俯瞰整个欢乐谷的美丽风景。不论是用餐、观景还是短暂休息放松，这里都是上佳选择。', '29.666622', '106.519154', '中式快餐/西式快餐', 'https://img2.baidu.com/it/u=747062568,1006620629&fm=253&fmt=auto&app=138&f=JPEG?w=750&h=500', '10:00:00', '18:00:00', 0, '2024-04-03 06:35:52', '2024-04-03 06:35:52', 0);
INSERT INTO `restaurant_facility` VALUES (5, '乐迪汉堡', '乐迪汉堡店，位于乐园的心脏地带，是每位游客尽享美食与欢乐的理想之选。店内环境明亮舒适，洋溢着轻松愉快的氛围。我们精选优质食材，手工现做每一个汉堡，保证新鲜美味。无论是经典牛肉汉堡，还是特色鸡肉汉堡，都能满足您的味蕾。此外，店内还提供各种小吃和饮品，让您在游玩之余，尽享美食的乐趣。快来乐迪汉堡店，与亲朋好友一起品尝美味，留下难忘的回忆吧', '29.667930', '106.518134', '西式快餐/饮品/小吃', 'https://img1.baidu.com/it/u=1401669384,1007582701&fm=253&fmt=auto&app=138&f=JPEG?w=450&h=278', '10:00:00', '18:00:00', 0, '2024-04-03 06:35:52', '2024-04-03 06:35:52', 0);

-- ----------------------------
-- Table structure for user
-- ----------------------------
DROP TABLE IF EXISTS `user`;
CREATE TABLE `user`  (
  `id` bigint NOT NULL AUTO_INCREMENT COMMENT '用户id',
  `username` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '用户名',
  `account` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '账号',
  `password` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '密码',
  `phone` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '手机号',
  `gender` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '性别（“男”，“女”）',
  `avatar_url` varchar(255) CHARACTER SET utf8mb4 COLLATE utf8mb4_0900_ai_ci NULL DEFAULT NULL COMMENT '用户头像',
  `create_time` timestamp NULL DEFAULT CURRENT_TIMESTAMP COMMENT '创建时间',
  `update_time` timestamp NULL DEFAULT NULL ON UPDATE CURRENT_TIMESTAMP COMMENT '更新时间',
  `is_deleted` int NULL DEFAULT NULL COMMENT '逻辑删除',
  PRIMARY KEY (`id`) USING BTREE
) ENGINE = InnoDB AUTO_INCREMENT = 9 CHARACTER SET = utf8mb4 COLLATE = utf8mb4_0900_ai_ci ROW_FORMAT = Dynamic;

-- ----------------------------
-- Records of user
-- ----------------------------
INSERT INTO `user` VALUES (1, '姜堂蕴之', 'jtyz', 'jtyz123', '17784276887', '女', 'https://c-ssl.duitang.com/uploads/blog/202010/24/20201024225641_2f87c.png', '2024-04-03 01:33:22', '2024-04-03 01:34:44', 0);
INSERT INTO `user` VALUES (2, '孟祥硕', 'mxs', 'mxs123', '17784276666', '男', 'https://c-ssl.duitang.com/uploads/blog/202010/24/20201024225640_0b017.png', '2024-04-03 01:33:22', '2024-04-03 01:34:44', 0);
INSERT INTO `user` VALUES (3, '池跃花', 'cyh', 'cyh123', '17784275555', '女', 'https://c-ssl.duitang.com/uploads/blog/202010/24/20201024225641_3e261.png', '2024-04-03 01:33:23', '2024-04-03 06:07:57', 0);
INSERT INTO `user` VALUES (4, '陈金铭', 'cjm', 'cjm123', '17784278888', '女', 'https://c-ssl.duitang.com/uploads/blog/202010/24/20201024225645_1e00e.png', '2024-04-03 01:33:23', '2024-04-03 01:34:44', 0);
INSERT INTO `user` VALUES (5, '徐畅', 'xc', 'xc123', '17784272222', '男', 'https://c-ssl.duitang.com/uploads/blog/202010/24/20201024225643_88588.png', '2024-04-03 01:33:23', '2024-04-03 01:34:44', 0);
INSERT INTO `user` VALUES (6, '陈佳鑫', 'cjx', 'cjx123', '17784279999', '男', 'https://c-ssl.duitang.com/uploads/blog/202010/24/20201024225642_80be5.png', '2024-04-03 01:33:23', '2024-04-03 06:07:57', 0);
INSERT INTO `user` VALUES (7, '秦红梅', 'qhm', 'qhm123', '17784273333', '女', 'https://c-ssl.duitang.com/uploads/blog/202010/24/20201024225641_3e261.png', '2024-04-03 01:33:23', '2024-04-03 01:34:44', 0);

SET FOREIGN_KEY_CHECKS = 1;
