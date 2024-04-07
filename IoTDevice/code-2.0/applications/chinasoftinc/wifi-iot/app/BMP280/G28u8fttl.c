/******************************************************************************\
**  版    权 :  深圳开鸿数字产业发展有限公司（2021）
**  文件名称 :  G28u8fttl.c
**  功能描述 :  GPS+北斗模组驱动
**  作    者 :  王滨泉
**  日    期 :  2021.10.14
**  版    本 :  V0.0.1
**  变更记录 :  V0.0.1/2021.10.14
                1 首次创建
\******************************************************************************/

/******************************************************************************\
                                 Includes
\******************************************************************************/

#include "G28u8fttl.h"
/******************************************************************************\
                             Variables definitions
\******************************************************************************/
#define GPS_TASK_STACK_SIZE         4096*2                 //任务栈大小
#define GPS_TASK_PRIO               10                   //任务优先等级
#define GPS_UART_NUM                HI_UART_IDX_2       //使用串口2与模组通讯
#define GPS_UART_BAUDRATE           9600                //串口通讯波特率
#define GPS_UART_TX                 12                  //GPIO12
#define GPS_UART_RX                 11                  //GPIO11

// 土壤湿度控制模组句柄
typedef struct {
    uint32_t Baudrate;                          //通讯波特率
    uint32_t Longitude;                         //经度
    uint32_t Latitude;                          //纬度
} TS_GPS_PARAM;

const uint32_t Gps_Module_Baudrate[9]= {4800,9600,19200,38400,57600,115200,230400,460800,921600}; //模块支持波特率数组

const uint8_t CloseFeature[11][26] = {
    //关闭 GPDTM 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x44, 0x54, 0x4d, 0x2a, 0x33, 0x42, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x0a, 0x00, 0x04, 0x23},
    //关闭 GPGBS 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x42, 0x53, 0x2a, 0x33, 0x30, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x09, 0x00, 0x03, 0x21},
    //关闭 GPGGA 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x47, 0x41, 0x2a, 0x32, 0x37, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x00, 0x00, 0xfa, 0x0f},
    //关闭 GPGLL 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x4c, 0x4c, 0x2a, 0x32, 0x31, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x01, 0x00, 0xfb, 0x11},
    //关闭 GPGRS 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x52, 0x53, 0x2a, 0x32, 0x30, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x06, 0x00, 0x00, 0x1b},
    //关闭 GPGSA 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x41, 0x2a, 0x33, 0x33, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x02, 0x00, 0xfc, 0x13},
    //关闭 GPGST 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x54, 0x2a, 0x32, 0x36, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x07, 0x00, 0x01, 0x1d},
    //关闭 GPGSV 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x53, 0x56, 0x2a, 0x32, 0x34, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x03, 0x00, 0xfd, 0x15},
    //关闭 GPRMC 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x52, 0x4d, 0x43, 0x2a, 0x33, 0x41, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x04, 0x00, 0xfe, 0x17},
    //关闭 GPVTG 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x56, 0x54, 0x47, 0x2a, 0x32, 0x33, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x05, 0x00, 0xff, 0x19},
    //关闭 GPZDA 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x5a, 0x44, 0x41, 0x2a, 0x33, 0x39, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x08, 0x00, 0x02, 0x1f},
};
const uint8_t OpenFeature[11][26] = {
	//开启 GPGGA 语句
    {0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x47, 0x47, 0x41, 0x2a, 0x32, 0x37, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x00, 0x01, 0xfb, 0x10},
	//开启 GPRMC 语句
	{0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2c, 0x52, 0x4d, 0x43, 0x2a, 0x33, 0x41, 0x0d, 0x0a, 0xb5, 0x62, 0x06, 0x01, 0x03, 0x00, 0xf0, 0x04, 0x01, 0xff, 0x18},
};



