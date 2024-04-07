# Updater<a name="EN-US_TOPIC_0000001148614629"></a>

-   [Introduction](#section184mcpsimp)
-   [Directory Structure](#section198mcpsimp)
-   [Usage](#section218mcpsimp)
    -   [Usage Guidelines](#section220mcpsimp)

-   [Repositories Involved](#section247mcpsimp)

## Introduction<a name="section184mcpsimp"></a>

The updater runs in the recovery partition. It reads the misc partition information to obtain the update package status and verifies the update package to ensure that the update package is valid. Then, the updater parses the executable program from the update package, creates a subprocess, and starts the update program. After that, update operations will be automatically implemented by the update script.

## Directory Structure<a name="section198mcpsimp"></a>

```
base/update/updater/
├── resources           # UI image resources of the update subsystem
├── services            # Service code of the updater
│   ├── applypatch      # Update package data update code
│   ├── fs_manager      # File system and partition management code
│   ├── include         # Header files for the update subsystem
│   ├── log             # Log module of the update subsystem
│   ├── package         # Update packages
│   ├── script          # Update scripts
│   ├── diffpatch       # Differential package restore code
│   ├── sparse_image    # Sparse image parsing code
│   ├── ui              # UI code
│   └── updater_binary  # Executable programs
├── interfaces
│   └── kits            # External APIs
└── utils               # Common utilities of the update subsystem
    └── include         # Header files for general functions of the update subsystem
```

## Usage<a name="section218mcpsimp"></a>

### Usage Guidelines<a name="section220mcpsimp"></a>

The updater runs in the recovery partition. To ensure proper functioning of the updater, perform the following operations:

1. Create a recovery partition. 

The recovery partition is independent of other partitions. It is recommended that the size of the recovery partition be greater than or equal to 20 MB. The recovery partition image is an ext4 file system. Ensure that the  **config**  option of the ext4 file system in the system kernel is enabled.

2. Create the misc partition.

The misc partition stores metadata required by the update subsystem during the update process. Such data includes update commands and records of resumable data transfer upon power-off. This partition is a raw partition and its size is about 1 MB. You do not need to create a file system for the misc partition, because the update subsystem can directly access this partition.

3. Prepare the partition configuration table.

During the update process, the updater needs to operate the partitions through the partition configuration table. The default file name of the partition configuration table is  **fstab.updater**. You need to pack the  **fstab.updater**  file into the updater during compilation.

4. Start the updater.

The init process in the recovery partition has an independent configuration file named  **init.cfg**. The startup configuration of the updater is stored in this file.

5. Compile the updater.

a. Add the updater configurations to the  **build/subsystem\_config.json**  file.

Example configuration:

```
"updater": {
"project": "hmf/updater",
"path": "base/update/updater",
"name": "updater",
"dir": "base/update"
},
```

b. Add the updater for the desired product.

For example, to add the updater for Hi3516D V300, add the following code to the  **productdefine/common/products/Hi3516DV300.json**  file.

```
     "updater:updater":{},
```

6. Compile the recovery partition image.

Add the compilation configuration to the  **build\_updater\_image.sh**  script, which is stored in the  **build**  repository and called by the OpenHarmony compilation system.

## Repositories Involved<a name="section247mcpsimp"></a>

Update subsystem

**update\_updater**

build

productdefine\_common

