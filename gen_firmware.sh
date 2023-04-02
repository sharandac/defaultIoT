#!/usr/bin/sh
clear
platformio run --target clean --environment esp32dev
platformio run --environment esp32dev
platformio run --target buildfs --environment esp32dev
clear
echo "copy files ..."
cd .pio/build/esp32dev/ ; cp firmware.bin ../../../defaultIoT.bin ; cp spiffs.bin ../../../defaultIoT.spiffs.bin
cd ../../../
# echo "update defaultIoT ..."
# curl --limit-rate 30k -F "file=@-" 192.168.2.106/update < defaultIoT.bin
# echo "... done"
# sleep 60
# curl --limit-rate 30k 192.168.2.106/info