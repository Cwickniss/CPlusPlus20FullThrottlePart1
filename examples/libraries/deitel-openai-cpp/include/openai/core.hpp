#pragma once
/**
 * @file core.hpp
 * @brief Core building blocks for the header-only OpenAI C++20 teaching SDK.
 *
 * This header defines:
 *   - HTTP primitives (HttpRequest, HttpResponse, HttpHeader)
 *   - OpenAIConfig (API key, base URL, etc.)
 *   - Utility functions for:
 *        * reading / writing binary files
 *        * Base64 encoding/decoding (via cppcodec)
 *        * creating and parsing data: URLs
 *        * guessing MIME types from file extensions
 *   - Multipart/form-data helpers for file uploads
 *   - libcurl plumbing (perform_curl_request)
 *
 * All higher-level APIs (Responses, Images, Audio, Moderations, Videos)
 * are built on top of these primitives in apis.hpp.
 */

#include <string>
#include <string_view>
#include <vector>
#include <map>
#include <optional>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <stdexcept>
#include <algorithm>
#include <utility>
#include <cstdlib>
#include <regex>

#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <cppcodec/base64_rfc4648.hpp>

namespace deitel::openai {

using json = nlohmann::json;

// =====================================================
//  HTTP primitives (for introspection / teaching)
// =====================================================

/**
 * @brief Simple key-value HTTP header.
 *
 * This is used both for requests (outgoing headers) and responses
 * (incoming headers).
 */
struct HttpHeader {
   std::string name;   ///< Header name, e.g. "Content-Type".
   std::string value;  ///< Header value.
};

/**
 * @brief HTTP request representation, used by the SDK.
 *
 * You can inspect these in your teaching code before executing them
 * to show students the raw REST calls: method, URL, headers and body.
 */
struct HttpRequest {
   std::string method;              ///< "GET", "POST", "DELETE", etc.
   std::string url;                 ///< Fully qualified URL.
   std::vector<HttpHeader> headers; ///< Outgoing HTTP headers.
   std::string body;                ///< Raw body (JSON or multipart).
};

/**
 * @brief HTTP response representation, used by the SDK.
 *
 * When you call OpenAIClient::execute or higher-level APIs, the
 * result is ultimately represented by this structure.
 */
struct HttpResponse {
   long status_code {0};            ///< HTTP status code (e.g., 200, 400).
   std::string content_type;        ///< Response Content-Type, if known.
   std::string body;                ///< Raw response body (binary allowed).
   std::vector<HttpHeader> headers; ///< Incoming headers.
};

// =====================================================
//  Configuration
// =====================================================

/**
 * @brief Configuration for OpenAIClient.
 *
 * Typical usage:
 * @code
 * deitel::openai::OpenAIConfig cfg;
 * cfg.api_key = std::getenv("OPENAI_API_KEY");
 * cfg.organization = "org_...";
 * cfg.project = "proj_...";
 * deitel::openai::OpenAIClient client{cfg};
 * @endcode
 */
struct OpenAIConfig {
   /// API key (required). Typically comes from OPENAI_API_KEY.
   std::string api_key;

   /// Base API URL. Default is the standard OpenAI REST endpoint.
   std::string base_url = "https://api.openai.com/v1";

   /// Optional: OpenAI-Organization header.
   std::string organization;

   /// Optional: OpenAI-Project header.
   std::string project;