static TS_GPS_PARAM s_Gps = {0};
static hi_u32 s_Thread_Mux_Id;
static nmea_msg s_GpsMsg = {0};             //GPS信息
/******************************************************************************\
                             Functions definitions
\******************************************************************************/
/*
* 函数名称 : Data_Transducer
* 功能描述 : 经纬度数据转换器(计算公式：abc+(de)/60+fghi/600000)
* 参    数 : str - 数据指针
* 返回值   : 转换值
* 示    例 : data = Data_Transducer(&str);
*/
/******************************************************************************/
float Data_Transducer(char *str)
/******************************************************************************/
{
    char *p = NULL;
    uint8_t point;
    uint8_t len;
    uint16_t abc;
    uint8_t de;
    uint16_t fghi;
    uint8_t i;
    float tmp;
    const uint16_t magnification[5] = {0,1000,100,10,1};
    
    p = str;
    len = strlen(str);
    //查找小数点位置
    for(point = 0; point<len; point++) {
        if(*p == '.') {
            break;
        }
        p++;
    }
    //计算abc值
    abc = 0;
    de = 0;
    fghi = 0;
    if(point-5 >= 0) {
        abc += (str[point-5]-0x30)*100;
    }
    if(point-4 >= 0) {
        abc += (str[point-4]-0x30)*10;
    }
    if(point-3 >= 0) {
        abc += (str[point-3]-0x30);
    }
    //计算de值
    if(point-2 >= 0) {
        de += (str[point-2]-0x30)*10;
    }
    if(point-1 >= 0) {
        de += (str[point-1]-0x30);
    }
    //计算fghi值
    for(i = 1 ; i < 5; i++) {
        if(len-1-point-i >= 0) {
            fghi += (str[point+i]-0x30)*magnification[i];
        }
    }
    //计算转换值，公式：abc+de/60+fghi/600000
    tmp = (float)abc+(float)de/60+(float)fghi/600000;
    
    printf("\r\n[Data_Transducer]str len = %d, point = %d, abc = %d, de = %d, fghi = %d, tmp = %2f \r\n", len, point, abc, de, fghi, tmp);
    
    return tmp*10000;
}
/*
* 函数名称 : NMEA_Comma_Pos
* 功能描述 : 从buf里面得到第cx个逗号所在的位置
* 参    数 : 无
* 返回值   : 0~0XFE,代表逗号所在位置的偏移.
             0XFF,代表不存在第cx个逗号
* 示    例 : p = NMEA_Comma_Pos(&buf,cx);
*/
/******************************************************************************/
uint8_t NMEA_Comma_Pos(uint8_t *buf,uint8_t cx)
/******************************************************************************/
{
    uint8_t *p=buf;
    while(cx) {
        if((*buf=='*') || (*buf<' ') || (*buf>'z')) {
            printf("\r\n[NMEA_Comma_Pos] 0XFF-------------------\r\n");
            return 0XFF;//遇到'*'或者非法字符,则不存在第cx个逗号
        }
        if(*buf==',') {
            cx--;
        }
        buf++;
    }
    return buf-p;
}
//m^n函数
//返回值:m^n次方.
/*
* 函数名称 : NMEA_Pow
* 功能描述 : m^n函数
* 参    数 : 无
* 返回值   : m^n次方.
* 示    例 : Longitude = NMEA_Pow(m,n);
*/
/******************************************************************************/
uint32_t NMEA_Pow(uint8_t m,uint8_t n)
/******************************************************************************/
{
    uint32_t result=1;
    while(n--) { result*=m; }
    return result;
}
//str转换为数字,以','或者'*'结束
//buf:数字存储区
//dx:小数点位数,返回给调用函数
//返回值:转换后的数值
int NMEA_Str2num(uint8_t *buf,uint8_t*dx) {
    uint8_t *p=buf;
    uint32_t ires=0,fres=0;
    uint8_t ilen=0,flen=0,i;
    uint8_t mask=0;
    int res;
    while(1) { //得到整数和小数的长度
        if(*p == '-') { //是负数
            mask|=0X02;
            p++;
        }
        if((*p == ',') || (*p == '*')) { //遇到结束了
            break;
        }
        if(*p=='.') { //遇到小数点了
            mask|=0X01;
            p++;
        } else if((*p > '9') || (*p < '0')) { //有非法字符
            ilen=0;
            flen=0;
            break;
        }
        if(mask&0X01) {
            flen++;
        } else {
            ilen++;
        }
        p++;
    }
    if(mask&0X02) {
        buf++;  //去掉负号
    }
    for(i=0; i<ilen; i++) { //得到整数部分数据
        printf("\r\n[NMEA_Comma_Pos] mask&0X02----------------\r\n");
        ires+=NMEA_Pow(10,ilen-1-i)*(buf[i]-'0');
    }
    if(flen>5) { flen=5; }  //最多取5位小数
    *dx=flen;           //小数点位数
    for(i=0; i<flen; i++) { //得到小数部分数据
        fres+=NMEA_Pow(10,flen-1-i)*(buf[ilen+1+i]-'0');
    }
    res=ires*NMEA_Pow(10,flen)+fres;
    if(mask&0X02) { res=-res; }
    return res;
}
//分析GPGSV信息
//gpsx:nmea信息结构体
//buf:接收到的GPS数据缓冲区首地址
void NMEA_GPGSV_Analysis(nmea_msg *gpsx,uint8_t *buf) {
    uint8_t *p,*p1,dx;
    uint8_t len,i,j,slx=0;
    uint8_t posx;
    p=buf;
    p1=(uint8_t*)strstr((const char *)p,"$GPGSV");
    len=p1[7]-'0';                              //得到GPGSV的条数
    posx=NMEA_Comma_Pos(p1,3);                  //得到可见卫星总数
    if(posx!=0XFF) { gpsx->svnum=NMEA_Str2num(p1+posx,&dx); }
    for(i=0; i<len; i++) {
        p1=(uint8_t*)strstr((const char *)p,"$GPGSV");
        for(j=0; j<4; j++) {
            posx=NMEA_Comma_Pos(p1,4+j*4);
            if(posx!=0XFF) { gpsx->slmsg[slx].num=NMEA_Str2num(p1+posx,&dx); }  //得到卫星编号
            else { break; }
            posx=NMEA_Comma_Pos(p1,5+j*4);
            if(posx!=0XFF) { gpsx->slmsg[slx].eledeg=NMEA_Str2num(p1+posx,&dx); } //得到卫星仰角
            else { break; }
            posx=NMEA_Comma_Pos(p1,6+j*4);
            if(posx!=0XFF) { gpsx->slmsg[slx].azideg=NMEA_Str2num(p1+posx,&dx); } //得到卫星方位角
            else { break; }
            posx=NMEA_Comma_Pos(p1,7+j*4);
            if(posx!=0XFF) { gpsx->slmsg[slx].sn=NMEA_Str2num(p1+posx,&dx); }   //得到卫星信噪比
            else { break; }
            slx++;
        }
        p=p1+1;//切换到下一个GPGSV信息
    }
}
//分析BDGSV信息
//gpsx:nmea信息结构体
//buf:接收到的北斗数据缓冲区首地址
void NMEA_BDGSV_Analysis(nmea_msg *gpsx,uint8_t *buf) {
    uint8_t *p,*p1,dx;
    uint8_t len,i,j,slx=0;
    uint8_t posx;
    p=buf;
    p1=(uint8_t*)strstr((const char *)p,"$BDGSV");
    len=p1[7]-'0';                              //得到BDGSV的条数
    posx=NMEA_Comma_Pos(p1,3);                  //得到可见北斗卫星总数
    if(posx!=0XFF) { gpsx->beidou_svnum=NMEA_Str2num(p1+posx,&dx); }
    for(i=0; i<len; i++) {
        p1=(uint8_t*)strstr((const char *)p,"$BDGSV");
        for(j=0; j<4; j++) {
            posx=NMEA_Comma_Pos(p1,4+j*4);
            if(posx!=0XFF) { gpsx->beidou_slmsg[slx].beidou_num=NMEA_Str2num(p1+posx,&dx); } //得到卫星编号
            else { break; }
            posx=NMEA_Comma_Pos(p1,5+j*4);
            if(posx!=0XFF) { gpsx->beidou_slmsg[slx].beidou_eledeg=NMEA_Str2num(p1+posx,&dx); } //得到卫星仰角
            else { break; }
            posx=NMEA_Comma_Pos(p1,6+j*4);
            if(posx!=0XFF) { gpsx->beidou_slmsg[slx].beidou_azideg=NMEA_Str2num(p1+posx,&dx); } //得到卫星方位角
            else { break; }
            posx=NMEA_Comma_Pos(p1,7+j*4);
            if(posx!=0XFF) { gpsx->beidou_slmsg[slx].beidou_sn=NMEA_Str2num(p1+posx,&dx); } //得到卫星信噪比
            else { break; }
            slx++;
        }
        p=p1+1;//切换到下一个BDGSV信息
    }
}
//分析GNGGA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS/北斗数据缓冲区首地址
void NMEA_GNGGA_Analysis(nmea_msg *gpsx,uint8_t *buf) {
    uint8_t *p1,dx;
    uint8_t posx;
    char *name = NULL;
    char *utc= NULL;
    char *latitude= NULL;
    char *nshemi= NULL;
    char *longitude= NULL;
    char *ewhemi= NULL;
    char *gpssta= NULL;
    float utc_data;
    double f_latitude,f_longitude;
    int temp;
    float rs;
    
    p1=(uint8_t*)strstr((const char *)buf,"$GNGGA");
    
    name = strsep(&p1, ",");                        //获取名称
    utc = strsep(&p1, ",");                         //获取UTC时间
    latitude = strsep(&p1, ",");                    //获取纬度
    nshemi = strsep(&p1, ",");                      //获取南北纬
    longitude = strsep(&p1, ",");                   //获取经度
    ewhemi = strsep(&p1, ",");                      //获取东西经
    gpssta = strsep(&p1, ",");                      //获取定位状态
    
    
    printf("\r\n[NMEA_GNGGA_Analysis] name = %s ", name);
    if(utc != NULL) {
        utc_data = atof(utc);
        printf("\r\n[NMEA_GNGGA_Analysis] utc = %f ", utc_data);
    }
    if(latitude != NULL) {
        printf("\r\n[NMEA_GNGGA_Analysis] gpsx->latitude = %s ", latitude);
        gpsx->latitude = Data_Transducer(latitude);
    }
    if(nshemi != NULL) {
        gpsx->nshemi = atoi(nshemi);
        printf("\r\n[NMEA_GNGGA_Analysis] nshemi = %s ", nshemi);
    }
    if(longitude != NULL) {
        printf("\r\n[NMEA_GNGGA_Analysis] longitude = %s ", longitude);
        gpsx->longitude = Data_Transducer(longitude);
    }
    
    if(ewhemi != NULL) {
        gpsx->ewhemi = atoi(ewhemi);
        printf("\r\n[NMEA_GNGGA_Analysis] ewhemi = %s ", ewhemi);
    }
    if(gpssta != NULL) {
        gpsx->gpssta = atoi(gpssta);
        printf("\r\n[NMEA_GNGGA_Analysis] gpssta = %s ", gpssta);
    }
    
    // p1=(uint8_t*)strstr((const char *)buf,"$GNGGA");
    
    // posx=NMEA_Comma_Pos(p1,6);                               //得到GPS状态
    // if(posx!=0XFF)gpsx->gpssta=NMEA_Str2num(p1+posx,&dx);
    // printf("\r\n********gpssta = %d*****************\r\n",gpsx->gpssta);
    // posx=NMEA_Comma_Pos(p1,7);                               //得到用于定位的卫星数
    // if(posx!=0XFF)gpsx->posslnum=NMEA_Str2num(p1+posx,&dx);
    // printf("\r\n********posslnum = %d*****************\r\n",gpsx->posslnum);
    // posx=NMEA_Comma_Pos(p1,9);                               //得到海拔高度
    // if(posx!=0XFF)gpsx->altitude=NMEA_Str2num(p1+posx,&dx);
    // printf("\r\n********altitude = %d*****************\r\n",gpsx->altitude);
}
//分析GNGSA信息
//gpsx:nmea信息结构体
//buf:接收到的GPS/北斗数据缓冲区首地址
void NMEA_GNGSA_Analysis(nmea_msg *gpsx,uint8_t *buf) {
    uint8_t *p1,dx;
    uint8_t posx;
    uint8_t i;
    p1=(uint8_t*)strstr((const char *)buf,"$GNGSA");
    posx=NMEA_Comma_Pos(p1,2);                              //得到定位类型
    if(posx!=0XFF) { gpsx->fixmode=NMEA_Str2num(p1+posx,&dx); }
    for(i=0; i<12; i++) {                                   //得到定位卫星编号
        posx=NMEA_Comma_Pos(p1,3+i);
        if(posx!=0XFF) { gpsx->possl[i]=NMEA_Str2num(p1+posx,&dx); }
        else { break; }
    }
    posx=NMEA_Comma_Pos(p1,15);                             //得到PDOP位置精度因子
    if(posx!=0XFF) { gpsx->pdop=NMEA_Str2num(p1+posx,&dx); }
    posx=NMEA_Comma_Pos(p1,16);                             //得到HDOP位置精度因子
    if(posx!=0XFF) { gpsx->hdop=NMEA_Str2num(p1+posx,&dx); }
    posx=NMEA_Comma_Pos(p1,17);                             //得到VDOP位置精度因子
    if(posx!=0XFF) { gpsx->vdop=NMEA_Str2num(p1+posx,&dx); }
}
//分析GNRMC信息
//gpsx:nmea信息结构体
//buf:接收到的GPS/北斗数据缓冲区首地址
void NMEA_GNRMC_Analysis(nmea_msg *gpsx,uint8_t *buf) {
    uint8_t *p1,dx;
    uint8_t posx;
    uint32_t temp;
    float rs;
    p1=(uint8_t*)strstr((const char *)buf,"GNRMC");//"$GNRMC",经常有&和GNRMC分开的情况,故只判断GPRMC.
	if(p1 == NULL) { return; }
    posx=NMEA_Comma_Pos(p1,1);                              //得到UTC时间
    if(posx!=0XFF) {
        temp=NMEA_Str2num(p1+posx,&dx)/NMEA_Pow(10,dx);     //得到UTC时间,去掉ms
        gpsx->utc.hour=temp/10000;
        gpsx->utc.min=(temp/100)%100;
        gpsx->utc.sec=temp%100;
    }
    posx=NMEA_Comma_Pos(p1,3);                              //得到纬度
    if(posx!=0XFF) {
        temp=NMEA_Str2num(p1+posx,&dx);
        gpsx->latitude=temp/NMEA_Pow(10,dx+2);  //得到°
        rs=temp%NMEA_Pow(10,dx+2);              //得到'
        gpsx->latitude=gpsx->latitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//转换为°
    }
    posx=NMEA_Comma_Pos(p1,4);                              //南纬还是北纬
    if(posx!=0XFF) { gpsx->nshemi=*(p1+posx); }
    posx=NMEA_Comma_Pos(p1,5);                              //得到经度
    if(posx!=0XFF) {
        temp=NMEA_Str2num(p1+posx,&dx);
        gpsx->longitude=temp/NMEA_Pow(10,dx+2); //得到°
        rs=temp%NMEA_Pow(10,dx+2);              //得到'
        gpsx->longitude=gpsx->longitude*NMEA_Pow(10,5)+(rs*NMEA_Pow(10,5-dx))/60;//转换为°
    }
    posx=NMEA_Comma_Pos(p1,6);                              //东经还是西经
    if(posx!=0XFF) { gpsx->ewhemi=*(p1+posx); }
    posx=NMEA_Comma_Pos(p1,9);                              //得到UTC日期
    if(posx!=0XFF) {
        temp=NMEA_Str2num(p1+posx,&dx);                     //得到UTC日期
        gpsx->utc.date=temp/10000;
        gpsx->utc.month=(temp/100)%100;
        gpsx->utc.year=2000+temp%100;
    }
}
//分析GNVTG信息
//gpsx:nmea信息结构体
//buf:接收到的GPS/北斗数据缓冲区首地址
void NMEA_GNVTG_Analysis(nmea_msg *gpsx,uint8_t *buf) {
    uint8_t *p1,dx;
    uint8_t posx;
    p1=(uint8_t*)strstr((const char *)buf,"$GNVTG");
    posx=NMEA_Comma_Pos(p1,7);                              //得到地面速率
    if(posx!=0XFF) {
        gpsx->speed=NMEA_Str2num(p1+posx,&dx);
        if(dx<3) { gpsx->speed*=NMEA_Pow(10,3-dx); }            //确保扩大1000倍
    }
}
//提取NMEA-0183信息
//gpsx:nmea信息结构体
//buf:接收到的GPS/北斗数据缓冲区首地址
void GPS_Analysis(nmea_msg *gpsx,uint8_t *buf) {
    // NMEA_GPGSV_Analysis(gpsx,buf);   //GPGSV解析
    // NMEA_BDGSV_Analysis(gpsx,buf);   //BDGSV解析
    NMEA_GNGGA_Analysis(gpsx,buf);  //GNGGA解析
    // NMEA_GNGSA_Analysis(gpsx,buf);   //GNGSA解析
    // NMEA_GNRMC_Analysis(gpsx,buf);   //GNRMC解析
    // NMEA_GNVTG_Analysis(gpsx,buf);   //GNVTG解析
    
    
}


