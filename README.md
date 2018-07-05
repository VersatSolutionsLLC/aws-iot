# aws-iot

# Build

# Deployment
## Prerequisites
1. Check module is latehed to network.
   To check module's network status try the following commands in ssh termainal
  
   **Command:** `cm radio` 
  
   **Expected Output:**
   ```
   Current Network Operator: 404 31
   Current RAT:           GSM network (LE_MRC_RAT_GSM)
   Status:                Registered, home network (LE_MRC_REG_HOME)
   Signal:                Very strong signal strength (5)
   PS:                    Packet Switched Registered, home network (LE_MRC_REG_HOME)
   ```
   More commands [Radio](https://docs.legato.io/17_08/toolsTarget_cm.html#toolsTarget_cm_radio).
2. Check the module is connected to data

   **Command:** `cm data` 
  
   **Expected Output:**
    ```
    Index:                 1
    APN:                   internet.sierrawireless.com
    PDP Type:              IPV4V6
    Connected:             yes
    Interface:             rmnet_data0
    Family:                inet
    IP:                    100.105.174.180
    Gateway:               100.105.174.181
    Dns1:                  122.160.237.197
    Dns2:                  59.144.127.117
    ```
    More commands [Data](https://docs.legato.io/17_08/toolsTarget_cm.html#toolsTarget_cm_data).
