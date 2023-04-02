#!/usr/bin/sh
clear
platformio run --target clean --environment esp32dev
platformio run --environment esp32dev
platformio run --target buildfs --environment esp32dev
clear
echo "copy files ..."
cd .pio/build/esp32dev/ ; cp firmware.bin ../../../defaultIoT.bin ; cp spiffs.bin ../../../defaultIoT.spiffs.bin
cd ../../../
echo "update test lab ..."
curl -F "data=@defaultIoT.bin" 192.168.2.26/update
echo "... done"
echo "update sensor Dirk ..."
curl -F "data=@defaultIoT.bin" 192.168.2.34/update
echo "... done"
echo "update sensor gas ..."
curl -F "data=@defaultIoT.bin" 192.168.2.85/update
echo "... done"
echo "update heater ..."
curl -F "data=@defaultIoT.bin" 192.168.2.86/update
echo "... done"
echo "update sensor Anne ..."
curl -F "data=@defaultIoT.bin" 192.168.2.88/update
echo "... done"
echo "update air sensor Dirk ..."
curl -F "data=@defaultIoT.bin" 192.168.2.108/update
echo "... done"
echo "update air sensor Anne ..."
curl -F "data=@defaultIoT.bin" 192.168.2.25/update
echo "... done"
echo "update air filter dirk ..."
curl -F "data=@defaultIoT.bin" 192.168.2.107/update
echo "... done"
echo "update air filter Anne ..."
curl -F "data=@defaultIoT.bin" 192.168.2.110/update
echo "... done"
sleep 20
curl --limit-rate 30k 192.168.2.26/info
curl --limit-rate 30k 192.168.2.34/info
curl --limit-rate 30k 192.168.2.85/info
curl --limit-rate 30k 192.168.2.86/info
curl --limit-rate 30k 192.168.2.88/info
curl --limit-rate 30k 192.168.2.108/info
curl --limit-rate 30k 192.168.2.25/info
curl --limit-rate 30k 192.168.2.107/info
curl --limit-rate 30k 192.168.2.110/info
