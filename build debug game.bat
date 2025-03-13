   mkdir temp
   cd temp
   cmake -DCMAKE_BUILD_TYPE=Debug -DEDITOR=OFF -DGAME=ON ..
   cmake --build . --config Debug
   cmake --install . --prefix ${CMAKE_INSTALL_PREFIX}
   echo Finished Build Debug Game!
   pause