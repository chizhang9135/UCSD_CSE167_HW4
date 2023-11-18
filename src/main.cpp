#include "hw1.h"
#include "hw2.h"
#include "hw3.h"
#include "timer.h"
#include <vector>
#include <string>

int main(int argc, char *argv[]) {
    std::vector<std::string> parameters;
    std::string hw_num;

    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-hw") {
            hw_num = std::string(argv[++i]);
        } else if (std::string(argv[i]) == "-animation") {
            std::string dir_path = std::string(argv[++i]);
            std::filesystem::path input_path(dir_path);
            std::string output_dir = "out_" + input_path.filename().string();

            // Check and create output directory
            if (!std::filesystem::exists(output_dir)) {
                std::filesystem::create_directory(output_dir);
            }


            // Iterate through all JSON files
            for (const auto &entry : std::filesystem::directory_iterator(dir_path)) {
                if (entry.path().extension() == ".json") {
                    parameters.clear();
                    parameters.push_back(entry.path().string());
                    Image3 img = hw_1_4(parameters);
                    std::filesystem::path output_path = output_dir / entry.path().filename();
                    imwrite(output_path.replace_extension(".png").string(), img);
                }
            }
            return 0;
        } else if (std::string(argv[i]) == "-animation_hw_2") {
            std::string dir_path = std::string(argv[++i]);
            std::filesystem::path input_path(dir_path);
            std::string output_dir = "out_" + input_path.filename().string();

            // Check and create output directory
            if (!std::filesystem::exists(output_dir)) {
                std::filesystem::create_directory(output_dir);
            }


            // Iterate through all JSON files
            for (const auto &entry : std::filesystem::directory_iterator(dir_path)) {
                if (entry.path().extension() == ".json") {
                    parameters.clear();
                    parameters.push_back(entry.path().string());
                    Image3 img = hw_2_4(parameters);
                    std::filesystem::path output_path = output_dir / entry.path().filename();
                    imwrite(output_path.replace_extension(".png").string(), img);
                }
            }
            return 0;

        } else {
            parameters.push_back(std::string(argv[i]));
        }
    }

    if (hw_num == "1_1") {
        Image3 img = hw_1_1(parameters);
        imwrite("hw_1_1.png", img);
    } else if (hw_num == "1_2") {
        Timer timer;
        tick(timer);
        Image3 img = hw_1_2(parameters);
        std::cout << "hw_1_2 took " << tick(timer) << " seconds." << std::endl;
        imwrite("hw_1_2.png", img);
    } else if (hw_num == "1_3") {
        Image3 img = hw_1_3(parameters);
        imwrite("hw_1_3.png", img);
    } else if (hw_num == "1_4") {
        Image3 img = hw_1_4(parameters);
        imwrite("hw_1_4.png", img);
    } else if (hw_num == "1_5") {
        Image3 img = hw_1_5(parameters);
        imwrite("hw_1_5.png", img);
    } else if (hw_num == "1_6") {
        Image3 img = hw_1_6(parameters);
        imwrite("hw_1_6.png", img);
    } else if (hw_num == "1_7") {
        Image3 img = hw_1_7(parameters);
        imwrite("hw_1_7.png", img);
    } else if (hw_num == "2_1") {
        Image3 img = hw_2_1(parameters);
        imwrite("hw_2_1.png", img);
    } else if (hw_num == "2_2") {
        auto start = std::chrono::high_resolution_clock::now();
        Image3 img = hw_2_2(parameters);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "hw_2_2 took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds." << std::endl;
        imwrite("hw_2_2.png", img);
    } else if (hw_num == "2_2_bonus") {
        auto start = std::chrono::high_resolution_clock::now();
        Image3 img = hw_2_2_bonus(parameters);
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "hw_2_2_bonus took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds." << std::endl;
        imwrite("hw_2_2_bonus.png", img);
    } else if (hw_num == "2_3") {
        Image3 img = hw_2_3(parameters);
        imwrite("hw_2_3.png", img);
    } else if (hw_num == "2_4") {
        Image3 img = hw_2_4(parameters);
        imwrite("hw_2_4.png", img);
    } else if (hw_num == "2_5") {
        Image3 img = hw_2_5(parameters);
        imwrite("hw_2_5.png", img);
    } else if (hw_num == "2_1_bonus") {
        Image3 img = hw_2_1_bonus(parameters);
        imwrite("hw_2_1_bonus.png", img);
    } else if (hw_num == "3_1") {
        hw_3_1(parameters);
    } else if (hw_num == "3_2") {
        hw_3_2(parameters);
    } else if (hw_num == "3_3") {
        hw_3_3(parameters);
    } else if (hw_num == "3_4") {
        hw_3_4(parameters);
    } else if (hw_num == "3_5") {
        hw_3_5(parameters);
    } else if (hw_num == "3_3_extra") {
        hw_3_3_extra(parameters);
    } else {
        std::cout << "Invalid hw number." << std::endl;
    }

    return 0;
}
