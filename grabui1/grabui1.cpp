// grabui1.cpp : このファイルには 'main' 関数が含まれています。プログラム実行の開始と終了がそこで行われます。
//

#include <iostream>
#include <ctime>
#include <string>
#include <direct.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
#include <GL/gl3w.h>            // Initialize with gl3wInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
#include <GL/glew.h>            // Initialize with glewInit()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
#include <glad/glad.h>          // Initialize with gladLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
#include <glad/gl.h>            // Initialize with gladLoadGL(...) or gladLoaderLoadGL()
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/Binding.h>  // Initialize with glbinding::Binding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
#define GLFW_INCLUDE_NONE       // GLFW including OpenGL headers causes ambiguity or multiple definition errors.
#include <glbinding/glbinding.h>// Initialize with glbinding::initialize()
#include <glbinding/gl/gl.h>
using namespace gl;
#else
#include IMGUI_IMPL_OPENGL_LOADER_CUSTOM
#endif

// カメラ
#include "VICamera.h"

// TCP 送信
#include "TcpSender.h"

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}


// グローバル
int glfwWidth = 640;
int glfwHeight = 480;

int main()
{
    // dataディレクトリ、無かったら直下に作る
    if (_mkdir(".\\data") == 0) {
        std::cout << "data dir is created" << std::endl;
    }
    else {
        std::cout << "maybe data dir exist already" << std::endl;
    }

    //  dataディレクトリへ移動
    if (_chdir(".\\data") == 0) {
        std::cout << "move to data dir" << std::endl;
    }
    else {
        std::cout << "change to data dir failed, images are working dir" << std::endl;
    }


    // Setup window
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(glfwWidth, glfwHeight, "VI camera Dear ImGui", NULL, NULL);

    if (window == NULL)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Window MAX
    //glfwMaximizeWindow(window);

    // Initialize OpenGL loader
#if defined(IMGUI_IMPL_OPENGL_LOADER_GL3W)
    bool err = gl3wInit() != 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLEW)
    bool err = glewInit() != GLEW_OK;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD)
    bool err = gladLoadGL() == 0;
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLAD2)
    bool err = gladLoadGL(glfwGetProcAddress) == 0; // glad2 recommend using the windowing library loader instead of the (optionally) bundled one.
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING2)
    bool err = false;
    glbinding::Binding::initialize();
#elif defined(IMGUI_IMPL_OPENGL_LOADER_GLBINDING3)
    bool err = false;
    glbinding::initialize([](const char* name) { return (glbinding::ProcAddress)glfwGetProcAddress(name); });
#else
    bool err = false; // If you use IMGUI_IMPL_OPENGL_LOADER_CUSTOM, your loader is likely to requires some form of initialization.
#endif
    if (err)
    {
        fprintf(stderr, "Failed to initialize OpenGL loader!\n");
        return 1;
    }


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    // Setup Dear ImGui style
    ImGui::StyleColorsClassic();

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\meiryo.ttc", 36.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    IM_ASSERT(font != NULL);

    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // カメラ初期化
    VICamera camera;

    // TCPサーバ設定
