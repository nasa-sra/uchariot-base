ssh uchariot@$1 "rm ~/uchariot-base/build/uChariotBase"
scp ./uChariotBase uchariot@$1:~/uchariot-base/build
# scp -r ../paths/ uchariot@$1:~/uchariot-base/build/paths
scp -r ../config/ uchariot@$1:~/uchariot-base/
