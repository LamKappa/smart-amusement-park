# HUKS<a name="EN-US_TOPIC_0000001148528849"></a>

-   [Introduction](#section11660541593)
-   [Directory Structure](#section161941989596)
-   [Repositories Involved](#section1371113476307)

## Introduction<a name="section11660541593"></a>

Harmony Universal KeyStore \(HUKS\) provides key library capabilities for applications, such as key management and cryptographic operations on keys. HUKS also provides APIs for applications to import or generate keys.

HUKS consists of the following modules:

-   HUKS SDK layer: provides HUKS APIs for applications.

-   HUKS service layer: implements functions such as HUKS key management and storage.
-   HUKS engine layer: HUKS core module, which generates, encrypts, and decrypts keys. In a commercial version for L2 devices, this module must run in a secure environment such as a TEE or a chip with security capabilities. A secure environment requires dedicated hardware and is therefore implemented only by emulation in the open-source code.

## Directory Structure<a name="section161941989596"></a>

```
base/security/huks/
├── frameworks                       # Framework code, which is used by interfaces and services
│   └── huks_standard                # HUKS module in a standard system
|   └── huks_lite                    # L0 and L1 code implementation
├── interfaces                       # APIs
│   └── innerkits
│       └── huks_standard
│       └── huks_lite
└── services
    └── huks_standard
```

## Repositories Involved<a name="section1371113476307"></a>

**base/security/huks**
