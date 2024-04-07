#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "wifi_device.h"
#include "lwip/netifapi.h"
#include "lwip/api_shell.h"
#include <at.h>
#include <hi_at.h>
#include "hi_wifi_api.h"
#include "lwip/ip_addr.h"

void connect_wifi(const char * wifi_ssid, const char * wifi_password)
{

    printf("in connect_wifi 1\r\n");
    WifiErrorCode errCode;
    WifiDeviceConfig apConfig = {};
    int netId = -1;
    printf("in connect_wifi 2\r\n");

    // setup your AP params
    strcpy(apConfig.ssid, wifi_ssid);     //
    strcpy(apConfig.preSharedKey, wifi_password); //密码
    apConfig.securityType = WIFI_SEC_TYPE_PSK;   //加密方式
    printf("in connect_wifi 3\r\n");

    errCode = EnableWifi();
    errCode = AddDeviceConfig(&apConfig, &netId);
    printf("in connect_wifi 4\r\n");

    errCode = ConnectTo(netId);
    printf("ConnectTo(%d): %d\r\n", netId, errCode);
    usleep(1000 * 1000);
    // 联网业务开始
    // 这里是网络业务代码...
    struct netif *iface = netifapi_netif_find("wlan0");
    if (iface)
    {
        err_t ret = netifapi_dhcp_start(iface); //返回IP地址
        printf("netifapi_dhcp_start: %d\r\n", ret);

        // usleep(2000 * 1000);
        sleep(5);
        ; // wait DHCP server give me IP
        ret = netifapi_netif_common(iface, dhcp_clients_info_show, NULL);
        printf("netifapi_netif_common: %d\r\n", ret);
    }
}