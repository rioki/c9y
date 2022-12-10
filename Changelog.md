# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Fixed

- fixed the situation where queue::pop would unlock a thread and it would instantly need to wait on mutex::lock 

### Changed

- changed queue::pop, pop_wait and pop_wait_for return std::optional instead of taking a reference
- changed constructor of thread_pool to accept any callable that a std::thread can take.

### Added

- added queue::emplace to prevent copy of large objects
- added thread_pool::request_stop to allow sopting on a std::stop_token.

### Removed

- removed queue::wake and replaced it with queue::stop

## [0.5.1] - 2022-12-04

### Fixed

- fixed sporadic early return in latch::wait

## [0.5.0] - 2022-12-03

### Added

- add support for coroutines using both sync and async mechanics

### Fixed

- add missing cmake module export
- fixed compile error on latch::max

## [0.4.0] - 2022-07-30

### Added

- parallel_all_of
- parallel_any_of
- parallel_copy_if
- parallel_count
- parallel_count_if
- parallel_for_each
- parallel_none_of
- parallel_reduce

### Fixed

- Made API documentation consisten and added missing bits.

## [0.3.0] - 2022-05-10

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
- POSIX build via CMake

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

[Unreleased]: https://github.com/rioki/c9y/compare/v0.5.1...master

[0.5.1]: https://github.com/rioki/c9y/compare/v0.5.0...v0.5.1
[0.5.0]: https://github.com/rioki/c9y/compare/v0.4.0...v0.5.0
[0.4.0]: https://github.com/rioki/c9y/compare/v0.3.0...v0.4.0
[0.3.0]: https://github.com/rioki/c9y/commits/v0.3.0
[0.2.2]: https://github.com/rioki/c9y/compare/v0.2.1...v0.2.2
[0.2.1]: https://github.com/rioki/c9y/compare/v0.2...v0.2.1
[0.2.0]: https://github.com/rioki/c9y/compare/v0.1...v0.2
[0.1.0]: https://github.com/rioki/c9y/commits/v0.1
