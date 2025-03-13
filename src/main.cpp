#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "linmath.h"

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <unordered_map>
#include <string>
#include <chrono>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>


#include <vector>
#include <map>
#include <../assimp/Importer.hpp>
#include <../assimp/scene.h>
#include <../assimp/postprocess.h>
#include <../assimp/Exporter.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <../stb_image.h>



#include <filesystem>
#include <zlib.h>
#include <fstream>
#include <unordered_set>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <sstream>

#include <ShlObj.h>

#include <Commdlg.h>
#include <regex>
#include <iostream>

#include "Base.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <corecrt_math_defines.h>

#include <cmath>

#include "../LibGizmo/inc/IGizmo.h"

#include <thread>

#include <fstream>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

std::string enginePath;

struct Project
{
    std::string ProjectDirection;
    std::string ProjectName;
};
Project project;


BaseFunc bf;

namespace fs = std::filesystem;



bool canAffectToViewport;

int VSync = 1;
ImGuiViewport* viewport;


bool CreateProject;


class ConfigManager {
public:
    void load(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        std::string section;

        if (!file.is_open()) {
            std::cerr << "" << filename << std::endl;
            return; // Конфигурация не загружена
        }

        while (std::getline(file, line)) {
            line.erase(remove(line.begin(), line.end(), ' '), line.end());
            if (line.empty() || line[0] == ';') continue;

            if (line[0] == '[' && line.back() == ']') {
                section = line.substr(1, line.size() - 2);
            }
            else {
                size_t pos = line.find('=');
                if (pos != std::string::npos && !section.empty()) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    configData[section + "." + key] = value;
                }
            }
        }
    }

    std::string get(const std::string& key) {
        return configData.count(key) ? configData[key] : "а"; // Проверка существования ключа
    }

    void set(const std::string& key, const std::string& value) {
        configData[key] = value;
    }

    void save(const std::string& filename) {
        std::ofstream file(filename);
        std::string current_section;

        if (!file.is_open()) {

            return; // Не удается сохранить конфигурацию
        }

        for (const auto& [key, value] : configData) {
            std::string section = key.substr(0, key.find('.'));
            if (section != current_section) {
                if (!current_section.empty()) {
                    file << std::endl; // Разделение пустой строкой между секциями
                }
                file << "[" << section << "]" << std::endl;
                current_section = section;
            }
            file << key.substr(key.find('.') + 1) << "=" << value << std::endl;
        }
    }

private:
    std::map<std::string, std::string> configData;
};
ConfigManager configMan;



std::string wcharToString(const wchar_t* wcharString)
{
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wcharString, -1, nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wcharString, -1, &str[0], size_needed, nullptr, nullptr);
    return str;
}

enum fdMode
{
    Models,
    ProjectFiles,
    Images,
    Sounds
};

