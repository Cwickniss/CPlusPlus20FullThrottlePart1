#pragma once
/**
 * @file requests.hpp
 * @brief Request configuration structs for the OpenAI teaching SDK.
 *
 * Each struct holds configuration for one type of OpenAI API call
 * and uses std::optional members to represent optional parameters.
 *
 * Fields are named and typed to match the official OpenAI API docs.
 * Where the OpenAI documentation specifies a default, we set that
 * default in the struct (e.g., temperature=1.0, n=1, etc.).
 *
 * For API fields not explicitly modeled here, each struct also
 * contains an `extra` json object. You can set arbitrary additional
 * fields via that object and they will be merged into the outgoing
 * JSON request (last-write-wins if keys collide).
 */

#include <string>
#include <optional>
#include <vector>

#include <nlohmann/json.hpp>

namespace deitel::openai {

using json = nlohmann::json;

/**
 * @brief Simple reference to a file resource.
 *
 * This is a placeholder for future extensions where the C++ SDK
 * interacts directly with uploaded files or vector stores.
 */
struct FileRef {
   std::string id;
};

/**
 * @brief Placeholder for error/metadata structures.
 *
 * This can be extended as needed if you want to model more of
 * the OpenAI error responses in C++.
 */
struct ErrorInfo {
   std::string message;
   std::string type;
   std::optional<std::string> param;
   std::optional<std::string> code;
};

/**
 * @brief Request configuration for the Responses API.
 *
 * This corresponds to POST /v1/responses.
 *
 * The fields are modeled after the official Responses API:
 * https://platform.openai.com/docs/api-reference/responses/create
 *
 * Usage example:
 * @code
 * deitel::openai::ResponsesRequest r;
 * r.model = "gpt-4.1-mini";
 * r.input = "Explain Java virtual threads in Java.";
 * r.temperature = 0.7;
 * auto json = client.responses().create(r);
 * @endcode
 */
struct ResponsesRequest {
   /// Required: model name, e.g. "gpt-4.1-mini".
   std::string model;

   /// Optional system / developer instructions.
   std::optional<std::string> instructions;

   /**
    * Required: input for the model.
    *
    * This can be:
    *  - a simple string (for text-only)
    *  - an array of content blocks
    *  - a more complex structured object
    */
   json input;

   /// Arbitrary metadata object.
   std::optional<json> metadata;

   /// Sampling temperature. OpenAI default: 1.0.
   std::optional<double> temperature = 1.0;

   /// Nucleus sampling probability. OpenAI default: 1.0.
   std::optional<double> top_p = 1.0;

   /// Maximum number of output tokens.
   std::optional<int> max_output_tokens;

   /// For multi-turn interactions, link to a previous response.
   std::optional<std::string> previous_response_id;

   /// Reasoning configuration object.
   std::optional<json> reasoning;

   /// Text output configuration (including structured outputs).
   std::optional<json> text;

   /// Tool definitions.
   std::optional<json> tools;

   /// Tool choice configuration.
   std::optional<json> tool_choice;

   /// Truncation configuration.
   std::optional<json> truncation;

   /// Additional fields to include in the response payload.
   std::optional<json> include;

   /// Whether tools can be called in parallel.
   std::optional<bool> parallel_tool_calls;

   /// Enable server-side streaming.
   std::optional<bool> stream;

   /// Audio output configuration.
   std::optional<json> audio;

   /// Whether to store the response server-side.
   std::optional<bool> store;

   /// Optional user identifier for abuse tracking.
   std::optional<std::string> user;

   /// Requested service tier (for models that support it).
   std::optional<std::string> service_tier;

   /// Arbitrary extra fields not modeled explicitly.
   json extra = json::object();
};

// ---------- Images: POST /v1/images (generate) -----------------------------

/**
 * @brief Request configuration for the Images "generate" API.
 *
 * This corresponds to /v1/images with an action that creates images
 * from a text prompt.
 *
 * Usage:
 * @code
 * ImagesGenerateRequest r;
 * r.prompt = "A friendly robot teaching C++ in a classroom";
 * r.response_format = std::string("b64_json");
 * auto result = client.images().generate(r);
 * @endcode
 */
struct ImagesGenerateRequest {
   /// Model name, e.g. "gpt-image-1".
   std::string model;

   /// Text prompt describing the desired image.
   std::string prompt;

   /// Number of images to generate.
   std::optional<int> n;

   /// Image size, e.g. "256x256", "512x512", "1024x1024".
   std::optional<std::string> size;

