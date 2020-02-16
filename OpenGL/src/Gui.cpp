#include "pch.h"

#include "Gui.h"

#include "imgui/imgui.h"

#include "OpenGL.h"
#include "Shader.h"

namespace detail
{
	static constexpr const char* VertexShader =
        "layout (location = 0) in vec2 Position;\n"
        "layout (location = 1) in vec2 UV;\n"
        "layout (location = 2) in vec4 Color;\n"
        "uniform mat4 ProjMtx;\n"
        "out vec2 Frag_UV;\n"
        "out vec4 Frag_Color;\n"
        "void main()\n"
        "{\n"
        "    Frag_UV = UV;\n"
        "    Frag_Color = Color;\n"
        "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
        "}\n";

    static constexpr const char* FragmentShader =
        "in vec2 Frag_UV;\n"
        "in vec4 Frag_Color;\n"
        "uniform sampler2D Texture;\n"
        "layout (location = 0) out vec4 Out_Color;\n"
        "void main()\n"
        "{\n"
        "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
        "}\n";

    static bool ShowDemoWindow = true;
    static bool ShowOtherWindow = false;
    static ImVec4 ClearColour = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    static double Time = 0.0;

    static GLuint FontTexture = 0;

    static GLuint VboHandle = 0;
    static GLuint ElementsHandle = 0;

    class ImGuiShader : public Shader
    {
    public:
        ImGuiShader() {}
        ImGuiShader(const ImGuiShader& other) {}
        virtual ~ImGuiShader() {}

    public:
        virtual bool Initialize(OpenGL* openGL, HWND hWnd) override
        {
            if (!CompileShader(VertexShader, FragmentShader, openGL, hWnd))
	        {
		        return false;
	        }

	        return true;
        }

        virtual bool SetShaderParameters(OpenGL* openGL, 
									     float* worldMatrix, 
									     float* viewMatrix, 
									     float* projectionMatrix, 
									     int textureUnit = 0,
									     float* lightDirection = nullptr, 
									     float* diffuseLightColour = nullptr,
									     float* ambientLight = nullptr) override
        {
            unsigned int location = openGL->glGetUniformLocation(_shaderProgram, "ProjMtx");
	        if (location == -1)
	        {
		        return false;
	        }
            openGL->glUniformMatrix4fv(location, 1, false, projectionMatrix);
        
            return true;
        }

    protected:
        // does nothing since the specification is in the shader (layout = 0,1,2)
        virtual void BindAttributes(OpenGL* openGL) const override {}

    };

    static ImGuiShader* GuiShader = nullptr;

}

Gui::Gui()
{
}

Gui::Gui(const Gui& other)
{
}

Gui::~Gui()
{
}

