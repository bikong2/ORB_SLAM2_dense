echo "Configuring and building Thirdparty/DBoW2 ..."

cd Thirdparty/DBoW2
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
cd ../../../

echo "Configuring and building Thirdparty/g2o ..."
cd Thirdparty/g2o
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j

cd ../../../

echo "Uncompress vocabulary ..."

cd Vocabulary
tar -xf ORBvoc.txt.tar.gz
cd ..

echo "Configuring and building ORB_SLAM2 ..."

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
cd ..

#echo "*****************************************"
#echo "**********building ROS examples**********"
#echo "*****************************************"
#cd Examples/ROS/ORB_SLAM2/build
#cmake .. -DROS_BUILD_TYPE=Release
#make -j
#cd ../../../../
