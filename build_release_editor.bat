   mkdir temp
   cd temp
   cmake -DCMAKE_BUILD_TYPE=Release -DEDITOR=ON -DGAME=OFF ..
   cmake --build . --config Release
   cmake --install . --prefix ${CMAKE_INSTALL_PREFIX}
   cd ..
   echo Finished Build Release Editor!
   pause