bool Gui::Initialize(OpenGL* openGL, HWND hWnd)
{
    if (detail::GuiShader == nullptr && (detail::GuiShader = new detail::ImGuiShader()) == nullptr)
    {
        return false;
    }
    
    if (!detail::GuiShader->Initialize(openGL, hWnd))
    {
        return false;
    }

    // setup ImGUi
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    //g_Window = window;
    detail::Time = 0.0;

    // setup back-end capabilities flags
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;         // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;          // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_glfw";

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    /*io.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
    io.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
    io.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
    io.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
    io.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
    io.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
    io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
    io.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
    io.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = GLFW_KEY_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
    io.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
    io.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
    io.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
    io.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
    io.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;*/

    /*io.SetClipboardTextFn = ImGui_ImplGlfw_SetClipboardText;
    io.GetClipboardTextFn = ImGui_ImplGlfw_GetClipboardText;
    io.ClipboardUserData = g_Window;*/
#if defined(_WIN32)
    io.ImeWindowHandle = hWnd;
#endif

    // Create mouse cursors
    // (By design, on X11 cursors are user configurable and some cursors may be missing. When a cursor doesn't exist,
    // GLFW will emit an error which will often be printed by the app, so we temporarily disable error reporting.
    // Missing cursors will return NULL and our _UpdateMouseCursor() function will use the Arrow cursor instead.)
//    GLFWerrorfun prev_error_callback = glfwSetErrorCallback(NULL);
//    g_MouseCursors[ImGuiMouseCursor_Arrow] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_TextInput] = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_ResizeNS] = glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_ResizeEW] = glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_Hand] = glfwCreateStandardCursor(GLFW_HAND_CURSOR);
//#if GLFW_HAS_NEW_CURSORS
//    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_RESIZE_ALL_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_RESIZE_NESW_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_RESIZE_NWSE_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_NOT_ALLOWED_CURSOR);
//#else
//    g_MouseCursors[ImGuiMouseCursor_ResizeAll] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_ResizeNESW] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_ResizeNWSE] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//    g_MouseCursors[ImGuiMouseCursor_NotAllowed] = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
//#endif
//    glfwSetErrorCallback(prev_error_callback);

    // Chain GLFW callbacks: our callbacks will call the user's previously installed callbacks, if any.
    //g_PrevUserCallbackMousebutton = NULL;
    //g_PrevUserCallbackScroll = NULL;
    //g_PrevUserCallbackKey = NULL;
    //g_PrevUserCallbackChar = NULL;
    //if (install_callbacks)
    //{
    //    g_InstalledCallbacks = true;
    //    g_PrevUserCallbackMousebutton = glfwSetMouseButtonCallback(window, ImGui_ImplGlfw_MouseButtonCallback);
    //    g_PrevUserCallbackScroll = glfwSetScrollCallback(window, ImGui_ImplGlfw_ScrollCallback);
    //    g_PrevUserCallbackKey = glfwSetKeyCallback(window, ImGui_ImplGlfw_KeyCallback);
    //    g_PrevUserCallbackChar = glfwSetCharCallback(window, ImGui_ImplGlfw_CharCallback);
    //}

    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("fonts/roboto-medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/cousine-regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/droidsans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("fonts/proggytiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    openGL->glGenBuffers(1, &detail::VboHandle);
    openGL->glGenBuffers(1, &detail::ElementsHandle);

    BuildFonts();

    // Restore modified GL state
//    glBindTexture(GL_TEXTURE_2D, last_texture);
//    glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
//#ifndef IMGUI_IMPL_OPENGL_ES2
//    glBindVertexArray(last_vertex_array);
//#endif

    //g_ClientApi = client_api;
    return true;
}

void Gui::Shutdown(OpenGL* openGL)
{
    if (detail::GuiShader != nullptr)
    {
        detail::GuiShader->Shutdown(openGL);
        delete detail::GuiShader;
        detail::GuiShader = nullptr;
    }
}

void Gui::Render(OpenGL* openGL, HWND hWnd)
{
    NewFrame(hWnd);

    // some demo code
    if (detail::ShowDemoWindow)
    {
        ImGui::ShowDemoWindow(&detail::ShowDemoWindow);
    }

    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Hello, world!");                          

    ImGui::Text("This is some useful text.");               
    ImGui::Checkbox("Demo Window", &detail::ShowDemoWindow);
    ImGui::Checkbox("Another Window", &detail::ShowOtherWindow);

    ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
    ImGui::ColorEdit3("clear color", (float*)&detail::ClearColour);

    if (ImGui::Button("Button"))
    {
        counter++;
    }
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::End();

    if (detail::ShowOtherWindow)
    {
        ImGui::Begin("Another Window", &detail::ShowOtherWindow);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
        {
            detail::ShowOtherWindow = false;
        }
        ImGui::End();
    }

    // render
    ImGui::Render();


    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);
    GLint displayWidth = dims[2];
    GLint displayHeight = dims[3];

    glViewport(0, 0, displayWidth, displayHeight);

    // rener draw data
    ImDrawData* drawData = ImGui::GetDrawData();

    int fbWidth = (int)(drawData->DisplaySize.x * drawData->FramebufferScale.x);
    int fbHeight = (int)(drawData->DisplaySize.y * drawData->FramebufferScale.y);
    if (fbWidth <= 0 || fbHeight <= 0)
    {
        return;
    }

    openGL->glActiveTexture(GL_TEXTURE0);
    
    //bool clip_origin_lower_left = true;

    // Setup desired GL state
    // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
    // The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
    GLuint vertexArrayHandle = 0;
    openGL->glGenVertexArrays(1, &vertexArrayHandle);
    SetupRenderState(openGL, drawData, fbWidth, fbHeight, vertexArrayHandle);

    // Will project scissor/clipping rectangles into framebuffer space
    ImVec2 clipOff = drawData->DisplayPos;         // (0,0) unless using multi-viewports
    ImVec2 clipScale = drawData->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

    // Render command lists
    for (int n = 0; n < drawData->CmdListsCount; n++)
    {
        const ImDrawList* cmdList = drawData->CmdLists[n];

        // Upload vertex/index buffers
        openGL->glBufferData(GL_ARRAY_BUFFER, (ptrdiff_t)cmdList->VtxBuffer.Size * sizeof(ImDrawVert), (const GLvoid*)cmdList->VtxBuffer.Data, GL_STREAM_DRAW);
        openGL->glBufferData(GL_ELEMENT_ARRAY_BUFFER, (ptrdiff_t)cmdList->IdxBuffer.Size * sizeof(ImDrawIdx), (const GLvoid*)cmdList->IdxBuffer.Data, GL_STREAM_DRAW);

        for (int cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmdList->CmdBuffer[cmd_i];
            if (pcmd->UserCallback != NULL)
            {
                // User callback, registered via ImDrawList::AddCallback()
                // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                    SetupRenderState(openGL, drawData, fbWidth, fbHeight, vertexArrayHandle);
                else
                    pcmd->UserCallback(cmdList, pcmd);
            }
            else
            {
                
                // Project scissor/clipping rectangles into framebuffer space
                ImVec4 clipRect;
                clipRect.x = (pcmd->ClipRect.x - clipOff.x) * clipScale.x;
                clipRect.y = (pcmd->ClipRect.y - clipOff.y) * clipScale.y;
                clipRect.z = (pcmd->ClipRect.z - clipOff.x) * clipScale.x;
                clipRect.w = (pcmd->ClipRect.w - clipOff.y) * clipScale.y;

                

                if (clipRect.x < fbWidth && clipRect.y < fbHeight && clipRect.z >= 0.0f && clipRect.w >= 0.0f)
                {
                    // Apply scissor/clipping rectangle
                    //if (clip_origin_lower_left)
                    //{
                    //    glScissor((int)clipRect.x, (int)(fbHeight - clipRect.w), (int)(clipRect.z - clipRect.x), (int)(clipRect.w - clipRect.y));
                    //}
                    //else
                    //{
                    //    glScissor((int)clipRect.x, (int)clipRect.y, (int)clipRect.z, (int)clipRect.w); // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)
                    //}

                    // Bind texture, Draw
                    glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
#if IMGUI_IMPL_OPENGL_MAY_HAVE_VTX_OFFSET
                    if (g_GlVersion >= 3200)
                        glDrawElementsBaseVertex(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)), (GLint)pcmd->VtxOffset);
                    else
#endif
                    glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, (void*)(intptr_t)(pcmd->IdxOffset * sizeof(ImDrawIdx)));
                }
                
            }
        }
        
    }

    // destroy the temporary VAO
    openGL->glDeleteVertexArrays(1, &vertexArrayHandle);
}

