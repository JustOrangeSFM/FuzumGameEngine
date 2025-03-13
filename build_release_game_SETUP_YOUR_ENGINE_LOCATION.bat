set PROJECT_DIR="D:\VisualStudioProjects\NEW_YEAR_2025\GameEngine
mkdir %PROJECT_DIR%\temp"
cd %PROJECT_DIR%\temp"
cmake -DCMAKE_BUILD_TYPE=Release -DEDITOR=OFF -DGAME=ON ..
cmake --build . --config Release
cmake --install . --prefix ${CMAKE_INSTALL_PREFIX}
rmdir /s /q %PROJECT_DIR%\Games"
echo Finished Build Release Game!
pause