std::string openFileDialog(fdMode fm__Models__ProjectFiles__Images__Sounds)
{
    OPENFILENAMEW ofn; // Используем OPENFILENAMEW для широких символов
    wchar_t szFile[MAX_PATH]; // Буфер для получения имени файла   
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = L'\0'; // Убираем '0', заменяем на L'\0'
    ofn.nMaxFile = sizeof(szFile);

    switch (fm__Models__ProjectFiles__Images__Sounds) {
    case Models:
        ofn.lpstrFilter =
            L"All Files (*.*)\0*.*\0"
            L"3D Models (*.3d)\0*.3d\0"
            L"3D Studio Max (*.3ds)\0*.3ds\0"
            L"3D Model Format (*.3mf)\0*.3mf\0"
            L"AC3D Files (*.ac)\0*.ac\0"
            L"AC3D Files (*.ac3d)\0*.ac3d\0"
            L"Assimp Model Files (*.assimp)\0*.assimp\0"
            L"Blender Files (*.blend)\0*.blend\0"
            L"3D Model Files (*.b3d)\0*.b3d\0"
            L"Biovision Hierarchy (*.bvh)\0*.bvh\0"
            L"COLLADA Files (*.dae)\0*.dae\0"
            L"DXF Files (*.dxf)\0*.dxf\0"
            L"ENFF Files (*.enff)\0*.enff\0"
            L"FBX Files (*.fbx)\0*.fbx\0"
            L"glTF 1.0 Files (*.gltf;*.glb)\0*.gltf;*.glb\0"
            L"glTF 2.0 Files (*.gltf;*.glb)\0*.gltf;*.glb\0"
            L"HMB Files (*.hmb)\0*.hmb\0"
            L"IFC-STEP Files (*.ifc)\0*.ifc\0"
            L"IQM Files (*.iqm)\0*.iqm\0"
            L"IRR Files (*.irr)\0*.irr\0"
            L"LightWave Object Files (*.lwo)\0*.lwo\0"
            L"LightWave Scene Files (*.lws)\0*.lws\0"
            L"Luxology Files (*.lxo)\0*.lxo\0"
            L"M3D Files (*.m3d)\0*.m3d\0"
            L"MD2 Files (*.md2)\0*.md2\0"
            L"MD3 Files (*.md3)\0*.md3\0"
            L"MD5 Files (*.md5)\0*.md5\0"
            L"MDC Files (*.mdc)\0*.mdc\0"
            L"MDL Files (*.mdl)\0*.mdl\0"
            L"MESH Files (*.mesh;*.mesh.xml)\0*.mesh;*.mesh.xml\0"
            L"MOT Files (*.mot)\0*.mot\0"
            L"MilkShape 3D Files (*.ms3d)\0*.ms3d\0"
            L"NDO Files (*.ndo)\0*.ndo\0"
            L"NFF Files (*.nff)\0*.nff\0"
            L"OBJ Files (*.obj)\0*.obj\0"
            L"OFF Files (*.off)\0*.off\0"
            L"OGEX Files (*.ogex)\0*.ogex\0"
            L"PLY Files (*.ply)\0*.ply\0"
            L"PMX Files (*.pmx)\0*.pmx\0"
            L"PRJ Files (*.prj)\0*.prj\0"
            L"Q3O Files (*.q3o)\0*.q3o\0"
            L"Q3S Files (*.q3s)\0*.q3s\0"
            L"RAW Files (*.raw)\0*.raw\0"
            L"Scene Files (*.scn)\0*.scn\0"
            L"SIB Files (*.sib)\0*.sib\0"
            L"SMD Files (*.smd)\0*.smd\0"
            L"STEP Files (*.stp)\0*.stp\0"
            L"STL Files (*.stl)\0*.stl\0"
            L"TER Files (*.ter)\0*.ter\0"
            L"UC Files (*.uc)\0*.uc\0"
            L"USD Files (*.usd)\0*.usd\0"
            L"VTA Files (*.vta)\0*.vta\0"
            L"DirectX Files (*.x)\0*.x\0"
            L"X3D Files (*.x3d)\0*.x3d\0"
            L"XGL Files (*.xgl)\0*.xgl\0"
            L"ZGL Files (*.zgl)\0*.zgl\0";


        break;
    case ProjectFiles:
        ofn.lpstrFilter = L"Project Files (*.prj)\0*.prj\0";
        break;
    case Images:
        printf("Images");
        break;
    case Sounds:
        printf("Sounds");
        break;
    }



    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = L"";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileNameW(&ofn)) // Используем GetOpenFileNameW
    {
        return wcharToString(ofn.lpstrFile);
    }

    return ""; // Если файл не выбран, возвращаем пустую строку
}

std::string openFolderDialog(fdMode fm__Models__ProjectFiles__Images__Sounds)
{
    char path[MAX_PATH]; // Буфер для пути
    BROWSEINFO bi = { 0 }; // Инициализируем структуру нулями
    bi.hwndOwner = NULL;
    bi.pszDisplayName = path; // Присваиваем указатель на буфер
    bi.lParam = 0;
    bi.ulFlags = BIF_RETURNONLYFSDIRS;

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi); // Открываем диалог

    if (pidl != NULL) {
        if (SHGetPathFromIDListA(pidl, path)) { // Получаем путь от ID списка
            LPMALLOC pMalloc;
            if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
                pMalloc->Free(pidl); // Освобождаем память
                pMalloc->Release();
            }
            switch (fm__Models__ProjectFiles__Images__Sounds) {
            case Models:
                
                break;
            case ProjectFiles:
                CreateProject = true;
                break;
            case Images:
                
                break;
            case Sounds:
                
                break;
            }
            return std::string(path); // Конвертируем в строку
        }

        // Освобождаем память, если путь не получен
        LPMALLOC pMalloc;
        if (SUCCEEDED(SHGetMalloc(&pMalloc))) {
            pMalloc->Free(pidl);
            pMalloc->Release();
        }
    }
    return ""; // Возвращаем пустую строку, если папка не выбрана
}
std::string folderPath;
void CreateNewProject()
{
    folderPath = openFolderDialog(fdMode::ProjectFiles); // Открываем диалог
    if (!folderPath.empty()) {
        
    }
}
std::string GetProjectPathFromFile(const std::string& fileName, const std::string& searchKey) {
    std::string out;
    std::ifstream file(fileName);

    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string key; // Локальная переменная для поиска ключа
            if (iss >> key && key == searchKey) {
                iss >> out; // Читаем значение после ключа
                break;
            }
        }
        file.close();
    }

    return out;
}

