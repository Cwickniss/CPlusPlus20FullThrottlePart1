#pragma once
/**
 * @file apis.hpp
 * @brief High-level OpenAIClient and nested API classes.
 *
 * This header defines:
 *  - OpenAIClient: central entry point with configuration
 *  - ResponsesAPI, ImagesAPI, ModerationsAPI, AudioAPI, SpeechAPI,
 *    TranscriptionsAPI, VideosAPI
 */

#include <string>
#include <stdexcept>
#include <optional>
#include <sstream>
#include <vector>

#include <nlohmann/json.hpp>

#include "core.hpp"
#include "requests.hpp"

namespace deitel::openai {

using json = nlohmann::json;

class ResponsesAPI;
class ImagesAPI;
class ModerationsAPI;
class AudioAPI;
class SpeechAPI;
class TranscriptionsAPI;
class VideosAPI;

// =====================================================
//  OpenAIClient
// =====================================================

/**
 * @brief Main entry point for using the OpenAI C++ teaching SDK.
 *
 * Create an instance of OpenAIClient with your OpenAIConfig and use
 * its nested API helpers:
 *
 *  - ResponsesAPI:   client.responses().create(...)
 *  - ImagesAPI:      client.images().generate(...)
 *  - ModerationsAPI: client.moderations().create(...)
 *  - AudioAPI:
 *        * client.audio().speech().create(...)
 *        * client.audio().transcriptions().create_raw/create_json(...)
 *  - VideosAPI:      client.videos().create(), .retrieve(), .download(...)
 */
class OpenAIClient {
public:
   /**
    * @brief Construct with an explicit configuration.
    */
   explicit OpenAIClient(OpenAIConfig config)
      : config_(std::move(config))
   {
      if (config_.api_key.empty()) {
         throw std::runtime_error("OpenAIConfig.api_key is required");
      }
   }

   /**
    * @brief Default constructor that reads OPENAI_API_KEY from the environment.
    *
    * Throws std::runtime_error if the environment variable is missing.
    */
   OpenAIClient()
   {
      const char* key = std::getenv("OPENAI_API_KEY");
      if (!key || std::string(key).empty()) {
         throw std::runtime_error(
            "OPENAI_API_KEY environment variable is not set");
      }
      config_.api_key = key;
   }

   /**
    * @brief Access immutable configuration.
    */
   const OpenAIConfig& config() const { return config_; }

   /**
    * @brief Access mutable configuration (advanced usage).
    */
   OpenAIConfig& mutable_config() { return config_; }

   /**
    * @brief Execute an HTTP request using libcurl.
    */
   HttpResponse execute(const HttpRequest& req) const {
      return perform_curl_request(req, config_);
   }

   /// Access the Responses API (client.responses().create(...)).
   ResponsesAPI responses();

   /// Access the Images API (client.images().generate(...)).
   ImagesAPI images();

   /// Access the Moderations API (client.moderations().create(...)).
   ModerationsAPI moderations();

   /// Access the Audio API (client.audio().speech() / .transcriptions()).
   AudioAPI audio();

   /// Access the Videos API (client.videos().create(...) etc.).
   VideosAPI videos();

   /// Attach common headers (Authorization, OpenAI-Project, etc.).
   void add_common_headers(HttpRequest& req,
                           const std::string& content_type = "application/json") const
   {
      // Authorization header
      req.headers.push_back(HttpHeader{
         "Authorization",
         "Bearer " + config_.api_key
      });

      // Content-Type header (if requested)
      if (!content_type.empty()) {
         req.headers.push_back(HttpHeader{
            "Content-Type",
            content_type
         });
      }

      // Optional organization
      if (!config_.organization.empty()) {
         req.headers.push_back(HttpHeader{
            "OpenAI-Organization",
            config_.organization
         });
      }

      // Optional project
      if (!config_.project.empty()) {
         req.headers.push_back(HttpHeader{
            "OpenAI-Project",
            config_.project
         });
      }

      // User agent for introspection
      req.headers.push_back(HttpHeader{
         "User-Agent",
         "openai-cpp-teaching-sdk/0.1"
      });
   }

private:
   OpenAIConfig config_;

   friend class ResponsesAPI;
   friend class ImagesAPI;
   friend class ModerationsAPI;
   friend class AudioAPI;
   friend class SpeechAPI;
   friend class TranscriptionsAPI;
   friend class VideosAPI;
};

// =====================================================
//  ResponsesAPI
// =====================================================

/**
 * @brief API wrapper for /v1/responses.
 *
 * Call via:
 *  - client.responses().create(...)
 */
class ResponsesAPI {
public:
   explicit ResponsesAPI(OpenAIClient& client) : client_(client) {}