   /// Timeout for each HTTP request, in seconds.
   long timeout_seconds = 300;
};

// =====================================================
//  Utilities: files, Base64, data URLs, MIME guessing
// =====================================================

/**
 * @namespace deitel::openai::util
 * @brief Utility helpers used across the SDK.
 *
 * These are also convenient for student exercises:
 * - reading/writing binary files
 * - Base64 conversions
 * - data: URLs for images/audio/etc.
 */
namespace util {

using base64 = cppcodec::base64_rfc4648;
namespace fs = std::filesystem;
using ::deitel::openai::json;

// =====================================================
//  Core binary + Base64 helpers (existing API)
// =====================================================

inline std::vector<std::uint8_t>
read_file_bytes(const std::filesystem::path& path)
{
   std::ifstream in(path, std::ios::binary);
   if (!in) {
      throw std::runtime_error(
         "deitel::openai::util::read_file_bytes: unable to open " +
         path.string());
   }

   in.seekg(0, std::ios::end);
   auto size = in.tellg();
   if (size < 0) {
      throw std::runtime_error(
         "deitel::openai::util::read_file_bytes: tellg() failed for " +
         path.string());
   }

   std::vector<std::uint8_t> buffer(
      static_cast<std::size_t>(size));

   in.seekg(0, std::ios::beg);
   if (size > 0) {
      in.read(reinterpret_cast<char*>(buffer.data()),
              static_cast<std::streamsize>(size));
   }

   return buffer;
}

inline void
write_file_bytes(const std::filesystem::path& path,
                 const std::vector<std::uint8_t>& data)
{
   std::ofstream out(path, std::ios::binary);
   if (!out) {
      throw std::runtime_error(
         "deitel::openai::util::write_file_bytes: unable to open " +
         path.string());
   }

   if (!data.empty()) {
      out.write(reinterpret_cast<const char*>(data.data()),
                static_cast<std::streamsize>(data.size()));
   }
}

inline std::string
bytes_to_base64(const std::vector<std::uint8_t>& bytes)
{
   return base64::encode(bytes);
}

inline std::vector<std::uint8_t>
base64_to_bytes(const std::string& b64)
{
   return base64::decode<std::vector<std::uint8_t>>(b64);
}

inline std::string
file_to_base64(const std::filesystem::path& path)
{
   return bytes_to_base64(read_file_bytes(path));
}

inline void
base64_to_file(const std::string& b64,
               const std::filesystem::path& path)
{
   auto bytes = base64_to_bytes(b64);
   write_file_bytes(path, bytes);
}

// =====================================================
//  MIME type helpers (merged core.hpp + utils.hpp)
// =====================================================

inline std::string
guess_mime_type(const std::filesystem::path& path)
{
   std::string ext = path.extension().string();
   std::transform(ext.begin(), ext.end(), ext.begin(),
                  [](unsigned char c) {
                     return static_cast<char>(std::tolower(c));
                  });

   // Images
   if (ext == ".png")  return "image/png";
   if (ext == ".jpg" || ext == ".jpeg") return "image/jpeg";
   if (ext == ".gif")  return "image/gif";
   if (ext == ".webp") return "image/webp";
   if (ext == ".bmp")  return "image/bmp";
   if (ext == ".svg")  return "image/svg+xml";

   // Video
   if (ext == ".mp4")  return "video/mp4";
   if (ext == ".mov")  return "video/quicktime";
   if (ext == ".webm") return "video/webm";

   // Audio
   if (ext == ".mp3")  return "audio/mpeg";
   if (ext == ".wav")  return "audio/wav";
   if (ext == ".ogg")  return "audio/ogg";
   if (ext == ".flac") return "audio/flac";
   if (ext == ".m4a")  return "audio/mp4";

   // Text / data
   if (ext == ".pdf")  return "application/pdf";
   if (ext == ".json") return "application/json";
   if (ext == ".txt")  return "text/plain";
   if (ext == ".vtt")  return "text/vtt";

   return "application/octet-stream";
}

// =====================================================
//  data: URL helpers (existing API)
// =====================================================

inline std::string
bytes_to_data_url(const std::vector<std::uint8_t>& bytes,
                  const std::string& mime_type)
{
   auto b64 = bytes_to_base64(bytes);
   return "data:" + mime_type + ";base64," + b64;
}

inline std::string
file_to_data_url(const std::filesystem::path& path)
{
   auto mime  = guess_mime_type(path);
   auto bytes = read_file_bytes(path);
   return bytes_to_data_url(bytes, mime);
}

inline std::pair<std::string, std::string>
split_data_url(const std::string& data_url)
{
   constexpr std::string_view prefix = "data:";
   constexpr std::string_view marker = ";base64,";

   if (data_url.rfind(prefix, 0) != 0) {
      throw std::invalid_argument(
         "deitel::openai::util::split_data_url: not a data URL "
         "(missing 'data:' prefix)");
   }

   auto marker_pos = data_url.find(marker);
   if (marker_pos == std::string::npos) {
      throw std::invalid_argument(
         "deitel::openai::util::split_data_url: missing ';base64,' segment");
   }

   auto mime_start = prefix.size();
   auto mime_len   = marker_pos - mime_start;
   auto mime_type  = data_url.substr(mime_start, mime_len);
   auto b64        = data_url.substr(marker_pos + marker.size());
   return {mime_type, b64};
}

inline std::vector<std::uint8_t>
data_url_to_bytes(const std::string& data_url,
                  std::string* mime_type_out = nullptr)
{
   auto [mime, b64] = split_data_url(data_url);
   if (mime_type_out) {
      *mime_type_out = std::move(mime);
   }
   return base64_to_bytes(b64);
}

inline void
data_url_to_file(const std::string& data_url,
                 const std::filesystem::path& path)
{
   auto bytes = data_url_to_bytes(data_url, nullptr);
   write_file_bytes(path, bytes);
}

// =====================================================
//  Additional convenience helpers from utils.hpp
// =====================================================

inline fs::path
user_home()
{
   if (const char* home = std::getenv("HOME")) {
      return fs::path{home};
   }
#ifdef _WIN32
   if (const char* userprofile = std::getenv("USERPROFILE")) {
      return fs::path{userprofile};
   }
#endif
   throw std::runtime_error{
      "deitel::openai::util::user_home: HOME/USERPROFILE not set"};
}

inline std::string
read_text_file(const fs::path& path)
{
   std::ifstream in{path};
   if (!in) {
      throw std::runtime_error{
         "deitel::openai::util::read_text_file: unable to open " +
         path.string()};
   }

   std::ostringstream buffer;
   buffer << in.rdbuf();
   return buffer.str();
}

inline std::vector<unsigned char>
read_binary_file(const fs::path& path)
{
   auto bytes = read_file_bytes(path);
   return std::vector<unsigned char>(bytes.begin(), bytes.end());
}

inline void
write_text_file(const fs::path& path,
                std::string_view contents)
{
   std::ofstream out{path};
   if (!out) {
      throw std::runtime_error{
         "deitel::openai::util::write_text_file: unable to open " +
         path.string()};
   }
   out << contents;
}

inline void
write_binary_file(const fs::path& path,
                  const std::vector<unsigned char>& data)
{
   std::vector<std::uint8_t> bytes(data.begin(), data.end());
   write_file_bytes(path, bytes);
}

inline std::string
base64_encode(const std::vector<unsigned char>& data)
{
   std::vector<std::uint8_t> bytes(data.begin(), data.end());
   return bytes_to_base64(bytes);
}

inline std::vector<unsigned char>
base64_decode(const std::string& encoded)
{
   auto bytes = base64_to_bytes(encoded);
   return std::vector<unsigned char>(bytes.begin(), bytes.end());
}

inline std::string
make_data_url(const std::vector<unsigned char>& data,
              const std::string& mime_type)
{
   return "data:" + mime_type + ";base64," + base64_encode(data);
}

inline std::string
make_data_url_from_file(const fs::path& path)
{
   auto bytes = read_binary_file(path);
   auto mime  = guess_mime_type(path);
   return make_data_url(bytes, mime);
}

inline void
save_base64_to_file(const std::string& b64, const fs::path& path)
{
   auto bytes = base64_decode(b64);
   write_binary_file(path, bytes);
}

inline std::string 
first_text_output(const json& response)
{
   // Handle error object if present
   if (response.contains("error") && !response.at("error").is_null()) {
      const auto& err = response.at("error");

      std::string msg = "unknown error";
      std::string type = "error";

      if (err.contains("message") && err.at("message").is_string()) {
         msg = err.at("message").get<std::string>();
      }

      if (err.contains("type") && err.at("type").is_string()) {
         type = err.at("type").get<std::string>();
      }

      throw std::runtime_error{"OpenAI error (" + type + "): " + msg};
   }

   // Validate output
   if (!response.contains("output") ||
       !response.at("output").is_array() ||
       response.at("output").empty()) {
      throw std::runtime_error{
         "first_text_output: response contains no output items"};
   }

   const auto& output = response.at("output");

   // Find the *message* output block (skip reasoning)
   const json* messageBlock = nullptr;
   for (const auto& item : output) {
      if (item.contains("type") &&
          item.at("type").is_string() &&
          item.at("type") == "message") {
         messageBlock = &item;
         break;
      }
   }

   if (!messageBlock) {
      throw std::runtime_error{
         "first_text_output: no message block found in output"};
   }

   // A valid message block MUST have content[]
   if (!messageBlock->contains("content") ||
       !messageBlock->at("content").is_array() ||
       messageBlock->at("content").empty()) {
      throw std::runtime_error{
         "first_text_output: message block contains no content[]"};
   }

   // Look for "output_text" item
   const auto& content0 = messageBlock->at("content").at(0);

   if (content0.contains("text") && content0.at("text").is_string()) {
      return content0.at("text").get<std::string>();
   }

   throw std::runtime_error{
      "first_text_output: no text field found in content"};
}

// Find the first image_generation_call item in a Responses output.
inline const json& first_image_generation_call(const json& response)
{
   if (!response.contains("output") ||
       !response.at("output").is_array()) {
      throw std::runtime_error{
         "first_image_generation_call: response has no output array"};
   }

   const auto& output = response.at("output");

   for (const auto& item : output) {
      if (item.contains("type") &&
          item.at("type").is_string() &&
          item.at("type") == "image_generation_call") {
         return item;
      }
   }

   throw std::runtime_error{
      "first_image_generation_call: no image_generation_call found"};
}

// Get the first base64 image from a non-streaming Responses image_generation call.
inline std::string first_image_base64_output(const json& response)
{
   const json& call = first_image_generation_call(response);

   if (!call.contains("result")) {
      throw std::runtime_error{
         "first_image_base64_output: image_generation_call has no result field"};
   }

   const json& result = call.at("result");

   // Most examples return a single base64 string.
   if (result.is_string()) {
      return result.get<std::string>();
   }

   // Be defensive: if it's an array, take the first string element.
   if (result.is_array() && !result.empty() && result[0].is_string()) {
      return result[0].get<std::string>();
   }

   throw std::runtime_error{
      "first_image_base64_output: result is neither a string nor a non-empty "
      "array of strings"};
}

inline std::string 
strip_leading_whitespace(const std::string& s) {
   return std::regex_replace(s, std::regex(R"(^[ \t]+)", std::regex_constants::multiline), "");
}

} // namespace util


// =====================================================
//  Multipart/form-data helpers
// =====================================================

/**
 * @brief A simple form field for multipart/form-data.
 */
struct MultipartField {
   std::string name;   ///< Field name.
   std::string value;  ///< Field string value.
};

/**
 * @brief A binary file part for multipart/form-data.
 */
struct MultipartFile {
   std::string name;          ///< Form field name.
   std::string filename;      ///< Filename shown to the server.
   std::string content_type;  ///< MIME type of the uploaded file.
   std::vector<std::uint8_t> data; ///< Raw file bytes.
};

/**
 * @brief Generate a pseudo-random multipart boundary string.
 *
 * This function is intentionally simple and deterministic enough
 * for teaching; in production you might want a stronger RNG.
 */
inline std::string random_boundary()
{
   static const char* hex = "0123456789abcdef";
   std::string out = "----openai_cpp_boundary_";
   out.reserve(out.size() + 16);
   for (int i = 0; i < 16; ++i) {
      unsigned char r = static_cast<unsigned char>(std::rand() & 0x0F);
      out.push_back(hex[r]);
   }
   return out;
}

/**
 * @brief Build a multipart/form-data body.
 *
 * @param boundary Boundary string used in the Content-Type header.
 * @param fields   Simple text fields.
 * @param files    Binary file parts.
 * @return A string containing the complete multipart body.
 */
inline std::string build_multipart_body(
   const std::string& boundary,
   const std::vector<MultipartField>& fields,
   const std::vector<MultipartFile>& files)
{
   std::ostringstream body;

   for (const auto& f : fields) {
      body << "--" << boundary << "\r\n";
      body << "Content-Disposition: form-data; name=\""
           << f.name << "\"\r\n\r\n";
      body << f.value << "\r\n";
   }

   for (const auto& file : files) {
      body << "--" << boundary << "\r\n";
      body << "Content-Disposition: form-data; name=\""
           << file.name << "\"; filename=\""
           << file.filename << "\"\r\n";
      body << "Content-Type: " << file.content_type << "\r\n\r\n";
      body.write(reinterpret_cast<const char*>(file.data.data()),
                 static_cast<std::streamsize>(file.data.size()));
      body << "\r\n";
   }

   body << "--" << boundary << "--\r\n";
   return body.str();
}

// =====================================================
//  libcurl plumbing
// =====================================================

/**
 * @brief libcurl write callback for response body.
 */
inline size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
   auto* s = static_cast<std::string*>(userdata);
   s->append(ptr, size * nmemb);
   return size * nmemb;
}

