# 本地数据管理组件<a name="ZH-CN_TOPIC_0000001124534865"></a>

-   [简介](#section11660541593)
    -   [关系型数据库（RDB）](#section1589234172717)
    -   [轻量级偏好数据库（Preferences）](#section1287582752719)

-   [目录](#section161941989596)
-   [关系型数据库（RDB）](#section101010894114)
    -   [约束](#section18387142613414)

-   [轻量级偏好数据库（Preferences）](#section762641474720)
    -   [约束](#section1944481420489)

-   [相关仓](#section1371113476307)

## 简介<a name="section11660541593"></a>

**关系型数据库（Relational Database，RDB）**是一种基于关系模型来管理数据的数据库。OpenHarmony关系型数据库基于SQLite组件提供了一套完整的对本地数据库进行管理的机制。

**轻量级偏好数据库（Preferences）**主要提供轻量级Key-Value操作，支持本地应用存储少量数据，数据存储在本地文件中，同时也加载在内存中的，所以访问速度更快，效率更高。轻量级偏好数据库属于非关系型数据库，不宜存储大量数据，经常用于操作键值对形式数据的场景。

### 关系型数据库（RDB）<a name="section1589234172717"></a>

OpenHarmony关系型数据库底层使用SQLite作为持久化存储引擎，支持SQLite具有的所有数据库特性，包括但不限于事务、索引、视图、触发器、外键、参数化查询和预编译SQL语句。

**图 1**  关系型数据库运作机制<a name="fig3330103712254"></a>  


![](figures/zh-cn_image_0000001115980740.png)

### 轻量级偏好数据库（Preferences）<a name="section1287582752719"></a>

1.  本模块提供偏好型数据库的操作类，应用通过这些操作类完成数据库操作。
2.  借助PreferencesHelper，可以将指定文件的内容加载到Preferences实例，每个文件最多有一个Preferences实例，系统会通过静态容器将该实例存储在内存中，直到主动从内存中移除该实例或者删除该文件。
3.  获取Preferences实例后，可以借助Preferences类的函数，从Preferences实例中读取数据或者将数据写入Preferences实例，通过flush或者flushSync将Preferences实例持久化。

**图 2**  轻量级偏好数据库运行机制<a name="fig833053712258"></a>  


![](figures/zh-cn_image_0000001162419711.png)

## 目录<a name="section161941989596"></a>

```
//foundation/distributeddatamgr/appdatamgr
├── frameworks            # 框架层代码
│   └── innerkitsimpl     # 内部接口实现
└── interfaces            # 接口代码
    └── innerkits         # 内部接口声明
```

## 关系型数据库（RDB）<a name="section101010894114"></a>

以下是几个基本概念：

-   **关系型数据库**

    创建在关系模型基础上的数据库，以行和列的形式存储数据。

-   **结果集**

    指用户查询之后的结果集合，可以对数据进行访问。结果集提供了灵活的数据访问方式，可以更方便的拿到用户想要的数据。

-   **SQLite数据库**

    一款轻量级的数据库，是遵守ACID的关系型数据库组件。它是一个开源的项目。


### 约束<a name="section18387142613414"></a>

数据库中连接池的最大数量是4个，用以管理用户的读写操作。

为保证数据的准确性，数据库同一时间只能支持一个写操作。

## 轻量级偏好数据库（Preferences）<a name="section762641474720"></a>

以下是几个基本概念：

-   **Key-Value数据库**

    一种以键值对存储数据的一种数据库。Key是关键字，Value是值。

-   **非关系型数据库**

    区别于关系数据库，不保证遵循ACID（Atomic、Consistency、Isolation及Durability）特性，不采用关系模型来组织数据，数据之间无关系，扩展性好。

-   **偏好数据**

    用户经常访问和使用的数据。


### 约束<a name="section1944481420489"></a>

Key键为String类型，要求非空且长度不超过80个字符。

如果Value值为String类型，可以为空但是长度不超过8192个字符。

存储的数据量应该是轻量级的，建议存储的数据不超过一万条，否则会在内存方面产生较大的开销。

## 相关仓<a name="section1371113476307"></a>

分布式数据管理子系统

distributeddatamgr\_appdatamgr

third\_party\_sqlite