   /**
    * @brief Build the underlying HttpRequest for a Responses API call.
    *
    * This is useful for teaching and introspection.
    */
   HttpRequest create_request(const ResponsesRequest& r) const {
      json body;
      body["model"] = r.model;
      body["input"] = r.input;

      if (r.instructions)         body["instructions"]         = *r.instructions;
      if (r.metadata)             body["metadata"]             = *r.metadata;
      if (r.temperature)          body["temperature"]          = *r.temperature;
      if (r.top_p)                body["top_p"]                = *r.top_p;
      if (r.max_output_tokens)    body["max_output_tokens"]    = *r.max_output_tokens;
      if (r.previous_response_id) body["previous_response_id"] = *r.previous_response_id;
      if (r.reasoning)            body["reasoning"]            = *r.reasoning;
      if (r.text)                 body["text"]                 = *r.text;
      if (r.tools)                body["tools"]                = *r.tools;
      if (r.tool_choice)          body["tool_choice"]          = *r.tool_choice;
      if (r.truncation)           body["truncation"]           = *r.truncation;
      if (r.include)              body["include"]              = *r.include;
      if (r.parallel_tool_calls)  body["parallel_tool_calls"]  = *r.parallel_tool_calls;
      if (r.stream)               body["stream"]               = *r.stream;
      if (r.audio)                body["audio"]                = *r.audio;
      if (r.store)                body["store"]                = *r.store;
      if (r.user)                 body["user"]                 = *r.user;
      if (r.service_tier)         body["service_tier"]         = *r.service_tier;

      // Extra fields override if keys collide.
      for (auto it = r.extra.begin(); it != r.extra.end(); ++it) {
         body[it.key()] = it.value();
      }

      HttpRequest req;
      req.method = "POST";
      req.url    = client_.config_.base_url + "/responses";
      req.body   = body.dump();
      client_.add_common_headers(req, "application/json");
      return req;
   }

   /**
    * @brief Make a Responses API call and parse the JSON response.
    */
   json create(const ResponsesRequest& r) const {
      auto req = create_request(r);
      auto res = client_.execute(req);
      if (res.status_code / 100 != 2) {
         throw std::runtime_error("Responses API error: " + res.body);
      }
      return json::parse(res.body);
   }

private:
   OpenAIClient& client_;
};

// =====================================================
//  ImagesAPI
// =====================================================

class ImagesAPI {
public:
   explicit ImagesAPI(OpenAIClient& client) : client_(client) {}

   // --------- Text → Image (generations) ----------------

   HttpRequest generate_request(const ImagesGenerateRequest& r) const {
      json body;
      body["model"]  = r.model;
      body["prompt"] = r.prompt;

      if (r.n)               body["n"]               = *r.n;
      if (r.size)            body["size"]            = *r.size;
      if (r.quality)         body["quality"]         = *r.quality;
      if (r.style)           body["style"]           = *r.style;
      if (r.response_format) body["response_format"] = *r.response_format;
      if (r.user)            body["user"]            = *r.user;

      // Extra arbitrary fields
      for (auto it = r.extra.begin(); it != r.extra.end(); ++it) {
         body[it.key()] = it.value();
      }

      HttpRequest req;
      req.method = "POST";
      // Official endpoint: /v1/images/generations
      req.url    = client_.config_.base_url + "/images/generations";
      req.body   = body.dump();
      client_.add_common_headers(req, "application/json");
      return req;
   }

   json generate(const ImagesGenerateRequest& r) const {
      auto req = generate_request(r);
      auto res = client_.execute(req);
      if (res.status_code / 100 != 2) {
         throw std::runtime_error("Images API error: " + res.body);
      }
      return json::parse(res.body);
   }

   // --------- Image → Image (edits) ---------------------

