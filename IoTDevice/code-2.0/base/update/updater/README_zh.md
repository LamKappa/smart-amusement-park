# 升级包安装组件<a name="ZH-CN_TOPIC_0000001148614629"></a>

-   [简介](#section184mcpsimp)
-   [目录](#section198mcpsimp)
-   [说明](#section218mcpsimp)
    -   [使用说明](#section220mcpsimp)

-   [相关仓](#section247mcpsimp)

## 简介<a name="section184mcpsimp"></a>

升级包安装组件运行在recovery分区，其功能主要包括读取misc分区信息获取升级包状态，对升级包进行校验，确保升级包合法有效；然后从升级包中解析出升级的可执行程序，创建子进程并启动升级程序。具体升级的动作由升级脚本控制。

## 目录<a name="section198mcpsimp"></a>

```
base/update/updater/
├── resources           # 升级子系统用户界面图片资源目录
├── services            # 组件服务层代码目录
│   ├── applypatch      # 升级包数据更新代码目录
│   ├── fs_manager      # 文件系统和分区管理代码目录
│   ├── include         # 升级子系统头文件目录
│   ├── log             # 升级子系统日志模块目录
│   ├── package         # 升级包管理模块目录
│   ├── script          # 升级脚本管理目录
│   ├── diffpatch       # 差分还原代码目录
│   ├── sparse_image    # 稀疏镜像解析代码目录
│   ├── ui              # 升级ui界面代码目录
│   └── updater_binary  # 升级可执行程序目录
├── interfaces
│   └── kits            # 对外模块接口定义
└── utils               # 升级子系统通用代码目录
    └── include         # 升级子系统通用函数头文件目录
```

## 说明<a name="section218mcpsimp"></a>

### 使用说明<a name="section220mcpsimp"></a>

升级包安装组件运行在recovery分区里，需要如下的操作

1、创建recovery分区

recovery是一个独立的分区，分区大小建议不小于20MB。recovery分区镜像是ext4 格式文件系统。确保系统内核ext4 文件系统的config 是打开状态。

2、创建misc分区

misc 分区中存储了升级子系统在升级过程中需要的元数据\(metadata\)，如升级命令，掉电续传记录等。 misc 分区的大小约1MB，是一个裸分区，无需制作文件系统， 升级子系统直接访问。

3、分区配置表

升级包安装组件在运行过程中，需要通过分区配置表操作分区。默认的分区配置表文件名是fstab.updater，在编译的时候，打包到升级包安装组件中。

4、升级包安装组件启动

recovery分区的init 进程有单独的配置文件 init.cfg，升级包安装进程启动配置在该文件中。

5、升级包安装组件编译

a、在build/subsystem\_config.json文件添加配置。

如下：

```
"updater": {
"project": "hmf/updater",
"path": "base/update/updater",
"name": "updater",
"dir": "base/update"
},
```

b、 产品中添加需要编译的组件

以Hi3516DV300为例，在productdefine/common/products/Hi3516DV300.json 中添加updater：

```
     "updater:updater":{},
```

6、recovery分区镜像编译

编译配置在build仓下，build\_updater\_image.sh 脚本中，该脚本由OHOS 编译系统调用。

## 相关仓<a name="section247mcpsimp"></a>

升级子系统

**update\_updater**

build

productdefine\_common