bool Gui::SetupRenderState(OpenGL* openGL, ImDrawData* drawData, int fbWidth, int fbHeight, GLuint vertexArrayHandle)
{
    glEnable(GL_BLEND);
    openGL->glBlendEquation(GL_FUNC_ADD);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    //glEnable(GL_SCISSOR_TEST);
#ifdef GL_POLYGON_MODE
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif

    // Setup viewport, orthographic projection matrix
    // Our visible imgui space lies from drawData->DisplayPos (top left) to drawData->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
    glViewport(0, 0, (GLsizei)fbWidth, (GLsizei)fbHeight);

    float L = drawData->DisplayPos.x;
    float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
    float T = drawData->DisplayPos.y;
    float B = drawData->DisplayPos.y + drawData->DisplaySize.y;
    
    const float orthoProj[4][4] =
    {
        { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
        { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
        { 0.0f,         0.0f,        -1.0f,   0.0f },
        { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
    };

    detail::GuiShader->SetShader(openGL);

    GLuint shaderHandle = detail::GuiShader->GetShaderHandle();
    GLint location = openGL->glGetUniformLocation(shaderHandle, "Texture");
    openGL->glUniform1i(location, 0);

    location = openGL->glGetUniformLocation(shaderHandle, "ProjMtx");
    openGL->glUniformMatrix4fv(location, 1, GL_FALSE, &orthoProj[0][0]);
#ifdef GL_SAMPLER_BINDING
    openGL->glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 may set that otherwise.
#endif

    (void)vertexArrayHandle;  // what is the point of this?????
#ifndef IMGUI_IMPL_OPENGL_ES2
    openGL->glBindVertexArray(vertexArrayHandle);
#endif

    // Bind vertex/index buffers and setup attributes for ImDrawVert
    openGL->glBindBuffer(GL_ARRAY_BUFFER, detail::VboHandle);
    openGL->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, detail::ElementsHandle);

    location = openGL->glGetAttribLocation(shaderHandle, "Position");
    openGL->glEnableVertexAttribArray(location);
    openGL->glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, pos));

    location = openGL->glGetAttribLocation(shaderHandle, "UV");
    openGL->glEnableVertexAttribArray(location);
    openGL->glVertexAttribPointer(location, 2, GL_FLOAT, GL_FALSE, sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, uv));

    location = openGL->glGetAttribLocation(shaderHandle, "Color");
    openGL->glEnableVertexAttribArray(location);
    openGL->glVertexAttribPointer(location, 4, GL_UNSIGNED_BYTE, GL_TRUE,  sizeof(ImDrawVert), (GLvoid*)IM_OFFSETOF(ImDrawVert, col));

    return true;
}

