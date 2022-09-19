#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sstream>

#include "httplib.h"
#include "inja/inja.hpp"
#include "xxh3.h"
#include "xxhash.h"

using json = nlohmann::json;
using namespace httplib;

int main(void) {
  Server svr;

  svr.set_mount_point("/www-data", "./www-data");

  // Main index
  svr.Get("/", [&](const httplib::Request& req, httplib::Response& res) {
    res.set_redirect("/H64");
  });

  svr.Get("/H(\\d+)", [&](const Request& req, Response& res) {
    // Load the index Template
    inja::Environment env;
    inja::Template tpl = env.parse_template("./templates/index.tpl.html");

    // Get hashing width
    auto hashWidthMatch = req.matches[1];

    // Stream in
    std::stringstream ss;
    ss << hashWidthMatch;
    // Stream out
    int64_t hashWidth = 0;
    ss >> hashWidth;

    // Check for valid hash value
    if (hashWidth != 32 && hashWidth != 64 && hashWidth != 128) return;

    // Build data
    inja::json pageData;
    pageData["page_title"] = "Hash any string";
    pageData["hash_width"] = hashWidth;

    // Render content
    res.set_content(env.render(tpl, pageData), "text/html");
  });

  // Request Hashed Process
  svr.Post(
      "/H(\\d+)", [&](const httplib::Request& req, httplib::Response& res) {
        // Load the index template
        inja::Environment env;
        inja::Template tpl = env.parse_template("./templates/result.tpl.html");

        // Get hashing width
        auto hashWidthMatch = req.matches[1];

        // Stream in
        std::stringstream css;
        css << hashWidthMatch;
        // Stream out
        int64_t hashWidth = 0;
        css >> hashWidth;

        // Get input
        std::string userStr = req.get_param_value("string_to_hash");

        // Setup string stream for hashing
        std::stringstream ss;

        // Perform hashing
        switch (hashWidth) {
          case 32: {
            auto hash = XXH32(userStr.c_str(), userStr.length(), 0);
            ss << std::hex << std::setw(8) << std::setfill('0') << hash;
            break;
          }
          case 64: {
            auto hash = XXH64(userStr.c_str(), userStr.length(), 0);
            ss << std::hex << std::setw(16) << std::setfill('0') << hash;
            break;
          }
          case 128: {
            auto hash = XXH128(userStr.c_str(), userStr.length(), 0);
            ss << std::hex << std::setw(16) << std::setfill('0') << hash.low64
               << hash.high64;
            break;
          }

          // Stop on any other width
          default:
            return;
        }

        // Build data
        inja::json pageData;
        pageData["page_title"] = "Your hash result!";
        pageData["hash_width"] = hashWidth;
        pageData["string_hash"] = ss.str();

        // Render content
        res.set_content(env.render(tpl, pageData), "text/html");
      });

  svr.Get("/hi", [](const Request& req, Response& res) {
    res.set_content("Hello World!", "text/plain");
  });

  // Stop the server when the user access /stop
  svr.Get("/stop", [&](const Request& req, Response& res) {
    svr.stop();
    res.set_redirect("/");
  });

  // Setup Port, host
  std::ifstream f("j.json");
  json conf = json::parse(f);

  // Listen server to port
  svr.listen(conf["host"], conf["port"]);
}