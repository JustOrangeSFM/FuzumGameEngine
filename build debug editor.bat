   mkdir temp
   cd temp
   cmake -DCMAKE_BUILD_TYPE=Debug -DEDITOR=ON -DGAME=OFF ..
   cmake --build . --config Debug
   cmake --install . --prefix ${CMAKE_INSTALL_PREFIX}
   echo Finished Build Debug Editor!
   pause