void Gui::NewFrame(HWND hWnd)
{
    ImGuiIO& io = ImGui::GetIO();
    assert(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // setup display size (every frame to accommodate for window resizing)
    RECT rect;
    if (!GetClientRect(hWnd, &rect))
    {
        MessageBox(hWnd, L"Failed to get client rect, huh?", L"Error", MB_OK);

        rect.top = 0;
        rect.bottom = 1280;
        rect.left = 0;
        rect.right = 720;
    }

    int width = (rect.right - rect.left);
    int height = (rect.bottom - rect.top);
    
    GLint dims[4] = {0};
    glGetIntegerv(GL_VIEWPORT, dims);
    GLint displayWidth = dims[2];
    GLint displayHeight = dims[3];

    io.DisplaySize = ImVec2((float)width, (float)height);
    if (width > 0 && height > 0)
    {
        io.DisplayFramebufferScale = ImVec2((float)displayWidth / width, (float)displayHeight / height);
    }

    // Setup time step
    /*double current_time = glfwGetTime();
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f/60.0f);
    g_Time = current_time;*/

    //ImGui_ImplGlfw_UpdateMousePosAndButtons();
    //ImGui_ImplGlfw_UpdateMouseCursor();

    // Update game controllers (if enabled and available)
    //ImGui_ImplGlfw_UpdateGamepads();

    ImGui::NewFrame();
}

void Gui::BuildFonts()
{
    ImGuiIO& io = ImGui::GetIO();
   
    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

    // upload texture to graphics system
    GLint lastTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &lastTexture);
    glGenTextures(1, &detail::FontTexture);
    glBindTexture(GL_TEXTURE_2D, detail::FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
#ifdef GL_UNPACK_ROW_LENGTH
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    io.Fonts->TexID = (ImTextureID)(intptr_t)detail::FontTexture;

    // restore state
    glBindTexture(GL_TEXTURE_2D, lastTexture);
}