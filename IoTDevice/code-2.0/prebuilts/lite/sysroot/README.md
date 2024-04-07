# sysroot<a name="EN-US_TOPIC_0000001096759155"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Constraints](#section119744591305)
-   [Compilation and Building](#section137768191623)
-   [Usage](#section68313135353)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

**sysroot**  is a root directory used by the Clang compiler to search for standard libraries and header files. The libc library is generated from the open-source musl library by compilation.

## Directory Structure<a name="section161941989596"></a>

```
/prebuilts/lite/sysroot
├── build                   # Toolchain building (including build scripts)
├── thirdparty              # Temporary third-party header files required for toolchain building
├── usr                     # C library and header files exposed externally
│   ├── include             # Header files exposed externally
│   │  └── arm-liteos       # Chip architecture for the toolchain
│   └── lib                 # C library exposed externally
│       └── arm-liteos      # Chip architecture for the toolchain
```

## Constraints<a name="section119744591305"></a>

**sysroot**  applies only to the OpenHarmony kernel.

## Compilation and Building<a name="section137768191623"></a>

When bugs in the musl library are fixed or the version is updated, you need to compile and build a new libc library by executing  **thirdparty\_headers.sh**  and  **build\_musl\_clang.sh**  scripts in the  **build**  directory, respectively. The new libc library will be stored in the  **/prebuilts/lite/sysroot/build/usr**  directory. Then, you need to replace the header files and libc library in the  **/prebuilts/lite/sysroot/usr**  directory.

## Usage<a name="section68313135353"></a>

-   Add the bin directory to the PATH environment variable. For details about how to set up the compilation environment, see  [Setting Up the Hi3518 Development Environment](https://gitee.com/openharmony/docs/blob/master/en/device-dev/quick-start/setting-up-the-hi3518-development-environment.md)  and  [Setting Up the Hi3516 Development Environment](https://gitee.com/openharmony/docs/blob/master/en/device-dev/quick-start/setting-up-the-hi3516-development-environment.md).
-   The following is a sample script for compiling the  **helloworld.c**  program:

```
clang -o helloworld helloworld.c -target arm-liteos -L ~/llvm/lib/clang/9.0.0/lib/arm-liteos/a7_softfp_neon-vfpv4 --sysroot=/usr/xxx/OS/prebuilts/lite/sysroot/
```

The compiler directory is  **\~/llvm**.

## Repositories Involved<a name="section1371113476307"></a>

[Kernel subsystem](https://gitee.com/openharmony/docs/blob/master/en/readme/kernel.md)

**prebuilts\_lite\_sysroot**