/*
* 函数名称 : Get_Gps_Longitude
* 功能描述 : 获取经度
* 参    数 : 无
* 返回值   : 经度
* 示    例 : Longitude = Get_Gps_Longitude();
*/
/******************************************************************************/
int Get_Gps_Longitude(void)
/******************************************************************************/
{
    return s_Gps.Longitude = s_GpsMsg.longitude;
}

/*
* 函数名称 : Get_Gps_Latitude
* 功能描述 : 获取纬度
* 参    数 : 无
* 返回值   : 纬度
* 示    例 : Latitude = Get_Gps_Latitude();
*/
/******************************************************************************/
int Get_Gps_Latitude(void)
/******************************************************************************/
{
    return s_Gps.Latitude = s_GpsMsg.latitude;
}


void GPS_init_mutex(void) {
    hi_mux_create(&s_Thread_Mux_Id);
}

void GPS_thread_lock(void) {
    hi_mux_pend(s_Thread_Mux_Id, HI_SYS_WAIT_FOREVER);
    return;
}

void GPS_thread_unlock(void) {
    hi_mux_post(s_Thread_Mux_Id);
    return;
}
/*
* 函数名称 : GPS_Init
* 功能描述 : GPS模组初始化
* 参    数 : 无
* 返回值   : 0 - 初始化成功
            -1 - 初始化失败
* 示    例 : result = GPS_Init();
*/
/******************************************************************************/
int GPS_Init(void)
/******************************************************************************/
{
    int ret = 0;
    IoTGpioInit(GPS_UART_RX);
    IoTGpioInit(GPS_UART_TX);
    IoTGpioSetDir(GPS_UART_RX, IOT_GPIO_DIR_OUT);
    IoTGpioSetDir(GPS_UART_TX, IOT_GPIO_DIR_IN);
    hi_io_set_func(GPS_UART_RX, HI_IO_FUNC_GPIO_11_UART2_TXD);
    hi_io_set_func(GPS_UART_TX, HI_IO_FUNC_GPIO_12_UART2_RXD);
    
    hi_uart_attribute uart_attr = {
        .baud_rate = GPS_UART_BAUDRATE, /* baud_rate: 9600 */
        .data_bits = 8,                 /* data_bits: 8bits */
        .stop_bits = 1,
        .parity = 0,
    };
    
    // GPS_init_mutex();               //初始化全局的互斥锁
    /* Initialize uart driver */
    ret = hi_uart_init(GPS_UART_NUM, &uart_attr, HI_NULL);
    if(ret != HI_ERR_SUCCESS) {
        printf("\r\nFailed to init uart%d! Err code = %d\n", GPS_UART_NUM, ret);
        return;
    } else {
        printf("\r\nSuccess to init uart2!!!!\n");
    }
}

