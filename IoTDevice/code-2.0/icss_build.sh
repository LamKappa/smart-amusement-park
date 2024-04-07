###
 # @Description  
 # @Version  1.0
 # @Autor  yuchen
 # @Date  2021-09-13 17:05:09
 # @LastEditors  yuchen
 # @LastEditTime  2021-09-17 19:05:30
### 

sdk_path="./device/hisilicon/hispark_pegasus/sdk_liteos"
sh_name="./icss_build.sh"
firmware_version="1.0.0"

create_product(){
    product_id=$1
    product_path="./vendor/chinasoft"
    python3 ./hilink_auto_generate_code/auto_generation_code.py $product_id
    if [ $? = 0 ];then
        echo "create code success."
        rm -rf $product_path/$product_id
        cp -r $product_path/LYW-S1H1  $product_path/$product_id
        sed -i "s/LYW-S1H1/$product_id/g" $product_path/$product_id/config.json
        sed -i "s/LYW-S1H1/$product_id/g" $product_path/$product_id/BUILD.gn
        sed -i "s/LYW-S1H1/$product_id/g" $product_path/$product_id/hals/utils/token/BUILD.gn
        rm -rf $product_path/$product_id/$product_id
        mv $product_id $product_path/$product_id
        find . -maxdepth  1 -name "$product_id*.json" -exec mv {} $product_path/$product_id/$product_id \;
        find . -maxdepth  1 -name "*.key" -exec mv {} $product_path/$product_id/$product_id \;
        echo "create product success."
        return 0
    fi
    echo "create product fail."
     return 1
}

checkout_product(){
    product_id=$1
    product_path="./vendor/chinasoft/$product_id"
    if [ ! -d "$product_path" ]; then
        echo "product path is no exist!"
    else
    cp -f $product_path/$product_id/hal_sys_param.c $product_path/hals/utils/sys_param/hal_sys_param.c
    echo "copy hal_sys_param.c success."
    cp -f $product_path/$product_id/hal_token.c $product_path/hals/utils/token/hal_token.c
    echo "copy hal_token.c success."
    cp -f $product_path/$product_id/hilink_device_sdk.c ./device/hisilicon/hi3861/sdk_liteos/components/hilink/hilink_device_sdk.c
    echo "copy hilink_device_sdk.c success."
    cp -f $product_path/$product_id/hilink_device.c ./device/hisilicon/hi3861/sdk_liteos/components/hilink/hilink_device.c
    echo "copy hilink_device.c success."
    cp -f $product_path/$product_id/hilink_device.h ./device/hisilicon/hi3861/sdk_liteos/components/hilink/include/hilink_device.h
    echo "copy hilink_device.h success."
    echo "build prepare done."
    hb set
    fi
}

package_product(){
    product_id=$1
    out_path="./out/hi3861/$product_id"
    product_path="./vendor/chinasoft/$product_id"
    if [ ! -d "$product_path" ]; then
        echo "product path is no exist!"
        mkdir product_path
        echo "create product path" 
    fi
    date=` date  +%Y%m%d`
    cp $out_path/Hi3861_wifiiot_app_allinone.bin $product_path/S1H1-$product_id-$firmware_version-$date-UART.bin
    cp $out_path/Hi3861_wifiiot_app_ota.bin $product_path/S1H1-$product_id-$firmware_version-$date-OTA.bin
    cp $product_path/S1H1-$product_id-$firmware_version-$date-OTA.bin $product_path/image2_all_ota1.bin
    cp $product_path/S1H1-$product_id-$firmware_version-$date-OTA.bin $product_path/image2_all_ota2.bin
    sha=$(sha256sum $product_path/S1H1-$product_id-$firmware_version-$date-OTA.bin)
    sha=${sha%% *}
    cat>$product_path/filelist.json<<EOF
{
    "image2_all_ota1.bin":
    {
        "sha256":"$sha"
    },
    "image2_all_ota2.bin":
    {
        "sha256":"$sha"
    }
}
EOF
    mkdir $product_path/package
    mv $product_path/filelist.json $product_path/package
    mv $product_path/image2_all_ota1.bin $product_path/package
    mv $product_path/image2_all_ota2.bin $product_path/package
    cd $product_path
    zip -r package.zip package
    rm -r package
}

menuconfig(){
    cd $sdk_path
    ./build.sh menuconfig
}

help(){
    echo "usage: $sh_name [command] <options>

    menuconfig              set hi3861sdk menuconfig

    all <prodid>            Create and compile penetrate product
    create <prodid>         Create penetrate product by profile
    checkout <prodid>       Checkout product 
    pack <prodid>           Package product
    " 
}

error_echo(){
    echo "Parameter error,Please enter '$sh_name help' to know more."
}

if [ $# = 1 ];then
    if [ $1 = "help" ];then
        help
    elif [ $1 = "menuconfig" ];then
        menuconfig
    else
        error_echo
    fi
elif [ $# = 2 ];then
    if [ $1 = "all" ];then
        create_product $2
        if [ $? = 0 ];then
            checkout_product $2
            echo "Do you want to compile?[yes/no]"
            read compile_set $2
            if [ $compile_set = "yes" ] || [ $compile_set = "y" ] || [ $compile_set = "Yes" ] || [ $compile_set = "Y" ];then
                hb build -f
                package_product $2
            fi
        fi
    elif [ $1 = "create" ];then
        create_product $2
        if [ $? = 0 ];then
            checkout_product $2
        fi
    elif [ $1 = "checkout" ];then
        checkout_product $2
    elif [ $1 = "pack" ];then
        package_product $2
    else
        error_echo
    fi
else
     error_echo
fi