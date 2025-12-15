# OpenAI C++20 Teaching SDK (Header‑Only)

This project is a **header‑only, C++20** wrapper around the OpenAI HTTP API,
designed for **teaching** and for side‑by‑side comparison with the official
Python and Java SDKs.

The goals are:

- Keep the **API shape** familiar, e.g.

  ```cpp
  client.responses().create(...);
  client.images().generate(...);
  client.audio().speech().create(...);
  client.audio().transcriptions().create_json(...);
  client.moderations().create(...);
  ```

- Expose the underlying HTTP requests for **introspection**:

  ```cpp
  auto req = client.responses().create_request(r);
  std::cout << req.method << " " << req.url << "\n";
  std::cout << req.body << "\n";
  ```

- Stay **lightweight and explicit** – no magic, just simple wrappers and
  plain‑old‑data request structs.

> **Note:** This SDK is intended for classroom demos, code walk‑throughs,
> and experimentation. For production use, prefer the official OpenAI SDKs.


---

## Features

- **Header‑only** C++20 library
  - Single public header: `#include <openai/openai.hpp>`
- Uses:
  - **libcurl** for HTTP
  - **nlohmann::json** for JSON
  - **cppcodec** for Base64
- Supports core OpenAI APIs (non‑streaming):
  - **Responses API**
  - **Images API** (generate, edit)
  - **Audio API**
    - Text‑to‑speech (Speech)
    - Transcriptions (Speech‑to‑text)
  - **Moderations API**

The library intentionally mirrors the REST docs and the official SDKs, while
keeping everything small enough to show in a lecture or textbook.


---

## Project Layout

- `include/openai/`
  - `openai.hpp` – *single header to include in applications*
  - `core.hpp` – HTTP primitives, configuration, utilities, multipart helpers
  - `requests.hpp` – request POD structs
  - `apis.hpp` – `OpenAIClient` and nested API classes
- `README.md` – this document
- `INSTALL.md` – how to set up dependencies and build


---

## Requirements

- A C++20‑capable compiler (GCC, Clang, MSVC)
- `libcurl` (with HTTPS support)
- Header‑only dependencies:
  - [`nlohmann/json`](https://github.com/nlohmann/json)
  - [`cppcodec`](https://github.com/tplgy/cppcodec)
- An OpenAI API key (`OPENAI_API_KEY` environment variable)

See **INSTALL.md** for platform‑specific hints.


---

## Quickstart

### 1. Add the SDK to your project

Either:

- Copy `include/openai/` into your own project’s `include/` tree, **or**
- Add this repository as a submodule and add its `include/` directory to your
  compiler’s include path.

Compile with something like:

```bash
g++ -std=c++20     -Iexternal/openai-cpp/include     -lcurl     src/main.cpp -o main
```


### 2. Minimal “hello model” example

```cpp
#include <cstdlib>
#include <iostream>
#include <openai/openai.hpp>

int main() {
   using namespace deitel::openai;

   OpenAIConfig cfg;
   cfg.api_key = std::getenv("OPENAI_API_KEY");
   if (cfg.api_key.empty()) {
      std::cerr << "ERROR: OPENAI_API_KEY env var not set\n";
      return 1;
   }

   OpenAIClient client{cfg};

   ResponsesRequest r;
   r.model = "gpt-4.1-mini";
   r.input = "Say hello from C++ and keep it short.";

   auto json = client.responses().create(r);
   std::cout << json.dump(2) << "\n";
}
```

Compile and run (on a Unix‑like system):

```bash
g++ -std=c++20 -Iinclude -lcurl main.cpp -o main
OPENAI_API_KEY=sk-xxx ./main
```


---

## Teaching‑Friendly Design

This SDK is designed to be “open book” for students:

- Every high‑level call has a matching `create_request(...)` that returns
  a **plain `HttpRequest` object** – you can print its URL, headers, and body.
- Request payloads are plain **POD structs** (e.g. `ResponsesRequest`,
  `ImagesGenerateRequest`, `AudioSpeechRequest`) with public fields, so
  students see directly which values map to which JSON keys.
- Utility functions in `core.hpp` (under `deitel::openai::util`) make it
  trivial to:
  - read/write text and binary files
  - convert files to `data:` URLs
  - decode Base64 images
  - extract text from Responses outputs

The emphasis is on **clarity and transparency**, not on hiding details.


---

## Next Steps

- See **INSTALL.md** for detailed installation and build instructions.
- See **EXAMPLES.md** for short but complete examples of:
  - Responses API (text & multimodal)
  - Image generation
  - Audio text‑to‑speech and transcriptions
  - Moderations

Once you can compile and run the examples, you can adapt them to your own
teaching materials and exercises.
