#pragma once
/**
 * @file openai.hpp
 * @brief Master header for the OpenAI C++20 teaching SDK.
 *
 * Include this file in your applications instead of including
 * core.hpp / requests.hpp / apis.hpp individually.
 *
 * Example:
 * @code
 * #include <openai/openai.hpp>
 *
 * int main() {
 *    deitel::openai::OpenAIConfig cfg;
 *    cfg.api_key = std::getenv("OPENAI_API_KEY");
 *    deitel::openai::OpenAIClient client{cfg};
 *
 *    deitel::openai::ResponsesRequest r;
 *    r.model = "gpt-4.1-mini";
 *    r.input = "Hello from C++!";
 *    auto result = client.responses().create(r);
 * }
 * @endcode
 */

#include "openai/core.hpp"
#include "openai/requests.hpp"
#include "openai/apis.hpp"