int recv_uart(hi_uart_idx id, hi_u8 *data, hi_u32 data_len)
{
	int rsize = 0;
	while(data_len - rsize > 0){
    	int len = hi_uart_read_timeout(GPS_UART_NUM, data + rsize, data_len - rsize, 600);
		if(len > 0){
			rsize += len;
		}else if(len == 0 && rsize != 0){
			break;
		}
	}
	return rsize;
}


/*
* 函数名称 : GPS_Task
* 功能描述 : GPS任务
* 参    数 : arg - 任务参数
* 返回值   : 空
* 示    例 : GPS_Task(&argument);
*/
/******************************************************************************/
void GPS_Task(void *arg)
/******************************************************************************/
{
    uint8 id=0;
    int ret = 0;
    uint8_t data;
    uint16_t i;
    uint8_t recv_data_buf[500] = {0};
    uint8_t send_data_buf[500] = {0};
    uint8_t test[10] = {"12356.456"};
    hi_u8 uart_buff[32] = {0};
    hi_u8 *uart_buff_ptr = recv_data_buf;
    float tp;
    uint8_t dx;
    int temp;
    GPS_Init();  //initialize the BMP280

	// reset
	const uint8_t Reset[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00, 0x07, 0x1F, 0x9E};
	usleep(1000*500);
	hi_uart_write(GPS_UART_NUM, &Reset[0], sizeof(Reset)/sizeof(uint8_t));

	// keeplast
	const uint8_t KeepLast[] = {0xB5, 0x62, 0x06, 0x17, 0x04, 0x00, 0x01, 0x23, 0x00, 0x02, 0x47, 0x58};
	usleep(1000*500);
	hi_uart_write(GPS_UART_NUM, &KeepLast[0], sizeof(KeepLast)/sizeof(uint8_t));

    const uint8_t SetBaud[] = {0xb5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xd0, 0x08, 0x00, 0x00, 0x80, 0x25, 0x00, 0x00, 0x07, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa6, 0xcd, 0xb5, 0x62, 0x06, 0x00, 0x01, 0x00, 0x01, 0x08, 0x22};
    usleep(1000*500);
	hi_uart_write(GPS_UART_NUM, &SetBaud[0], sizeof(SetBaud)/sizeof(uint8_t));
	
	// GPS+北斗
	// const uint8_t Init[] = {0xB5, 0x62, 0x06, 0x3E, 0x2C, 0x00, 0x00, 0x00, 0x20, 0x05,
	// 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00,
	// 0x01, 0x01, 0x03, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00,
	// 0x00, 0x00, 0x01, 0x01, 0x06, 0x08, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x01, 0xFD, 0x25,
	// 0xB5, 0x62, 0x06, 0x17, 0x14, 0x00, 0x00, 0x41, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00,
	// 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x75, 0x57,
	// 0xB5, 0x62, 0x06, 0x17, 0x00, 0x00, 0x1D, 0x5D};
	// usleep(1000*500);
	// hi_uart_write(GPS_UART_NUM, &Init[0], sizeof(Init)/sizeof(uint8_t));

	// GLONASS+北斗
	// const uint8_t Init[] = {0xB5, 0x62, 0x06, 0x3E, 0x3C, 0x00, 0x00, 0x00, 0x20, 0x07,
	// 0x00, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00,
	// 0x01, 0x01, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, 0x03, 0x08, 0x10, 0x00,
	// 0x01, 0x00, 0x01, 0x01, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00, 0x01, 0x01, 0x05, 0x00,
	// 0x03, 0x00, 0x00, 0x00, 0x01, 0x01, 0x06, 0x08, 0x0E, 0x00, 0x01, 0x00, 0x01, 0x01,
	// 0x2D, 0x39};
	// usleep(1000*500);
	// hi_uart_write(GPS_UART_NUM, &Init[0], sizeof(Init)/sizeof(uint8_t));

	// GPS
	// const uint8_t Init[] = {0xB5, 0x62, 0x06, 0x3E, 0x2C, 0x00, 0x00, 0x00, 0x20, 0x05,
	// 0x00, 0x08, 0x10, 0x00, 0x01, 0x00, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00, 0x00, 0x00,
	// 0x01, 0x01, 0x03, 0x08, 0x10, 0x00, 0x00, 0x00, 0x01, 0x01, 0x05, 0x00, 0x03, 0x00,
	// 0x00, 0x00, 0x01, 0x01, 0x06, 0x08, 0x0E, 0x00, 0x00, 0x00, 0x01, 0x01, 0xFC, 0x11};
	// usleep(1000*500);
	// hi_uart_write(GPS_UART_NUM, &Init[0], sizeof(Init)/sizeof(uint8_t));

	for(i = 0; i < 11; i++) {
        usleep(1000*500);
        hi_uart_write(GPS_UART_NUM, &CloseFeature[i][0], 26);
    }

    usleep(1000*500);
    hi_uart_write(GPS_UART_NUM, &OpenFeature[0][0], 26);

	// const uint8_t Sync[] = {0xB5, 0x62, 0x06, 0x31, 0x20, 0x00, 0x00, 0x01, 0x00, 0x00, 0x32,
	// 0x00, 0x00, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x40, 0x42, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00,
	// 0xA0, 0x86, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF7, 0x00, 0x00, 0x00, 0xCA, 0xB6, 0xB5,
	// 0x62, 0x06, 0x31, 0x01, 0x00, 0x00, 0x38, 0xE5};
	// usleep(1000*500);
	// hi_uart_write(GPS_UART_NUM, &Sync[0], sizeof(Sync)/sizeof(uint8_t));

	const uint8_t Save[] = {0xB5, 0x62, 0x06, 0x09, 0x0D, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF,
	0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x31, 0xBF};
	usleep(1000*500);
	hi_uart_write(GPS_UART_NUM, &Save[0], sizeof(Save)/sizeof(uint8_t));

    const uint8_t StartUp[] = {0xB5, 0x62, 0x06, 0x04, 0x04, 0x00, 0x00, 0x00, 0x02, 0x00, 0x10, 0x68};
    usleep(1000*500);
    hi_uart_write(GPS_UART_NUM, &StartUp[0], 12);
	
    while(1) {
        hi_s32 len = recv_uart(GPS_UART_NUM, recv_data_buf, sizeof(recv_data_buf));
        if(len > 0) {
            memcpy(send_data_buf,recv_data_buf,len);
            GPS_Analysis(&s_GpsMsg, send_data_buf);
            
            memset(send_data_buf,0,sizeof(send_data_buf));
            
            printf("\r\n recv data len = %d",len);
            printf("\r\n recv_data = %s",recv_data_buf);
            printf("\r\n latitude = %d , longitude = %d\r\n",s_GpsMsg.latitude, s_GpsMsg.longitude);
        }
        usleep(50);
        
    }
}
/*
* 函数名称 : GPS_Demo
* 功能描述 : GPS示例配置
* 参    数 : 空
* 返回值   : 空
* 示    例 : GPS_Demo();
*/
/******************************************************************************/
void GPS_Demo(void)
/******************************************************************************/
{
    osThreadAttr_t attr;
    
    attr.name       = "GPS-Task";
    attr.attr_bits  = 0U;
    attr.cb_mem     = NULL;
    attr.cb_size    = 0U;
    attr.stack_mem  = NULL;
    attr.stack_size = GPS_TASK_STACK_SIZE;
    attr.priority   = GPS_TASK_PRIO;
    
    if(osThreadNew((osThreadFunc_t)GPS_Task, NULL, &attr) == NULL) {
        printf("\r\n[GPS_Demo] Falied to create GPS Demo!\n");
    } else {
        printf("\r\n[GPS_Demo] Succ to create GPS Demo!\n");
    }
}

APP_FEATURE_INIT(GPS_Demo);
/******************************* End of File (C) ******************************/
