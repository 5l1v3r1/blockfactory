# Contributing to BlockFactory

First off, thanks for your interest in contributing to this project! :tada:

The following is a set of guidelines for contributing to BlockFactory. This project is developed at the Italian Institute of Technology and it is part of our [robotology](https://github.com/robotology) organisation.

Keep in mind that the following are mostly guidelines, not strict rules. Use your best judgement and feel free to propose changes to this document in a pull request. This project is not yet mature as other ones inside our organisation, we'd be glad to hear your voice.

#### Table of contents

- [License](#license)
- [Code style](#code-style)
  - [C++](#c++)
  - [CMake](#cmake)
  - [Git](#git)
- [Documentation](#documentation)
- [Policies](#policies)
  - [Versioning](#versioning)
  - [Deprecations](#deprecations)

## License

All files should contain a license heading:

```
Copyright (C) 2019 Istituto Italiano di Tecnologia (IIT)
All rights reserved.

This software may be modified and distributed under the terms of the
GNU Lesser General Public License v2.1 or any later version.
```

## Code Style

### C++

We enforce a project-wise C++ style based on [clang-format](/.clang-format). For accepting your PR, your code should be formatted accordingly. Most IDEs nowadays support this tool out-of-the-box and configuring it should be straightforward.

Furthermore:

- All files should use UNIX end of lines.
- Include only what you use. If you are unsure, use the [iwyu](https://github.com/include-what-you-use/include-what-you-use) tool.
- The [pimpl pattern](https://cpppatterns.com/patterns/pimpl.html) is recommended.
- Add an empty line between project, system headers, and `std` headers.
- Forward-declare classes into namespaces.
- Do not use `#pragma once`, use instead `#ifndef ... #define ... #endif` macro.
- The defines macros should have the `BLOCKFACTORY_<NS1>_<NS2>_..._<CLASSNAME>_H` format.
- The final `#endif` should have a comment containing the closed `#ifndef`.
- When possible, use explicit declaration and explicit instantiation of templates.
- When not possible, use [three-files `-inl.h`](http://drake.mit.edu/cxx_inl.html) pattern approach.

Example:

```cpp
#ifndef BLOCKFACTORY_CORE_FACTORYSINGLETON_H
#define BLOCKFACTORY_CORE_FACTORYSINGLETON_H

#include "BlockFactory/Core/Log.h"
#include "shlibpp/SharedLibrary.h"
#include "shlibpp/SharedLibraryClass.h"

#include <memory>

namespace blockfactory {
    namespace core {
        class Block;
        class ClassFactorySingleton;
    } // namespace core
} // namespace blockfactory

class blockfactory::core::ClassFactorySingleton
{
private:
    class Impl;
    std::unique_ptr<Impl> pImpl;

public:
[...]

#endif // BLOCKFACTORY_CORE_FACTORYSINGLETON_H
```

### CMake

- Follow as much as possible [Modern CMake Guidelines](https://cliutils.gitlab.io/modern-cmake/).
- 4 spaces indentation (no tabs).
- Lowercase comments, i.e. `if` instead `IF`.
- No spaces between commands and brackets and do not repeat the argument in the closing blocks, i.e. `if(FOO) ... else() ... endif()`.
- No whitespaces at the end of lines.
- One EOL at the end of the file.
- All files should use UNIX end of lines.

### Git

- Try to comply as much as possible to GitFlow branching model (no need to use external tools).
- Do not push merges generated by `git pull`. Always `pull --rebase`.
- Always check what you are pushing using some graphic tool (`qgit`, `gitg`, `gitk`, `sourcetree`, `gitkraken`).
- Keep commits small and do one and only one thing in each commit, which should be documented with optional but appreciated commit description.
- Use `rebase -i` and `amend` to rewrite your local history and keep it clean.
- Never force-push on the `master` and `devel` branches unless there is a very good reason to do it.

## Documentation

- All the new classes and methods of your PR should contain the doxygen documentation in their headers. For checking its render before pushing, a possible way is the following:
   ```bash
   # From the root of the repository
   mkdir build_docs
   cd build_docs
   cmake -DBUILD_DOCS:BOOL=TRUE ..
   make dox
   cd doc/doxygen/html
   python3 -m http.server
   # Browse to http://localhost:8000
   ```
- Your PR should add a changelog in the [doc/release](/doc/release) file of the upcoming version.

## Policies

### Versioning

#### Major releases, e.g. `4`

- We use a fast rollout numbering, publishing a new major release every `devel` freeze. This happens when we decide that the amount of new features are worth a new release.
- Major releases may or may not break APIs. Read [Deprecations](#deprecations) for further information.

#### Minor releases, e.g. `4.1`

- Minor releases may contain hot-fixes applied directly to `master` (and then merged to `devel`).
- APIs stability is preserved.

### Deprecations

The following policy is still a rule-of-thumb. We are not yet enforcing it, but soon we will. Take note of it because this will be valid as soon as we reach an acceptable API stability.

- Functions and classes which are marked as deprecated in one major release will be kept at least for another major release. This means that a deprecated class in version `3.X` will still be present in versions `4.Y`, but it might be deleted in version `6.0`.