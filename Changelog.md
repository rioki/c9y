# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added

- thread_pool class
- taks_pool class
- thread safe queue
- async function
- sync function
- future suport for sync and async funcitons
- latch class
- parallel algorithms
- more unit tests to ensure qaulity

### Changed

- Upgraded Visual Studio Build to 2022

### Removed

- all basic threading and locking classes that come with C++11

### Note

- this release breaks the API as per [SemVer 4](https://semver.org/spec/v2.0.0.html#spec-item-4)

## [0.2.1] - 2011-05-08

### Added

- Visual Studio build suport.

### Fixed 
- properly implement 'dist' target for POSIX compliant installation

## [0.2.1] - 2011-08-09

### Added

- cleaned up automake build system

## [0.2.0] - 2011-02-09

### Added

- added to POSIX suport

### Removed

- Code::Blocks project files

## [0.1.0] - 2011-02-08

### Added

- thread class
- mutex class
- condition class
- lock class

[Unreleased]: https://github.com/rioki/c9y/commits/master

[0.2.2]: https://github.com/rioki/c9y/compare/v0.2.1...v0.2.2
[0.2.1]: https://github.com/rioki/c9y/compare/v0.2...v0.2.1
[0.2.0]: https://github.com/rioki/c9y/compare/v0.1...v0.2
[0.1.0]: https://github.com/rioki/c9y/commits/v0.1