std::string filePath;
std::ifstream prFile;
void OpenProject()
{
    filePath = openFileDialog(fdMode::ProjectFiles);
    

    if (!filePath.empty())
    {
        if (prFile.is_open())
        {
            prFile.close();
        }

        prFile.open(project.ProjectDirection + "/" + project.ProjectName + ".prj");

        std::string ProjectName = GetProjectPathFromFile(filePath, "ProjectName:");
        std::string ProjectPath = GetProjectPathFromFile(filePath, "ProjectPath:");
        std::string EditorPath = GetProjectPathFromFile(filePath, "EditorPath:");

        project.ProjectDirection = ProjectPath;
        project.ProjectName = ProjectName;

        configMan.set("LatestProjectSettings.LatestProjectPath", project.ProjectDirection);
        configMan.set("LatestProjectSettings.LatestProjectName", project.ProjectName);
        configMan.save(enginePath + "/configs/engine.ini");

        
    }
}

void OpenModel()
{
    filePath = openFileDialog(fdMode::Models);
    if (!filePath.empty())
    {
        
    }
}


bool dirExists(const std::string& dirName_in)
{
    DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
    if (ftyp == INVALID_FILE_ATTRIBUTES)
        return false;  //something is wrong with your path!
    if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
        return true;   // this is a directory!
    return false;    // this is not a directory!
}

bool fileExists(const std::string& filePath) {
    std::ifstream file(filePath);
    return file.good();
}



const char* title;
std::string AdditionalTitle;
void UpdateTitle(GLFWwindow* window)
{
#ifdef EDITOR_MODE
    if (project.ProjectDirection != "")
    {
        AdditionalTitle = "Editor [" + project.ProjectDirection + "/" + project.ProjectName + ".prj]";
        title = AdditionalTitle.c_str();
        if (!fileExists(project.ProjectDirection + "/" + project.ProjectName + ".prj"))
        {
            exit(ERROR);
        }
        glfwSetWindowTitle(window, title);
    }
    
#endif

#ifdef GAME_MODE
    if (project.ProjectName != "")
    {
        title = project.ProjectName.c_str();
        glfwSetWindowTitle(window, title);
    }
#endif

}

enum DeviceBuild {
    Windows_x64
};

void executeCommand(const std::string& command) {
    system(command.c_str());
    //ShellExecute(NULL, "open", "cmd.exe", "/c " + command.c_str(), NULL, SW_HIDE);
}

void BuildGame(DeviceBuild device)
{
    switch (device) {
    case Windows_x64:
        if (project.ProjectDirection != "" && project.ProjectName != "")
        {
            std::string combinedCommand = "cd /d \"" + project.ProjectDirection + "/build\" & .\\[1]Build.bat & .\\[2]Build.bat";
            std::thread thread(executeCommand, combinedCommand);
            thread.join();
            std::string combinedCommand2 = "cd /d \"" + project.ProjectDirection + "/build\" & .\\[2]Build.bat";
            std::thread thread2(executeCommand, combinedCommand2);
            thread2.join();
        }
        break;
    }
}