/**
 * @brief libcurl header callback to capture response headers.
 *
 * Each header line is parsed into a HttpHeader {name, value}.
 */
inline size_t header_callback(char* buffer, size_t size, size_t nitems, void* userdata)
{
   auto* hdrs = static_cast<std::vector<HttpHeader>*>(userdata);
   std::string line(buffer, size * nitems);

   auto pos = line.find(':');
   if (pos != std::string::npos) {
      std::string name = line.substr(0, pos);
      std::string value = line.substr(pos + 1);

      auto trim = [](std::string& s) {
         while (!s.empty() && (s.back() == '\r' || s.back() == '\n' || s.back() == ' ')) {
            s.pop_back();
         }
         std::size_t i = 0;
         while (i < s.size() && s[i] == ' ') ++i;
         s.erase(0, i);
      };

      trim(name);
      trim(value);

      if (!name.empty()) {
         hdrs->push_back({std::move(name), std::move(value)});
      }
   }

   return size * nitems;
}

/**
 * @brief RAII wrapper to initialize/cleanup libcurl exactly once.
 *
 * Because this is declared as an inline variable, including this header
 * in multiple translation units is safe in C++17 and later.
 */
struct CurlGlobal {
   CurlGlobal()  { curl_global_init(CURL_GLOBAL_DEFAULT); }
   ~CurlGlobal() { curl_global_cleanup(); }
};