   /// "standard" or "hd".
   std::optional<std::string> quality;

   /// "vivid" or "natural".
   std::optional<std::string> style;

   /// "url" or "b64_json".
   std::optional<std::string> response_format;

   /// Optional user identifier.
   std::optional<std::string> user;

   /// Arbitrary extra fields not modeled explicitly.
   json extra = json::object();
};

/**
 * @brief Request configuration for the Images "edit" API.
 *
 * This corresponds to POST /v1/images/edits and allows you to
 * restyle or modify an existing image using an optional mask
 * and a text prompt.
 */
struct ImageEditRequest {
   /// Model name, e.g. "gpt-image-1".
   std::string model;

   /// Path to the input image file on disk.
   std::string image_path;

   /// Optional path to a PNG mask image; transparent areas are editable.
   std::optional<std::string> mask_path;

   /// Optional text prompt describing how to edit/restyle the image.
   std::optional<std::string> prompt;

   /// Number of images to generate (default is 1).
   std::optional<int> n;

   /// Image size, e.g. "256x256", "512x512", "1024x1024".
   std::optional<std::string> size;

   /// "standard" or "hd".
   std::optional<std::string> quality;

   /// "vivid" or "natural".
   std::optional<std::string> style;

   /// "url" or "b64_json".
   std::optional<std::string> output_format;

   /// Optional user identifier.
   std::optional<std::string> user;

   /// Arbitrary extra fields not modeled explicitly.
   json extra = json::object();
};

/**
 * @brief Request configuration for the Moderations API.
 *
 * This corresponds to POST /v1/moderations.
 */
struct ModerationRequest {
   /// Model name, e.g. "omni-moderation-latest".
   std::string model;

   /// Input text(s) to classify. Can be a string or an array of strings.
   json input;

   /// Arbitrary extra fields not modeled explicitly.
   json extra = json::object();
};

// ---------- Audio: speech & transcriptions --------------------------------

/**
 * @brief Request configuration for POST /v1/audio/speech.
 *
 * This produces audio (text-to-speech) from text.
 */
struct AudioSpeechRequest {
   std::string model;   ///< e.g. "gpt-4o-mini-tts"
   std::string instructions;   ///< Text to synthesize.
   std::string input;   ///< Text to synthesize.
   std::string voice;   ///< Voice name, e.g. "alloy".

   /// Output format, e.g. "mp3", "wav", "opus", etc.
   std::optional<std::string> format;

   /// Arbitrary extra fields not modeled explicitly.
   json extra = json::object();
};

/**
 * @brief Request configuration for POST /v1/audio/transcriptions.
 *
 * This corresponds to the Whisper-based transcription endpoint.
 *
 * The teaching SDK simplifies file handling—`file_path` is just a
 * string, and the multipart body is represented symbolically.
 */
struct AudioTranscriptionRequest {
   /// Model name, e.g. "gpt-4o-transcribe" or "whisper-1".
   std::string model;

   /// Path to the local audio file to be transcribed.
   std::string file_path;

   /// Optional language hint.
   std::optional<std::string> language;

   /// Optional prompt to influence transcription.
   std::optional<std::string> prompt;

   /// "json", "text", "srt", etc.
   std::optional<std::string> response_format;

   /// Temperature for sampling; see OpenAI docs for defaults.
   std::optional<double> temperature;

   /// Arbitrary extra fields not modeled explicitly.
   json extra = json::object();
};

// ---------- Video: POST /v1/videos (simplified) ---------------------------

/**
 * @brief Request configuration for the Videos API (create).
 *
 * This is a simplified view of the video generation endpoint and
 * focuses on parameters that are easy to demonstrate in teaching
 * examples. You can extend this as OpenAI adds more features.
 */
struct VideoCreateRequest {
   /// Video model name, e.g. "sora-2" or "sora-2-pro".
   std::string model;

   /// Prompt describing the desired video content.
   std::string prompt;

   /// Aspect ratio, e.g. "16:9", "9:16", "1:1".
   std::optional<std::string> aspect_ratio;

   /// Encoded output format, e.g. "mp4".
   std::optional<std::string> format;

   /// Target duration in seconds.
   std::optional<int> duration;

   /// Random seed for reproducibility.
   std::optional<int> seed;

   /// Optional user identifier.
   std::optional<std::string> user;

   /// Arbitrary metadata object.
   std::optional<json> metadata;

   /// Arbitrary extra fields not modeled explicitly.
   json extra = json::object();
};

} // namespace deitel::openai
