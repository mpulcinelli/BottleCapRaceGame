#!/bin/bash
clear

rm -r /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/LinuxNoEditor

ssh mp@192.168.15.182 "rm -r /home/mp/Warpinator/LinuxNoEditor"

/media/mp/_ue/UE4.27/Engine/Build/BatchFiles/RunUAT.sh BuildCookRun -project="/home/mp/Documents/UEProjects/BottleCapRaceGame/BottleCapRaceGame.uproject" -noP4 -platform=linux -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="/home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild"

cp -T -v /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/BottleCapRaceGame.sh /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/LinuxNoEditor/BottleCapRaceGame.sh

scp -r /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/LinuxNoEditor/ mp@192.168.15.182:/home/mp/Warpinator/

exit 0