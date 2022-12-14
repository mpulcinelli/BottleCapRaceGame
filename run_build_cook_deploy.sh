#!/bin/bash
clear

rm -r /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/LinuxNoEditor

ssh mp@192.168.15.182 "rm -r /home/mp/Warpinator/LinuxNoEditor"

#################################################################################
# Para versão de testes e avaliação (Development)
/media/mp/_ue/UE4.27/Engine/Build/BatchFiles/RunUAT.sh BuildCookRun -project="/home/mp/Documents/UEProjects/BottleCapRaceGame/BottleCapRaceGame.uproject" -noP4 -platform=linux -clientconfig=Development -serverconfig=Development -cook -allmaps -build -stage -pak -archive -archivedirectory="/home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild"
#################################################################################

#################################################################################
# Para versão de produção (Shipping)
# /media/mp/_ue/UE4.27/Engine/Build/BatchFiles/RunUAT.sh BuildCookRun -project="/home/mp/Documents/UEProjects/BottleCapRaceGame/BottleCapRaceGame.uproject" -noP4 -platform=linux -clientconfig=Shipping -serverconfig=Shipping -cook -allmaps -build -stage -pak -archive -archivedirectory="/home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild"
#################################################################################

rm /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/LinuxNoEditor/BottleCapRaceGame/Binaries/Linux/BottleCapRaceGame.debug
rm /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/LinuxNoEditor/BottleCapRaceGame/Binaries/Linux/BottleCapRaceGame.sym

cp -T -v /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/BottleCapRaceGame.sh /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/LinuxNoEditor/BottleCapRaceGame.sh

scp -r /home/mp/Documents/UEProjects/BottleCapRaceGame/PackageBuild/LinuxNoEditor/ mp@192.168.15.182:/home/mp/Warpinator/

exit 0