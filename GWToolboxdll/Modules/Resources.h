#pragma once

#include <resource.h>
#include <ToolboxModule.h>
#include <Utf8.h>
#include <filesystem>
class Resources : public ToolboxModule {
    Resources() {};
    Resources(const Resources&) = delete;
    ~Resources() {
        if (worker.joinable()) worker.join();
    };
public:
    static Resources& Instance() {
        static Resources instance;
        return instance;
    }

    const char* Name() const override { return "Resources"; }

    void Initialize() override;
    void Terminate() override;

    void Update(float delta) override;
    void DxUpdate(IDirect3DDevice9* device);

    static std::experimental::filesystem::path GetSettingsFolderPath();
    static std::experimental::filesystem::path GetPath(const std::experimental::filesystem::path& file);
    static std::experimental::filesystem::path GetPath(const std::experimental::filesystem::path& folder, const std::experimental::filesystem::path& file);

    static utf8::string GetPathUtf8(std::wstring file);
    static void EnsureFolderExists(const std::experimental::filesystem::path& path);

    // folder should not contain a trailing slash
    void LoadTextureAsync(IDirect3DTexture9** tex, const std::experimental::filesystem::path& path_to_file);
    void LoadTextureAsync(IDirect3DTexture9** tex, const std::experimental::filesystem::path& path_to_file, WORD id);
    void LoadTextureAsyncEx(IDirect3DTexture9 **tex, const std::experimental::filesystem::path& path_to_file);


    // checks if file exists, and downloads from server if it doesn't.
    // If the file exists, executes callback immediately,
    // otherwise execute callback on download completion.
    // folder should not contain a trailing slash
    void EnsureFileExists(const std::experimental::filesystem::path& path_to_file,
        std::function<void(bool)> callback);

    // download to file, blocking
    bool Download(const std::experimental::filesystem::path& path_to_file, const std::wstring& url);
    // download to file, async, calls callback on completion
    void Download(const std::experimental::filesystem::path& path_to_file, const std::wstring& url,
        std::function<void(bool)> callback);

    // download to memory, blocking
    std::string Download(const std::wstring& url) const;
    // download to memory, async, calls callback on completion
    void Download(const std::wstring& url, 
        std::function<void(std::string file)> callback);

    void EnqueueWorkerTask(std::function<void()> f) { thread_jobs.push(f); }
    void EnqueueMainTask(std::function<void()> f) { todo.push(f); }

    // Stops the worker thread once it's done with the current jobs.
    void EndLoading();

private:
    // tasks to be done async by the worker thread
    std::queue<std::function<void()>> thread_jobs;

    // tasks to be done in the render thread
    std::queue<std::function<void(IDirect3DDevice9*)>> toload;

    // tasks to be done in main thread
    std::queue<std::function<void()>> todo;

    bool should_stop = false;
    std::thread worker;
};
