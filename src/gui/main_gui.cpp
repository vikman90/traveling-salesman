#include <iostream>
#include <string>
#include <thread>
#include <webview/webview.h>
#include <nlohmann/json.hpp>
#include "app_controller.h"
#include "gui_assets.h"

// Helper to safely parse incoming JS arguments
static nlohmann::json parseReq(const std::string &req) {
    if (req.empty()) return nlohmann::json::object();
    try {
        auto j = nlohmann::json::parse(req);
        if (j.is_array() && !j.empty()) {
            if (j[0].is_string()) {
                try {
                    return nlohmann::json::parse(j[0].get<std::string>());
                } catch (...) {
                    return j[0];
                }
            }
            return j[0];
        }
        return j;
    } catch (...) {
        return nlohmann::json::object();
    }
}

int main(int argc, char **argv) {
    try {
        AppController controller;

        // Create webview window (debug mode enabled)
        webview::webview w(true, nullptr);
        w.set_title("Traveling Salesman Problem - Studio");
        w.set_size(1280, 840, WEBVIEW_HINT_NONE);

        // Bind C++ API functions with native_ prefix to prevent any JS collision
        w.bind("native_getGraphState", [&](const std::string &) -> std::string {
            return controller.getGraphState().dump();
        });

        w.bind("native_generateRandomMap", [&](const std::string &req) -> std::string {
            auto params = parseReq(req);
            int count = params.value("count", 30);
            float width = params.value("width", 800.0f);
            float height = params.value("height", 600.0f);
            std::string mode = params.value("mode", "uniform");
            unsigned int seed = params.value("seed", 0);
            return controller.generateRandomMap(count, width, height, mode, seed).dump();
        });

        w.bind("native_loadTspString", [&](const std::string &req) -> std::string {
            auto params = parseReq(req);
            std::string content = params.is_string() ? params.get<std::string>() : params.value("content", "");
            return controller.loadTspString(content).dump();
        });

        w.bind("native_loadTourString", [&](const std::string &req) -> std::string {
            auto params = parseReq(req);
            std::string content = params.is_string() ? params.get<std::string>() : params.value("content", "");
            return controller.loadTourString(content).dump();
        });

        w.bind("native_getTspContent", [&](const std::string &) -> std::string {
            return controller.getTspContent().dump();
        });

        w.bind("native_getTourContent", [&](const std::string &) -> std::string {
            return controller.getTourContent().dump();
        });

        w.bind("native_updateNodePosition", [&](const std::string &req) -> std::string {
            auto params = parseReq(req);
            int index = params.value("index", -1);
            float x = params.value("x", 0.0f);
            float y = params.value("y", 0.0f);
            return controller.updateNodePosition(index, x, y).dump();
        });

        w.bind("native_addNode", [&](const std::string &req) -> std::string {
            auto params = parseReq(req);
            float x = params.value("x", 0.0f);
            float y = params.value("y", 0.0f);
            return controller.addNode(x, y).dump();
        });

        w.bind("native_clearMap", [&](const std::string &) -> std::string {
            return controller.clearMap().dump();
        });

        w.bind("native_loadPreset", [&](const std::string &req) -> std::string {
            auto params = parseReq(req);
            std::string name = params.is_string() ? params.get<std::string>() : params.value("name", "berlin52");
            return controller.loadPreset(name).dump();
        });

        w.bind("native_runAlgorithm", [&](const std::string &req) -> std::string {
            auto params = parseReq(req);
            return controller.runAlgorithm(params).dump();
        });

        bool isTestMode = (argc > 1 && std::string(argv[1]) == "--test-ui");
        int testExitCode = 0;

        w.bind("native_onTestFinished", [&](const std::string &req) -> std::string {
            auto res = parseReq(req);
            int passed = res.value("passed", 0);
            int failed = res.value("failed", 0);
            std::cout << "[UI-TEST-RUNNER] Results: " << passed << " passed, " << failed << " failed." << std::endl;
            testExitCode = (failed == 0) ? 0 : 1;
            w.terminate();
            return "{\"status\":\"ok\"}";
        });

        // Set embedded HTML UI and start the event loop
        w.set_html(GuiAssets::INDEX_HTML);

        if (isTestMode) {
            w.init("window.addEventListener('DOMContentLoaded', function() { setTimeout(function() { if (typeof runAutomatedSelfTest === 'function') runAutomatedSelfTest(); }, 600); });");
        }

        w.run();

        if (isTestMode) {
            return testExitCode;
        }

    } catch (const webview::exception &e) {
        std::cerr << "Webview initialization failed: " << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << "Application error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
