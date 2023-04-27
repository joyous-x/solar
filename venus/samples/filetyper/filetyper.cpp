// filetyper.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "gtest/gtest.h"
#include "gflags/gflags.h"
#include "./helper.h"
#include "./simple_helper.h"
#include "common/utils/utils.h"
#include "common/3rds/json.h"
#include "../filetype/detector.h"

#if _DEBUG || DEBUG
#pragma comment(lib, "shlwapi")
#pragma comment(lib, "gflags_static_debug")
#else 
#pragma comment(lib, "gflags_static")
#endif

DEFINE_string(message, "", "tools for filetype");
DEFINE_string(cmd, "", "check : check the filetype");
DEFINE_string(filepath, "", "filepath which need to be processed");

std::string usage() {
    std::string usage = "usage:"
        "\n\t[cmd] --cmd=check  --filepath=./doc"
        "\n\nplease use \"[cmd] --helpon=filetyper\" for more information"
        "\n!NOTE\tif filepath is a directory, it should end with backslash(\\).";
    return usage;
}

/*
* https://github.com/gcrowder/programming-language-classifier
* https://github.com/source-foundry/code-corpora/tree/231787181957bd08c880320f40e2a585207341a7
* https://github.com/horsicq/Detect-It-Easy
*/

extern
int get_ftype(ifile* file);

int processor(const char* filepath) {
    std::string msg = "ok";
    int nret = 0;

    struct ftype_item {
        uint32_t ftype;
        std::string msg;
        std::string filepath;

        nlohmann::json to_json() {
            auto cur_json = nlohmann::json{
                {"msg", msg},
                {"path", XsUtils::uni2utf(XsUtils::ansi2unicode(filepath))},
                {"type", ftype},
                {"type_str", filetype::ftype2str(ftype)}, 
            };
            return cur_json;
        }
        std::string to_string() {
            return to_json().dump();
        }
    };

    std::vector<ftype_item> ftypes;
    do {
        std::vector<std::string> files;
        nret = FileUtils::enum_dir(filepath, files);
        if (nret != 0) {
            ftypes.push_back(ftype_item{ 0, "enum files error", filepath });
            break;
        }

        for (uint32_t i = 0, ftype = 0; i < files.size(); i++) {
            ifile* file = openfile(files[i].c_str(), "rb");
            ON_SCOPE_EXIT([&]() { if (file) file->release(); });

            if (file) {
                ftype = get_ftype(file);
                msg = "ok";
            }
            else {
                ftype = 0;
                msg = "open file error";
            }
            ftypes.push_back(ftype_item{ ftype, msg, files[i] });
        }
    } while (false);

    auto cur_json = nlohmann::json{ {"rst", nlohmann::json::array()} };
    for (auto iter : ftypes) {
        cur_json["rst"].push_back(iter.to_json());
    }

    std::wcout << XsUtils::utf2uni(cur_json.dump()) << std::endl;
    return nret;
}

int main(int argc, char* argv[]) {
    gflags::SetVersionString("1.0.0");
    gflags::SetUsageMessage(usage());
    gflags::ParseCommandLineFlags(&argc, &argv, true);

    std::string strCmd = FLAGS_cmd;
    bool show_usage = true;

    if (strCmd == "check") {
        auto nret = processor(std::string(FLAGS_filepath).c_str());
        show_usage = nret != 0;
    }
    else if (strCmd == "gtest") {
        ::testing::InitGoogleTest(&argc, argv);
        return RUN_ALL_TESTS();
    }
    else {
        show_usage = true;
    }

    if (show_usage) {
        std::cout << "\n\n" << gflags::ProgramInvocationShortName() << ": ";
        std::cout << "\t" << FLAGS_message << std::endl;
        std::cout << gflags::ProgramUsage() << "\n\n" << std::endl;
    }
    gflags::ShutDownCommandLineFlags();
    
    return 0;
}