/// Global instance to ensure curl_global_init is called.
inline CurlGlobal curl_global_singleton{};

/**
 * @brief Perform an HTTP request using libcurl.
 *
 * @param req The request to send.
 * @param cfg OpenAIConfig with timeout and other settings.
 * @return HttpResponse containing status code, headers and body.
 *
 * @throws std::runtime_error if libcurl reports an error.
 */
inline HttpResponse perform_curl_request(const HttpRequest& req,
                                         const OpenAIConfig& cfg)
{
   CURL* curl = curl_easy_init();
   if (!curl) {
      throw std::runtime_error("Failed to init curl");
   }

   std::string response_body;
   std::vector<HttpHeader> response_headers;

   curl_easy_setopt(curl, CURLOPT_URL, req.url.c_str());
   curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, req.method.c_str());
   curl_easy_setopt(curl, CURLOPT_TIMEOUT, cfg.timeout_seconds);

   // headers
   struct curl_slist* header_list = nullptr;
   for (const auto& h : req.headers) {
      std::string line = h.name + ": " + h.value;
      header_list = curl_slist_append(header_list, line.c_str());
   }
   curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

   if (req.method == "POST" || req.method == "PUT" || req.method == "PATCH") {
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, req.body.c_str());
      curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, req.body.size());
   }

   curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
   curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_body);

   curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
   curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response_headers);

   CURLcode res = curl_easy_perform(curl);

   HttpResponse out;
   if (res != CURLE_OK) {
      curl_slist_free_all(header_list);
      curl_easy_cleanup(curl);
      throw std::runtime_error(std::string("curl error: ") +
                               curl_easy_strerror(res));
   }

   long status = 0;
   curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &status);
   out.status_code = status;
   out.body = std::move(response_body);
   out.headers = std::move(response_headers);

   char* ct = nullptr;
   curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
   if (ct) out.content_type = ct;

   curl_slist_free_all(header_list);
   curl_easy_cleanup(curl);
   return out;
}


