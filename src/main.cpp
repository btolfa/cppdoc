#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
namespace fs = std::filesystem;
#include <algorithm>

#include <fmt/format.h>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <CLI11.hpp>

#include <clang/Frontend/FrontendActions.h>
#include <clang/Tooling/Tooling.h>
#include <clang/Tooling/CompilationDatabase.h>
#include <clang/Tooling/JSONCompilationDatabase.h>

std::unique_ptr<clang::tooling::CompilationDatabase> GenerateFromJsonData(const std::string& jsonData)
{
    std::string errorMsg;

    auto compDb = clang::tooling::JSONCompilationDatabase::loadFromBuffer(jsonData, errorMsg, clang::tooling::JSONCommandLineSyntax::AutoDetect);
    if (!compDb)
    {
        llvm::errs() << errorMsg;
        return nullptr;
    }

    return compDb;
}

std::string GenerateCommandPrefix(
    const std::string& compiler,
    const std::vector<std::string>& clangToolArgs,
    const std::vector<std::string>& systemIncludes,
    const fs::path& headers
)
{
    fmt::memory_buffer out;

    // Compiler
    fmt::format_to(out, compiler);

    // Additional args
    for (const auto& arg : clangToolArgs)
    {
        fmt::format_to(out, " {}", arg);
    }

    // Additional system includes
    for (const auto& path : systemIncludes)
    {
        fmt::format_to(out, R"( -isystem "{}")", fs::absolute(path).string());
    }

    // Add target headers
    fmt::format_to(out, R"( -I "{}")", fs::absolute(headers).string());

    // Parse all comments
    fmt::format_to(out, " -fparse-all-comments");

    // Add mode selection for complier
    fmt::format_to(out, " -x c++-header ");

    return fmt::to_string(out);
}

std::string GenerateJsonCompDatabase(
    const std::string& compiler,
    const std::vector<std::string>& clangToolArgs,
    const std::vector<std::string>& systemIncludes,
    const fs::path& headers,
    const std::vector<std::string>& sourcePathList)
{
    json compilationDatabase;

    const auto commandPrefix = GenerateCommandPrefix(compiler, clangToolArgs, systemIncludes, headers);

    for (const auto& sourcePath : sourcePathList)
    {
        json record;
        fs::path path{ sourcePath };
        record["directory"] = path.parent_path().string();
        record["command"] = commandPrefix + path.string();
        record["file"] = path.string();

        compilationDatabase.push_back(record);
    }

    return compilationDatabase.dump();
}

std::vector<std::string> GenerateSourcePathList(const std::filesystem::path& headers)
{
    std::vector<std::string> result;

    for (auto& directoryEntry : fs::recursive_directory_iterator{ headers })
    {
        const auto& path = directoryEntry.path();
        const auto extension = path.extension().string();
        if (extension == ".h" || extension == ".hpp")
        {
            result.push_back(fs::absolute(path).string());
        }
    }

    return result;
}

int CheckSyntax(
    const clang::tooling::CompilationDatabase& compilationDatabase,
    const std::vector<std::string>& files)
{
    clang::tooling::ClangTool tool(compilationDatabase, files);
    const auto result = tool.run(
        clang::tooling::newFrontendActionFactory<clang::SyntaxOnlyAction>().get());
    if (result != 0)
    {
        std::cerr << "Can't compile input files, please fix compilation error\n";
    }

    return result;
}

int main(int argc, char** argv)
{
    
    CLI::App app{"Headers Checker"};

    std::string headers;
    std::vector<std::string> systemIncludes; // {R"(X:\LLVM\lib\clang\12.0.0\include)"};

    app.add_option("-I,--headers", headers, "path to product headers like include/Aspose.Words.Cpp")
        ->required()
        ->check(CLI::ExistingDirectory);
    app.add_option("-i,--systemInclude", systemIncludes, "Additional system includes like llvm, vs or Windows SDK")
        ->required()
        ->check(CLI::ExistingDirectory);

    CLI11_PARSE(app, argc, argv);

    const std::string compiler = "clang++";
    const std::vector<std::string> clangToolArgs = {"-std=c++14", "-Wno-inconsistent-missing-override"};

    const auto sourcePathList = GenerateSourcePathList(headers);
    const auto compilationDatabaseJson = GenerateJsonCompDatabase(compiler, clangToolArgs, systemIncludes, headers, sourcePathList);
    const auto compilationDatabase = GenerateFromJsonData(compilationDatabaseJson);
    return CheckSyntax(*compilationDatabase, sourcePathList);
}