int main()
{
    // Получаем текущий путь
    fs::path currentPath = fs::current_path();

    // Спускаемся на 2 уровня ниже
    fs::path newPath = currentPath.parent_path().parent_path().parent_path();

    // Преобразуем в std::string
    enginePath = newPath.string();
    for (char& c : enginePath) {
        if (c == '\\') {
            c = '/';
        }
    }

    if (!fileExists(enginePath + "/configs/engine.ini")) {
        return -1;
    }
    else {
        try {
            configMan.load(enginePath + "/configs/engine.ini");
        }
        catch (const std::out_of_range& e) {
            return -1;
        }
    }

    // Инициализация GLFW
    if (!glfwInit())
        return -1;

    // Получение размера экрана
    int screenWidth, screenHeight;
    GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
    screenWidth = mode->width;
    screenHeight = mode->height;

    
    GLFWmonitor* fullscr;
    


#ifdef EDITOR_MODE
    title = "Editor";
    fullscr = nullptr;

    if (project.ProjectDirection != "")
    {
        AdditionalTitle = "Editor [" + project.ProjectDirection + "]";
        title = AdditionalTitle.c_str();
        
    }
#endif

#ifdef GAME_MODE
    if (project.ProjectName != "")
    {
        title = project.ProjectName.c_str();
    }
    else {
        title = "Game";
    }
    
    fullscr = glfwGetPrimaryMonitor();
#endif

    
    // Создание окна с флагом, чтобы оно было скрытым
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, title, fullscr, NULL);

    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    gladLoadGL(); // Инициализация GLEW
    glfwSetWindowPos(window, 0, 30);

    

	// Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Включить управление с клавиатуры
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Включить управление с геймпадом
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");


    float clear_color[4] = { 0.2f, 0.3f, 0.8f, 1.0f };

    char tempProjectName[128] = "";


    // Главный цикл
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        
        glfwSwapInterval(VSync);
        viewport = ImGui::GetMainViewport();
        UpdateTitle(window);
        // Начало нового кадра ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //Main Bar
       // Creating the main menu bar
        if (ImGui::BeginMainMenuBar()) {
            if (ImGui::BeginMenu("File")) {
                if (ImGui::MenuItem("New")) {
                    CreateNewProject();
                }
                if (ImGui::BeginMenu("Open")) {
                    if (ImGui::MenuItem("Open latest project")) {
                        if (configMan.get("LatestProjectSettings.LatestProjectPath") != "None")
                            project.ProjectDirection = configMan.get("LatestProjectSettings.LatestProjectPath");
                        if (configMan.get("LatestProjectSettings.LatestProjectName") != "None")
                            project.ProjectName = configMan.get("LatestProjectSettings.LatestProjectName");
                    }
                    if (ImGui::MenuItem("Open...")) {
                        OpenProject();
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Save")) {
                    if (ImGui::MenuItem("Save all")) {}
                    if (ImGui::MenuItem("Save aa")) {}
                    if (ImGui::MenuItem("Save all2")) {}
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
               
            }

            if (ImGui::BeginMenu("Build")) {
                if (ImGui::BeginMenu("Build Game For")) {
                    if (project.ProjectDirection != "") {
                        if (ImGui::MenuItem("Windows x64"))
                            BuildGame(Windows_x64);
                    }
                    else {
                        ImGui::MenuItem("You cant build game without project!");
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMenu();
            }

            ImGui::EndMainMenuBar();
        }
        //MainBar

        //Main viewport
        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGuiID id = viewport->ID;
        ImGui::DockSpaceOverViewport(id);

        // ImGui window
        ImGui::Begin("Main Viewport");
        if (ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow) && ImGui::IsWindowFocused(ImGuiFocusedFlags_ChildWindows))
        {
            if (ImGui::IsMouseClicked(1)) {
                canAffectToViewport = !canAffectToViewport; // Устанавливаем флаг, чтобы избежать повторных вызовов
            }
        }
        else if (canAffectToViewport) {
            if (ImGui::IsMouseClicked(1)) {
                canAffectToViewport = !canAffectToViewport; // Устанавливаем флаг, чтобы избежать повторных вызовов
            }
        }
        ImGui::End();

        ImGui::PopStyleColor();

        // Ваш GUI код
        ImGui::Begin("Hello ImGui without Window");
        ImGui::Text("This is an ImGui window without a visible GLFW window!");
        ImGui::End();



        if (CreateProject) {

            ImGui::Begin("Create project page");
            ImGui::InputText("Project Name", tempProjectName, sizeof(tempProjectName));
            ImGui::Text("Editor Camera");
            std::string StrTempPrName = tempProjectName;
            std::regex r("[A-Za-zА-Яа-я0-9]*");
            if (!StrTempPrName.empty()) {
                if (std::regex_match(StrTempPrName, r)) {
                    if (!dirExists(folderPath + "\\" + StrTempPrName))
                    {
                        if (ImGui::Button("Create Project", ImVec2(100, 50))) {
                            

                            CreateProject = false;
                            std::string tempProjectPath = folderPath + "\\" + StrTempPrName;
                            fs::create_directories(tempProjectPath);

                            std::string src_projectdir = folderPath + "\\" + StrTempPrName + "\\src";
                            fs::create_directories(src_projectdir);

                            std::ofstream mFile(src_projectdir + "\\" + StrTempPrName + "_Main.h");
                            mFile << "#include \"Redirections.h\"\n" << std::endl;
                            mFile.close();

                            std::string build_projectdir = folderPath + "\\" + StrTempPrName + "\\build";
                            fs::create_directories(build_projectdir);


                            std::ofstream mBFile(build_projectdir + "\\" + "[1]Build.bat");
                            mBFile << "set SourceDir=\"../../" + StrTempPrName + "\n";
                            mBFile << "set TargetDir=\"" + enginePath + "/Games\"\n";
                            mBFile << "rmdir /s /q %SourceDir%/bin\"\n";
                            mBFile << "xcopy %SourceDir%\" %TargetDir% /e /s /i\n";
                            mBFile << "cd /d \"" + enginePath + "\"\n";
                            mBFile << "build_release_game_SETUP_YOUR_ENGINE_LOCATION.bat\n";
                            mBFile.close();

                            std::string engPathBinGame = enginePath + "\\Bin\\Game";
                            std::ofstream mBFile2(build_projectdir + "\\" + "[2]Build.bat");

                            // Write the contents of the .bat script to the file
                            mBFile2 << "set PROJECT_DIR=\"" + engPathBinGame + "\"" << std::endl;
                            mBFile2 << "set BIN_DIR=\"../../" + StrTempPrName + "/bin\"" << std::endl;
                            mBFile2 << "xcopy %PROJECT_DIR% %BIN_DIR% /e /s /i" << std::endl;

                            // Close the file
                            mBFile2.close();


                            
                            for (char& c : tempProjectPath) {
                                if (c == '\\') {
                                    c = '/';
                                }
                            }

                            std::ofstream prFile(tempProjectPath + "/" + StrTempPrName + ".prj");
                            prFile << "ProjectName: " + StrTempPrName << std::endl;
                            prFile << "ProjectPath: " + tempProjectPath << std::endl;
                            prFile << "EditorPath: " + enginePath << std::endl;
                            prFile.close();
                            
                            //Creating CMake
                             std::ofstream cmakePrFile(tempProjectPath + "\\" + "CMakeLists.txt");
                             cmakePrFile << "cmake_minimum_required(VERSION 3.10)"<< std::endl;
                             cmakePrFile << "project(" + StrTempPrName + " VERSION 1.0)" << std::endl;
                             cmakePrFile << "set(CMAKE_CXX_STANDARD 17)" << std::endl;
                             cmakePrFile << "set(CMAKE_CXX_STANDARD_REQUIRED True)" << std::endl;
                             
                             cmakePrFile << "set(ENGINE_PATH \"" + enginePath + "\")" << std::endl;
                             cmakePrFile << "include_directories(${ENGINE_PATH})" << std::endl;

                             cmakePrFile << "file(GLOB_RECURSE SOURCES" << std::endl;
                             cmakePrFile << "    \"" << "${CMAKE_SOURCE_DIR}" << "/*.cpp\"" << std::endl;
                             cmakePrFile << "    \"" << "${CMAKE_SOURCE_DIR}" << "/*.h\"" << std::endl;
                             cmakePrFile << ")" << std::endl;

                             cmakePrFile << "file(GLOB_RECURSE SOURCES2" << std::endl;
                             cmakePrFile << "    \"" << "${ENGINE_PATH}" << "/Redirections.h\"" << std::endl;
                             cmakePrFile << ")" << std::endl;

                             cmakePrFile << "set(SOURCES" << std::endl;
                             cmakePrFile << "    \"" << "${SOURCES}\"" << std::endl;
                             cmakePrFile << "    \"" << "${SOURCES2}\"" << std::endl;
                             cmakePrFile << ")" << std::endl;

                             cmakePrFile << "add_executable(${PROJECT_NAME} ${SOURCES})" << std::endl;

                             cmakePrFile.close();
                             //Creating CMake

                            fs::create_directories(tempProjectPath + "\\Content");

                            project.ProjectDirection = tempProjectPath;
                            project.ProjectName = StrTempPrName;

                            configMan.set("LatestProjectSettings.LatestProjectPath", project.ProjectDirection);
                            configMan.set("LatestProjectSettings.LatestProjectName", project.ProjectName);
                            configMan.save(enginePath + "/configs/engine.ini");


                        }
                    }
                }

            }
        }

        // Рендеринг
        ImGui::Render();
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window); // Переключение буферов
    }

    // Освобождение ресурсов
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;

}