#define IPADDR_SIZE 16
    char ipAddr[IPADDR_SIZE]("192.168.0.30");
    //char ipAddr[IPADDR_SIZE]("127.0.0.1");
    int ipPort = 1000;
    camera.SetIpAddress(ipAddr, ipPort);

    // 照明
    int lightInterval = 1000;
    int lightOnTime = 100;
    char  onCommand[16]("W12010999020999");
    char offCommand[16]("W12010000020000");
    camera.SetCommand(onCommand, offCommand);

    camera.SetLightInterval(lightInterval);
    camera.SetLightOnTime(lightOnTime);

    // カメラ
    int delayShutter = 50;
    double exposureTime = camera.GetDoubleExposureTime();

    // 運転
    bool flagLight = false;
    bool flagCamera = false;
    bool flagSave = false;

    // Main loop
    while (!glfwWindowShouldClose(window))
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // 元のウィンドウサイズを拾う
        glfwGetWindowSize(window, &glfwWidth, &glfwHeight);

        {
            ImGui::SetNextWindowSize(ImVec2(0.f, 0.f));
            ImGui::Begin("Main Window");

            // 照明電源 IP
            ImGui::Text(u8"照明電源ネットワーク");
            ImGui::Text("IP address");
            ImGui::SameLine();
            ImGui::InputText("def 192.168.0.30", ipAddr, IPADDR_SIZE);
            ImGui::Text("IP port");
            ImGui::SameLine();
            ImGui::InputInt("def. 1000", &ipPort);
            if (ImGui::Button("IP set")) {
                camera.TcpClose();
                camera.SetIpAddress(ipAddr, ipPort);
            }

            ImGui::Separator();

            // 照明時間
            ImGui::Text(u8"照明時間");
            ImGui::Text(u8"点灯間隔");
            ImGui::SameLine();
            ImGui::InputInt("ms >= 1000ms", &lightInterval);
            camera.SetLightInterval(lightInterval);

            ImGui::Text(u8"点灯時間");
            ImGui::SameLine();
            ImGui::InputInt("ms", &lightOnTime);
            camera.SetLightOnTime(lightOnTime);

            ImGui::Text(u8"照明時間は点滅開始時のみ反映");
            
            ImGui::Separator();

            // カメラ設定
            ImGui::Text(u8"カメラ設定");
            //camera.DisplayGainSlider();
            if (camera.IsGainDouble()) {
                float gain = (float)camera.GatDoubleGain(); //doubleGain.GetValue();
                float min = (float)camera.GetDoubleGainMin();
                float max = (float)camera.GetDoubleGainMax();
                ImGui::SliderFloat("gain", &gain, min, max, "%.1f");
                ImGui::InputFloat("gain", &gain, 0.1f, 0.5f, "%.1f");

                camera.SetDoubleGain((double)gain); //doubleGain.SetValue((double)gain);
            }
            else {
                int gain = (int)camera.GetIntGain(); //intGain.GetValue();
                int min = (int)camera.GetIntGainMin();
                int max = (int)camera.GetIntGainMax();
                ImGui::SliderInt("gain", &gain, min, max);
                ImGui::InputInt("gain", &gain);

                camera.SetIntGain((int64_t)gain); //intGain.SetValue((int64_t)gain);
            }
            
            ImGui::Text(u8"露出時間");
            ImGui::SameLine();
            ImGui::InputDouble("micro sec", &exposureTime, 100.0, 1000.0, "%.1f", ImGuiInputTextFlags_EnterReturnsTrue); // リタンキーで有効
            if (exposureTime < 0.0) { exposureTime = 100.0; }
            camera.SetDoubleExposureTime(exposureTime);

            ImGui::Separator();

            // 撮影制御
            ImGui::Text(u8"撮影");
            ImGui::Text(u8"電源 %s %d\n照明間隔 %d ms / 点灯時間 %d ms / カメラ待ち %d ms / 露出時間 %.1f us\n点灯パタン [%s]\n消灯パタン [%s]",
                ipAddr, ipPort, lightInterval, lightOnTime, delayShutter, exposureTime, onCommand, offCommand);

            ImGui::Checkbox(u8"照明", &flagLight);
            ImGui::Checkbox(u8"撮影", &flagCamera);
            ImGui::Checkbox(u8"保存", &flagSave);

            // 正確にチェックボックスに入れなくても保存する
            if (ImGui::IsKeyReleased(ImGui::GetKeyIndex(ImGuiKey_Space)) // space key
                || ImGui::IsMouseReleased(ImGuiMouseButton_Right)) { // mouse right button
                //Beep(500, 500);
                flagSave = true;
            }

            // 照明点滅
            if (flagLight) {
                if (!camera.IsGrabbing()) {
                    camera.StartGrabbing(Pylon::GrabStrategy_OneByOne, Pylon::GrabLoop_ProvidedByInstantCamera);
                }
            }
            else {
                if (camera.IsGrabbing()) {
                    camera.StopGrabbing();
                }
            }

            // 撮影する
            camera.GoGrab(flagCamera);

            // 保存する
            if (flagSave) {
                if (camera.IsSaved()) {
                    camera.SaveNow();
                    flagSave = false;
                }
            }

            
            ImGui::End();
        }


        // Rendering
        ImGui::Render();
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);

    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;


}

// プログラムの実行: Ctrl + F5 または [デバッグ] > [デバッグなしで開始] メニュー
// プログラムのデバッグ: F5 または [デバッグ] > [デバッグの開始] メニュー

// 作業を開始するためのヒント: 
//    1. ソリューション エクスプローラー ウィンドウを使用してファイルを追加/管理します 
//   2. チーム エクスプローラー ウィンドウを使用してソース管理に接続します
//   3. 出力ウィンドウを使用して、ビルド出力とその他のメッセージを表示します
//   4. エラー一覧ウィンドウを使用してエラーを表示します
//   5. [プロジェクト] > [新しい項目の追加] と移動して新しいコード ファイルを作成するか、[プロジェクト] > [既存の項目の追加] と移動して既存のコード ファイルをプロジェクトに追加します
//   6. 後ほどこのプロジェクトを再び開く場合、[ファイル] > [開く] > [プロジェクト] と移動して .sln ファイルを選択します