// Find the first tool-related output item for a given tool name.
//
// - For image tools, the Responses API uses type "image_generation_call".
// - For other tools, you may see:
//     { "type": "tool_call", "tool_name": "...", "output": ... }
//
// This helper handles both patterns.
//
// tool_type example values:
//   "image_generation"
//   "code_interpreter"
//   "file_search"
//   "web_search_preview"
//   etc.
inline const json& first_tool_call_output(
   const json& response, std::string_view tool_type)
{
   if (!response.contains("output") ||
       !response.at("output").is_array()) {
      throw std::runtime_error{
         "first_tool_call_output: response has no output array"};
   }

   const auto& output = response.at("output");
   const std::string tool_type_str{tool_type};
   const std::string call_type = tool_type_str + "_call";

   for (const auto& item : output) {
      if (!item.contains("type") || !item.at("type").is_string()) {
         continue;
      }

      const auto& t = item.at("type").get_ref<const std::string&>();

      // 1) Specialized Responses outputs like "image_generation_call"
      if (t == call_type) {
         return item;
      }

      // 2) Generic tool call with explicit tool_name
      if (t == "tool_call" &&
          item.contains("tool_name") &&
          item.at("tool_name").is_string() &&
          item.at("tool_name") == tool_type_str) {
         return item;
      }
   }

   throw std::runtime_error{
      "first_tool_call_output: no tool call found for requested tool type"};
}

// Get the first image payload (base64) from an image tool call
// in a non-streaming Responses result.
//
// This uses first_tool_call_output(response, "image_generation") and then
// reads the "result" field, which is typically:
//   - a base64 string, or
//   - an array of base64 strings (take the first).
inline std::string first_image_output(const json& response)
{
   const json& call =
      first_tool_call_output(response, "image_generation");

   if (!call.contains("result")) {
      throw std::runtime_error{
         "first_image_output: image_generation_call has no result field"};
   }

   const json& result = call.at("result");

   if (result.is_string()) {
      return result.get<std::string>();
   }

   if (result.is_array() &&
       !result.empty() &&
       result.at(0).is_string()) {
      return result.at(0).get<std::string>();
   }

   throw std::runtime_error{
      "first_image_output: result is neither a string nor a non-empty "
      "array of strings"};
}

} // namespace deitel::openai