   HttpRequest edit_request(const ImageEditRequest& r) const {
      using std::filesystem::path;

      const std::string boundary = random_boundary();

      // Text fields
      std::vector<MultipartField> fields;
      fields.push_back(MultipartField{
         .name  = "model",
         .value = r.model
      });

      if (r.prompt)          fields.push_back({"prompt", *r.prompt});
      if (r.n)               fields.push_back({"n", std::to_string(*r.n)});
      if (r.size)            fields.push_back({"size", *r.size});
      if (r.quality)         fields.push_back({"quality", *r.quality});
      if (r.style)           fields.push_back({"style", *r.style});
      if (r.output_format) fields.push_back({"output_format", *r.output_format});
      if (r.user)            fields.push_back({"user", *r.user});

      // Extra arbitrary fields (values serialized as JSON)
      for (auto it = r.extra.begin(); it != r.extra.end(); ++it) {
         fields.push_back(MultipartField{
            .name  = it.key(),
            .value = it.value().dump()
         });
      }

      // File parts: image (required) + optional mask
      std::vector<MultipartFile> files;

      // Main image
      {
         path imagePath{r.image_path};
         MultipartFile imageFile;
         imageFile.name         = "image";
         imageFile.filename     = imagePath.filename().string();
         imageFile.content_type = util::guess_mime_type(imagePath);
         imageFile.data         = util::read_file_bytes(imagePath);
         files.push_back(std::move(imageFile));
      }

      // Optional mask
      if (r.mask_path) {
         path maskPath{*r.mask_path};
         MultipartFile maskFile;
         maskFile.name         = "mask";
         maskFile.filename     = maskPath.filename().string();
         maskFile.content_type = util::guess_mime_type(maskPath);
         maskFile.data         = util::read_file_bytes(maskPath);
         files.push_back(std::move(maskFile));
      }

      HttpRequest req;
      req.method = "POST";
      // Official endpoint: /v1/images/edits
      req.url    = client_.config_.base_url + "/images/edits";
      req.body   = build_multipart_body(boundary, fields, files);
      client_.add_common_headers(
         req, "multipart/form-data; boundary=" + boundary);
      return req;
   }

   json edit(const ImageEditRequest& r) const {
      auto req = edit_request(r);
      auto res = client_.execute(req);
      if (res.status_code / 100 != 2) {
         throw std::runtime_error("Images API error: " + res.body);
      }
      return json::parse(res.body);
   }

private:
   OpenAIClient& client_;
};

// =====================================================
//  ModerationsAPI
// =====================================================

class ModerationsAPI {
public:
   explicit ModerationsAPI(OpenAIClient& client) : client_(client) {}

   HttpRequest create_request(const ModerationRequest& r) const {
      json body;
      body["model"] = r.model;
      body["input"] = r.input;

      for (auto it = r.extra.begin(); it != r.extra.end(); ++it) {
         body[it.key()] = it.value();
      }

      HttpRequest req;
      req.method = "POST";
      req.url    = client_.config_.base_url + "/moderations";
      req.body   = body.dump();
      client_.add_common_headers(req, "application/json");
      return req;
   }

   json create(const ModerationRequest& r) const {
      auto req = create_request(r);
      auto res = client_.execute(req);
      if (res.status_code / 100 != 2) {
         throw std::runtime_error("Moderations API error: " + res.body);
      }
      return json::parse(res.body);
   }

private:
   OpenAIClient& client_;
};

// =====================================================
//  SpeechAPI
// =====================================================

/**
 * @brief API wrapper for /v1/audio/speech (Text-to-Speech).
 */
class SpeechAPI {
public:
   explicit SpeechAPI(OpenAIClient& client) : client_(client) {}

   HttpRequest create_request(const AudioSpeechRequest& r) const {
      json body;
      body["model"] = r.model;
      body["instructions"] = r.instructions;
      body["input"] = r.input;
      body["voice"] = r.voice;

      if (r.format) {
         body["format"] = *r.format;
      }

      for (auto it = r.extra.begin(); it != r.extra.end(); ++it) {
         body[it.key()] = it.value();
      }

      HttpRequest req;
      req.method = "POST";
      req.url    = client_.config_.base_url + "/audio/speech";
      req.body   = body.dump();
      client_.add_common_headers(req, "application/json");
      return req;
   }

   std::vector<unsigned char> create(const AudioSpeechRequest& r) const {
      auto req = create_request(r);
      auto res = client_.execute(req);
      if (res.status_code / 100 != 2) {
         throw std::runtime_error("Audio Speech API error: " + res.body);
      }
      return std::vector<unsigned char>(res.body.begin(), res.body.end());
   }

private:
   OpenAIClient& client_;
};

// =====================================================
//  TranscriptionsAPI
// =====================================================

/**
 * @brief API wrapper for /v1/audio/transcriptions.
 *
 * NOTE: multipart handling is simplified for teaching purposes.
 */
class TranscriptionsAPI {
public:
   explicit TranscriptionsAPI(OpenAIClient& client) : client_(client) {}

