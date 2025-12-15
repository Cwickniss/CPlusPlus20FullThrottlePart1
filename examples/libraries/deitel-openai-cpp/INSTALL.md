# Installation & Build Instructions

This SDK is **header‑only**, but it depends on a few external libraries and a
C++20 compiler.

---

## 1. Dependencies

You will need:

- A **C++20** compiler (GCC, Clang, MSVC)
- **libcurl** (with HTTPS support)
- **nlohmann/json** (header‑only JSON library)
- **cppcodec** (header‑only Base64 library)

The SDK itself lives under `include/openai/` and is included via:

```cpp
#include <openai/openai.hpp>
```


### 1.1 Ubuntu / Debian

Install system packages:

```bash
sudo apt-get update
sudo apt-get install -y    build-essential    libcurl4-openssl-dev
```

For `nlohmann/json` and `cppcodec`, you can either:

- Install them via your package manager (if available), or
- Vendor the headers directly into your project (`external/` directory).

Example layout:

```text
your-project/
  external/
    nlohmann/
      json.hpp
    cppcodec/
      base64_rfc4648.hpp
  include/
    openai/...
  src/
    main.cpp
```

Compile with:

```bash
g++ -std=c++20     -Iinclude     -Iexternal     -lcurl     src/main.cpp -o main
```


### 1.2 macOS (Homebrew)

Install curl:

```bash
brew update
brew install curl
```

Again, bring in `nlohmann/json` and `cppcodec` either via a package manager
(vcpkg, Conan, etc.) or by vendoring the headers into your project and adding
the directory to your include path.


### 1.3 Windows

Typical options:

- **MSVC + CMake** with vcpkg or Conan
- **MinGW‑w64** plus manual installation of dependencies

Example with vcpkg:

```powershell
vcpkg install curl[ssl] nlohmann-json cppcodec
```

Configure your project to:

- add the SDK’s `include/` folder to the include paths
- link against `libcurl`


---

## 2. Adding the SDK to Your Project

Place the SDK folder somewhere in your project. For example:

```text
your-project/
  external/
    openai-cpp/
      include/openai/...
  src/
    main.cpp
```

In your build system (Makefile, CMake, etc.), add:

- `-Iexternal/openai-cpp/include` to the include paths
- `-lcurl` (or the platform’s equivalent curl library) to the linker flags

Example (simple g++ invocation):

```bash
g++ -std=c++20     -Iexternal/openai-cpp/include     -lcurl     src/main.cpp -o main
```


---

## 3. Configuration in Code

The key configuration type is `deitel::openai::OpenAIConfig`:

```cpp
#include <cstdlib>
#include <iostream>
#include <openai/openai.hpp>

int main() {
   using namespace deitel::openai;

   OpenAIConfig cfg;
   cfg.api_key = std::getenv("OPENAI_API_KEY");
   if (cfg.api_key.empty()) {
      std::cerr << "OPENAI_API_KEY not set\n";
      return 1;
   }

   // Optional: override default values
   // cfg.base_url        = "https://api.openai.com/v1";
   // cfg.timeout_seconds = 60;

   OpenAIClient client{cfg};

   // use client.responses(), client.images(), client.audio(), client.moderations(), ...
}
```

If you do not set `cfg.base_url`, it defaults to the standard OpenAI URL.
You should **always** set `cfg.api_key`, typically from the environment.


---

## 4. Linking

The SDK has **no compiled library**; it is header‑only. You only need to link
against `libcurl` (and whatever that depends on).

For example (Linux/macOS):

```bash
g++ -std=c++20     -Iinclude     -Iexternal     -lcurl     src/main.cpp -o main
```

On Windows/MSVC with vcpkg, make sure your CMake or project configuration:

- adds the vcpkg include paths (for curl, json, cppcodec)
- links against `libcurl`


---

## 5. Testing Your Setup

Create `src/main.cpp`:

```cpp
#include <cstdlib>
#include <iostream>
#include <openai/openai.hpp>

int main() {
   using namespace deitel::openai;

   OpenAIConfig cfg;
   cfg.api_key = std::getenv("OPENAI_API_KEY");
   if (cfg.api_key.empty()) {
      std::cerr << "OPENAI_API_KEY not set\n";
      return 1;
   }

   OpenAIClient client{cfg};

   ResponsesRequest r;
   r.model = "gpt-4.1-mini";
   r.input = "Say hello from C++.";

   auto result = client.responses().create(r);
   std::cout << result.dump(2) << "\n";
}
```

Compile and run. If you see a valid JSON response from the model, your
installation is working.


---

## 6. Where to Go Next

- Use **EXAMPLES.md** for more complete examples (text, images, audio,
  moderations).
- Look at the code in the `examples/` directory for larger, self‑contained
  programs that you can adapt to your own work.
