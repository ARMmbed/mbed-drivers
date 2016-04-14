# Change Log
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [Unreleased]

## [1.3.0]
### Added
- New Pinmap API for generating an unique index for each peripheral

## [1.2.0]
### Added
- Deprecated mbed-drivers/test_env.h in this module.
    - From now on we should use compatible greentea-client/test_env.h
- Add greentea-client added as dependency (to support GCOV prints in retarget.cpp)
- Remove old test cases from test/ directory
- Add new test cases supporting Async model:
    - test 'mbed-drivers-test-c_strings'
    - test 'mbed-drivers-test-dev_null'
    - test 'mbed-drivers-test-echo'
    - test 'mbed-drivers-test-generic_tests'
    - test 'mbed-drivers-test-rtc'
    - test 'mbed-drivers-test-stl_features'
    - test 'mbed-drivers-test-ticker'
    - test 'mbed-drivers-test-ticker_2'
    - test 'mbed-drivers-test-ticker_3'
    - test 'mbed-drivers-test-timeout'
    - test 'mbed-drivers-test-wait_us'
- Tests in test/ support Greentea v0.2.0 and Htrun v0.2.0.

## [1.1.2] - 2016-03-02
### Documentation
- Clarified that I2C.md covers the version 2, experimental I2C API

## [1.1.0]
### Added
- Prototype of V2 API for I2C. Full documentation at docs/I2C.md

## [1.0.0]
### Release
mbed-drivers has reached the 1.0.0 milestone.

## [0.12.2] - 2016-02-25
### Fixed
- gcov output generation that was broken in 0.12.0.

[Unreleased]: https://github.com/ARMmbed/mbed-drivers/compare/v1.3.0...HEAD
[1.3.0]: https://github.com/ARMmbed/mbed-drivers/compare/v1.2.0...v1.3.0
[1.2.0]: https://github.com/ARMmbed/mbed-drivers/compare/v1.1.2...v1.2.0
[1.1.2]: https://github.com/ARMmbed/mbed-drivers/compare/v1.1.0...v1.1.2
[1.1.0]: https://github.com/ARMmbed/mbed-drivers/compare/v1.0.0...v1.1.0
[1.0.0]: https://github.com/ARMmbed/mbed-drivers/compare/v1.0.0...v0.12.2
[0.12.2]: https://github.com/ARMmbed/mbed-drivers/compare/v0.12.1...v0.12.2