   HttpRequest create_request(const AudioTranscriptionRequest& r) const {
      HttpRequest req;
      req.method = "POST";
      req.url    = client_.config_.base_url + "/audio/transcriptions";

      // Use the helper from core.hpp
      const std::string boundary = random_boundary();

      // ---------- 1. Simple text fields ----------

      std::vector<MultipartField> fields;
      fields.push_back(MultipartField{
         .name  = "model",
         .value = r.model
      });

      if (r.language) {
         fields.push_back(MultipartField{
            .name  = "language",
            .value = *r.language
         });
      }

      if (r.prompt) {
         fields.push_back(MultipartField{
            .name  = "prompt",
            .value = *r.prompt
         });
      }

      if (r.response_format) {
         fields.push_back(MultipartField{
            .name  = "response_format",
            .value = *r.response_format
         });
      }

      if (r.temperature) {
         fields.push_back(MultipartField{
            .name  = "temperature",
            .value = std::to_string(*r.temperature)
         });
      }

      // Extra arbitrary fields (serialize values as JSON strings)
      for (auto it = r.extra.begin(); it != r.extra.end(); ++it) {
         fields.push_back(MultipartField{
            .name  = it.key(),
            .value = it.value().dump()
         });
      }

      // ---------- 2. File part (actual bytes) ----------

      namespace fs = std::filesystem;
      fs::path path{r.file_path};

      MultipartFile filePart;
      filePart.name         = "file";  // field name required by API
      filePart.filename     = path.filename().string();
      filePart.content_type = util::guess_mime_type(path);
      filePart.data         = util::read_file_bytes(path);

      std::vector<MultipartFile> files;
      files.push_back(std::move(filePart));

      // ---------- 3. Build body & headers ----------

      req.body = build_multipart_body(boundary, fields, files);

      client_.add_common_headers(
         req, "multipart/form-data; boundary=" + boundary);

      return req;
   }

   std::string create(const AudioTranscriptionRequest& r) const {
      auto req = create_request(r);
      auto res = client_.execute(req);
      if (res.status_code / 100 != 2) {
         throw std::runtime_error("Audio Transcriptions API error: " + res.body);
      }
      return res.body;
   }

   json create_json(const AudioTranscriptionRequest& r) const {
      return json::parse(create(r));
   }

private:
   OpenAIClient& client_;
};

// =====================================================
//  AudioAPI (after SpeechAPI/TranscriptionsAPI)
// =====================================================

/**
 * @brief Aggregates audio-related APIs similar to Python/Java SDKs.
 *
 * Access via:
 *   client.audio().speech().create(...)
 *   client.audio().transcriptions().create_raw(...)
 */
class AudioAPI {
public:
   explicit AudioAPI(OpenAIClient& client)
      : client_(client), speech_api_(client), transcriptions_api_(client)
   {}

   SpeechAPI& speech() { return speech_api_; }

   TranscriptionsAPI& transcriptions() { return transcriptions_api_; }

private:
   OpenAIClient&     client_;
   SpeechAPI         speech_api_;
   TranscriptionsAPI transcriptions_api_;
};

// =====================================================
//  VideosAPI (simplified)
// =====================================================

class VideosAPI {
public:
   explicit VideosAPI(OpenAIClient& client) : client_(client) {}

   HttpRequest create_request(const VideoCreateRequest& r) const {
      json body;
      body["model"]  = r.model;
      body["prompt"] = r.prompt;

      if (r.aspect_ratio) body["aspect_ratio"] = *r.aspect_ratio;
      if (r.format)       body["format"]       = *r.format;
      if (r.duration)     body["duration"]     = *r.duration;
      if (r.seed)         body["seed"]         = *r.seed;
      if (r.user)         body["user"]         = *r.user;
      if (r.metadata)     body["metadata"]     = *r.metadata;

      for (auto it = r.extra.begin(); it != r.extra.end(); ++it) {
         body[it.key()] = it.value();
      }

      HttpRequest req;
      req.method = "POST";
      req.url    = client_.config_.base_url + "/videos";
      req.body   = body.dump();
      client_.add_common_headers(req, "application/json");
      return req;
   }

   json create(const VideoCreateRequest& r) const {
      auto req = create_request(r);
      auto res = client_.execute(req);
      if (res.status_code / 100 != 2) {
         throw std::runtime_error("Videos API error (create): " + res.body);
      }
      return json::parse(res.body);
   }

private:
   OpenAIClient& client_;
};

// =====================================================
//  OpenAIClient API accessors
// =====================================================

inline ResponsesAPI   OpenAIClient::responses()   { return ResponsesAPI(*this); }
inline ImagesAPI      OpenAIClient::images()      { return ImagesAPI(*this); }
inline ModerationsAPI OpenAIClient::moderations() { return ModerationsAPI(*this); }
inline AudioAPI       OpenAIClient::audio()       { return AudioAPI(*this); }
inline VideosAPI      OpenAIClient::videos()      { return VideosAPI(*this); }

} // namespace deitel::openai
