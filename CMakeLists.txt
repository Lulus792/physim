cmake_minimum_required(VERSION 3.24)
project(Physim VERSION 0.1.0 LANGUAGES C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
foreach(config Debug Release RelWithDebInfo MinSizeRel)
  string(TOUPPER ${config} CONFIG)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_BINARY_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG} "${CMAKE_BINARY_DIR}/bin")
endforeach()
option(PHYSIM_BUILD_APP "Build the SDL3 desktop application" ON)
option(PHYSIM_GRAPHICS_TESTS "Run window and GPU tests (requires a display and OpenGL 3.3)" OFF)
option(PHYSIM_SANITIZERS "Enable address and undefined behavior sanitizers" OFF)
function(ps_warnings target)
  if(MSVC)
    target_compile_options(${target} PRIVATE /W4 /utf-8)
    target_compile_definitions(${target} PRIVATE _CRT_SECURE_NO_WARNINGS)
    get_target_property(target_type ${target} TYPE)
    if(target_type STREQUAL "EXECUTABLE")
      target_sources(${target} PRIVATE app/utf8.manifest)
    endif()
  else()
    target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wshadow)
    if(PHYSIM_SANITIZERS)
      target_compile_options(${target} PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
      target_link_options(${target} PRIVATE -fsanitize=address,undefined)
    endif()
  endif()
endfunction()
add_library(physim_zlib STATIC
  third_party/zlib-1.3.2/adler32.c third_party/zlib-1.3.2/crc32.c
  third_party/zlib-1.3.2/deflate.c third_party/zlib-1.3.2/trees.c
  third_party/zlib-1.3.2/zutil.c)
target_include_directories(physim_zlib SYSTEM PUBLIC third_party/zlib-1.3.2)
target_compile_definitions(physim_zlib PUBLIC Z_PREFIX)
if(PHYSIM_SANITIZERS AND NOT MSVC)
  target_compile_options(physim_zlib PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
endif()
add_library(physim_core STATIC src/core.c src/memory.c src/array.c src/string_view.c src/hashmap.c src/math.c src/data.c src/analysis.c src/scene.c src/numerics.c src/units.c src/series.c src/report.c src/report_export.c src/mechanics.c src/box_contacts.c src/collision.c src/measurement.c)
set_target_properties(physim_core PROPERTIES POSITION_INDEPENDENT_CODE ON OUTPUT_NAME physim-core)
target_include_directories(physim_core PUBLIC include)
if(NOT WIN32)
  target_link_libraries(physim_core PUBLIC m)
endif()
ps_warnings(physim_core)
add_library(physim_platform STATIC src/platform.c src/protocol.c)
target_include_directories(physim_platform PUBLIC include src)
target_link_libraries(physim_platform PUBLIC physim_core)
if(NOT WIN32)
  target_compile_definitions(physim_core PRIVATE _POSIX_C_SOURCE=200809L)
  target_compile_definitions(physim_platform PRIVATE _POSIX_C_SOURCE=200809L)
  target_link_libraries(physim_platform PUBLIC ${CMAKE_DL_LIBS})
endif()
ps_warnings(physim_platform)
add_executable(physim-runner runners/experiment.c)
target_link_libraries(physim-runner PRIVATE physim_core physim_platform)
ps_warnings(physim-runner)
add_executable(physim-analysis-runner runners/analysis.c)
target_link_libraries(physim-analysis-runner PRIVATE physim_core physim_platform)
ps_warnings(physim-analysis-runner)
add_library(physim_batch STATIC src/batch.c)
target_link_libraries(physim_batch PUBLIC physim_platform)
ps_warnings(physim_batch)
add_executable(physim-batch runners/batch.c)
target_link_libraries(physim-batch PRIVATE physim_batch)
ps_warnings(physim-batch)
foreach(example pendulum projectile collision box_floor spring uncertain_projectile box_collision buoyancy)
  add_library(${example} MODULE examples/${example}/main.c)
  target_link_libraries(${example} PRIVATE physim_core)
  set_target_properties(${example} PROPERTIES PREFIX "")
  ps_warnings(${example})
endforeach()
add_library(pendulum_analysis MODULE examples/pendulum/analysis.c)
target_link_libraries(pendulum_analysis PRIVATE physim_core)
set_target_properties(pendulum_analysis PROPERTIES PREFIX "")
ps_warnings(pendulum_analysis)
if(PHYSIM_BUILD_APP)
  if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/third_party/SDL3-3.2.30/cmake")
    list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/third_party/SDL3-3.2.30")
  endif()
  find_package(SDL3 3.2 CONFIG REQUIRED)
  add_executable(physim app/main.c app/parameter_catalog.c app/ui_backend.c app/ui_sdl.c app/ui_geometry.c app/graphics.c app/documentation.c app/library.c app/autosave.c app/preferences.c app/plot_view.c app/report_image.c app/png.c)
  target_include_directories(physim PRIVATE third_party)
  target_link_libraries(physim PRIVATE physim_core physim_platform physim_batch physim_zlib SDL3::SDL3)
  target_compile_definitions(physim PRIVATE PS_SOURCE_DIR="${CMAKE_SOURCE_DIR}" PS_CMAKE="${CMAKE_COMMAND}")
  if(PHYSIM_SANITIZERS AND NOT MSVC)
    target_compile_options(physim PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(physim PRIVATE -fsanitize=address,undefined)
  endif()
  if(MSVC)
    target_compile_options(physim PRIVATE /W4 /utf-8)
    target_compile_definitions(physim PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_sources(physim PRIVATE app/utf8.manifest)
  endif()
  if(WIN32)
    target_link_libraries(physim PRIVATE dwmapi)
    add_custom_command(TARGET physim POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:physim>)
  endif()
  install(TARGETS physim RUNTIME DESTINATION bin)
  if(WIN32)
    install(FILES $<TARGET_FILE:SDL3::SDL3> DESTINATION bin)
  endif()
endif()
enable_testing()
add_executable(physim-parameter-catalog-tests tests/test_parameter_catalog.c app/parameter_catalog.c)
target_include_directories(physim-parameter-catalog-tests PRIVATE app)
target_link_libraries(physim-parameter-catalog-tests PRIVATE physim_core)
ps_warnings(physim-parameter-catalog-tests)
add_test(NAME parameter_catalog COMMAND physim-parameter-catalog-tests)
add_library(physim_language STATIC src/language/lexer.c src/language/parser.c src/language/checker.c src/language/emitter.c src/language/builtins.c)
target_include_directories(physim_language PUBLIC src)
ps_warnings(physim_language)
if(TARGET physim)
  target_link_libraries(physim PRIVATE physim_language)
endif()
add_executable(physimc src/language/main.c src/language/loader.c)
target_link_libraries(physimc PRIVATE physim_language)
if(NOT WIN32)
  target_compile_definitions(physimc PRIVATE _POSIX_C_SOURCE=200809L)
endif()
ps_warnings(physimc)
install(TARGETS physimc RUNTIME DESTINATION bin)
if(TARGET physim)
  add_dependencies(physim physimc)
endif()
include(cmake/PhysimLanguage.cmake)
physim_add_experiment(physim-language-pendulum examples/language/pendulum.phys)
physim_add_experiment(physim-language-pendulum-rk4 examples/language/pendulum_rk4.phys)
physim_add_experiment(physim-language-pendulum-integrator examples/language/pendulum_integrator.phys)
physim_add_experiment(physim-language-pendulum-rk45 examples/language/pendulum_rk45.phys)
physim_add_experiment(physim-language-pendulum-verlet examples/language/pendulum_verlet.phys)
physim_add_experiment(physim-language-documentation-main examples/documentation/language_main.phys)
physim_add_analysis(physim-language-documentation-analysis examples/documentation/language_analysis.phys)
physim_add_analysis(physim-language-vacuum-analysis examples/documentation/vacuum_analysis.phys)
physim_add_experiment(physim-language-projectile examples/language/projectile.phys)
physim_add_experiment(physim-language-projectile-drag examples/language/projectile_drag.phys)
physim_add_experiment(physim-language-collision examples/language/collision.phys)
physim_add_analysis(physim-language-collision-analysis examples/language/analysis_collision.phys)
physim_add_experiment(physim-language-box-collision examples/language/box_collision.phys)
physim_add_analysis(physim-language-box-collision-analysis examples/language/analysis_box_collision.phys)
add_executable(physim-collision-language-parity-tests tests/test_collision_language_parity.c)
target_link_libraries(physim-collision-language-parity-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-collision-language-parity-tests)
add_dependencies(physim-collision-language-parity-tests collision physim-language-collision
  pendulum_analysis physim-language-collision-analysis physim-runner physim-analysis-runner)
add_test(NAME language_collision_parity COMMAND physim-collision-language-parity-tests
  $<TARGET_FILE:physim-runner> $<TARGET_FILE:collision>
  $<TARGET_FILE:physim-language-collision> $<TARGET_FILE:physim-analysis-runner>
  $<TARGET_FILE:pendulum_analysis> $<TARGET_FILE:physim-language-collision-analysis>
  $<TARGET_FILE:collision_air> $<TARGET_FILE:collision_custom> $<TARGET_FILE:collision_friction>
  ${CMAKE_BINARY_DIR})
set_tests_properties(language_collision_parity PROPERTIES TIMEOUT 60)
add_executable(physim-box-collision-language-parity-tests tests/test_box_collision_language_parity.c)
target_link_libraries(physim-box-collision-language-parity-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-box-collision-language-parity-tests)
add_dependencies(physim-box-collision-language-parity-tests box_collision box_collision_inelastic
  box_collision_oblique physim-language-box-collision physim-language-box-collision-analysis
  physim-runner physim-analysis-runner)
add_test(NAME language_box_collision_parity COMMAND physim-box-collision-language-parity-tests
  $<TARGET_FILE:physim-runner> $<TARGET_FILE:box_collision>
  $<TARGET_FILE:physim-language-box-collision> $<TARGET_FILE:physim-analysis-runner>
  $<TARGET_FILE:physim-language-box-collision-analysis>
  $<TARGET_FILE:box_collision_inelastic> $<TARGET_FILE:box_collision_oblique>
  ${CMAKE_BINARY_DIR})
set_tests_properties(language_box_collision_parity PROPERTIES TIMEOUT 60)
physim_add_experiment(physim-language-buoyancy examples/language/buoyancy.phys)
physim_add_analysis(physim-language-buoyancy-analysis examples/language/analysis_buoyancy.phys)
add_library(buoyancy_analysis MODULE examples/buoyancy/analysis.c)
target_link_libraries(buoyancy_analysis PRIVATE physim_core)
set_target_properties(buoyancy_analysis PROPERTIES PREFIX "")
ps_warnings(buoyancy_analysis)
add_executable(physim-buoyancy-language-parity-tests tests/test_buoyancy_language_parity.c)
target_link_libraries(physim-buoyancy-language-parity-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-buoyancy-language-parity-tests)
add_dependencies(physim-buoyancy-language-parity-tests buoyancy physim-language-buoyancy
  buoyancy_analysis physim-language-buoyancy-analysis physim-runner physim-analysis-runner)
add_test(NAME language_buoyancy_parity COMMAND physim-buoyancy-language-parity-tests
  $<TARGET_FILE:physim-runner> $<TARGET_FILE:buoyancy>
  $<TARGET_FILE:physim-language-buoyancy> $<TARGET_FILE:physim-analysis-runner>
  $<TARGET_FILE:buoyancy_analysis>
  $<TARGET_FILE:physim-language-buoyancy-analysis> ${CMAKE_BINARY_DIR})
set_tests_properties(language_buoyancy_parity PROPERTIES TIMEOUT 60)
physim_add_analysis(physim-language-drag-analysis examples/documentation/drag_analysis.phys)
add_library(documentation_drag_analysis MODULE examples/documentation/drag_analysis.c)
target_link_libraries(documentation_drag_analysis PRIVATE physim_core)
set_target_properties(documentation_drag_analysis PROPERTIES PREFIX "")
ps_warnings(documentation_drag_analysis)
add_executable(physim-projectile-drag-parity-tests tests/test_projectile_drag_parity.c)
target_link_libraries(physim-projectile-drag-parity-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-projectile-drag-parity-tests)
add_dependencies(physim-projectile-drag-parity-tests projectile physim-language-projectile-drag
  documentation_drag_analysis physim-language-drag-analysis physim-runner)
add_test(NAME language_projectile_drag_parity COMMAND physim-projectile-drag-parity-tests
  $<TARGET_FILE:physim-runner> $<TARGET_FILE:projectile>
  $<TARGET_FILE:physim-language-projectile-drag> $<TARGET_FILE:documentation_drag_analysis>
  $<TARGET_FILE:physim-language-drag-analysis> ${CMAKE_BINARY_DIR})
physim_add_analysis(physim-language-analysis examples/language/analysis.phys)
physim_add_analysis(physim-language-analysis-monte-carlo examples/language/analysis_monte_carlo.phys)
physim_add_analysis(physim-language-analysis-batch-endpoints examples/language/analysis_batch_endpoints.phys)
physim_add_analysis(physim-language-analysis-stale tests/fixtures/language/analysis_stale.phys)
physim_add_analysis(physim-language-analysis-units tests/fixtures/language/analysis_units.phys)
physim_add_analysis(physim-language-analysis-reference tests/fixtures/language/analysis_reference.phys)
physim_add_analysis(physim-language-analysis-operations-error tests/fixtures/language/analysis_operations_error.phys)
physim_add_analysis(physim-language-analysis-selection tests/fixtures/language/analysis_selection.phys)
physim_add_analysis(physim-language-analysis-selection-error tests/fixtures/language/analysis_selection_error.phys)
physim_add_analysis(physim-language-analysis-export-columns
  tests/fixtures/language/analysis_export_columns.phys)
physim_add_analysis(physim-language-analysis-export-columns-error
  tests/fixtures/language/analysis_export_columns_error.phys)
physim_add_analysis(physim-language-analysis-export-columns-unaligned
  tests/fixtures/language/analysis_export_columns_unaligned.phys)
physim_add_analysis(physim-language-analysis-series-values
  tests/fixtures/language/analysis_series_values.phys)
physim_add_analysis(physim-language-analysis-series-values-error
  tests/fixtures/language/analysis_series_values_error.phys)
physim_add_analysis(physim-language-analysis-series-quantile
  tests/fixtures/language/analysis_series_quantile.phys)
physim_add_analysis(physim-language-analysis-series-quantile-error
  tests/fixtures/language/analysis_series_quantile_error.phys)
add_test(NAME language_series_quantile COMMAND ${CMAKE_COMMAND}
  -DRUNNER=$<TARGET_FILE:physim-analysis-runner>
  -DMODULE=$<TARGET_FILE:physim-language-analysis-series-quantile>
  -DERROR_MODULE=$<TARGET_FILE:physim-language-analysis-series-quantile-error>
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-series-quantile-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_series_quantile.cmake)
add_test(NAME language_series_values COMMAND ${CMAKE_COMMAND}
  -DRUNNER=$<TARGET_FILE:physim-analysis-runner>
  -DMODULE=$<TARGET_FILE:physim-language-analysis-series-values>
  -DERROR_MODULE=$<TARGET_FILE:physim-language-analysis-series-values-error>
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-series-values-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_series_values.cmake)
add_test(NAME language_export_columns COMMAND ${CMAKE_COMMAND}
  -DRUNNER=$<TARGET_FILE:physim-analysis-runner>
  -DMODULE=$<TARGET_FILE:physim-language-analysis-export-columns>
  -DERROR_MODULE=$<TARGET_FILE:physim-language-analysis-export-columns-error>
  -DUNALIGNED_MODULE=$<TARGET_FILE:physim-language-analysis-export-columns-unaligned>
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-export-columns-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_export_columns.cmake)
physim_add_analysis(physim-language-analysis-sensors examples/language/analysis_sensors.phys)
physim_add_analysis(physim-language-scalar-search-analysis tests/fixtures/language/scalar_search_analysis.phys)
add_executable(physim-language-report-tests tests/test_language_report.c)
target_link_libraries(physim-language-report-tests PRIVATE physim_platform)
ps_warnings(physim-language-report-tests)
add_dependencies(physim-language-report-tests physim-language-analysis-selection
  physim-language-analysis-selection-error physim-language-analysis-sensors)
add_test(NAME language_report COMMAND physim-language-report-tests
  $<TARGET_FILE:physim-language-analysis-selection> $<TARGET_FILE:physim-language-analysis-selection-error>
  $<TARGET_FILE:physim-language-analysis-sensors> "${CMAKE_BINARY_DIR}")
set_tests_properties(language_report PROPERTIES TIMEOUT 90)
add_executable(physim-language-analysis-tests tests/test_language_analysis.c)
target_link_libraries(physim-language-analysis-tests PRIVATE physim_platform)
ps_warnings(physim-language-analysis-tests)
add_dependencies(physim-language-analysis-tests physim-language-analysis
  physim-language-analysis-monte-carlo
  physim-language-analysis-batch-endpoints
  physim-language-analysis-stale physim-language-analysis-units physim-language-projectile
  physim-runner physim-analysis-runner physim-language-analysis-reference physim-language-analysis-operations-error)
add_test(NAME language_analysis COMMAND physim-language-analysis-tests
  $<TARGET_FILE:physim-language-analysis> $<TARGET_FILE:physim-language-analysis-stale>
  $<TARGET_FILE:physim-language-analysis-units> $<TARGET_FILE:physim-analysis-runner>
  $<TARGET_FILE:physim-runner> $<TARGET_FILE:physim-language-projectile> "${CMAKE_BINARY_DIR}"
  $<TARGET_FILE:physim-language-analysis-reference> $<TARGET_FILE:physim-language-analysis-operations-error>
  $<TARGET_FILE:physim-language-analysis-monte-carlo>
  $<TARGET_FILE:physim-language-analysis-batch-endpoints>)
set_tests_properties(language_analysis PROPERTIES TIMEOUT 90)
foreach(fault error scene_error create_error)
  physim_add_experiment(physim-language-${fault} tests/fixtures/language/experiment_${fault}.phys)
endforeach()
physim_add_experiment(physim-language-random examples/language/random_samples.phys)
physim_add_experiment(physim-language-parameters tests/fixtures/language/parameter_experiment.phys)
physim_add_experiment(physim-language-parameter-pair tests/fixtures/language/parameter_pair.phys)
add_executable(physim-language-parameter-tests tests/test_language_parameters.c)
target_link_libraries(physim-language-parameter-tests PRIVATE physim_core)
ps_warnings(physim-language-parameter-tests)
add_executable(physim-parameter-tests tests/test_parameters.c)
target_link_libraries(physim-parameter-tests PRIVATE physim_core)
ps_warnings(physim-parameter-tests)
add_test(NAME parameters_api COMMAND physim-parameter-tests)
add_dependencies(physim-language-parameter-tests physim-language-parameters physim-runner)
add_test(NAME language_parameters COMMAND ${CMAKE_COMMAND}
  -DEXPERIMENT=$<TARGET_FILE:physim-language-parameters>
  -DRUNNER=$<TARGET_FILE:physim-runner>
  -DVERIFY=$<TARGET_FILE:physim-language-parameter-tests>
  -DWORK=${CMAKE_BINARY_DIR}/language-parameters
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_parameters.cmake)
add_executable(physim-parameter-sweep-tests tests/test_parameter_sweep.c)
target_link_libraries(physim-parameter-sweep-tests PRIVATE physim_core)
ps_warnings(physim-parameter-sweep-tests)
add_dependencies(physim-parameter-sweep-tests physim-language-parameters
  physim-language-parameter-pair physim-batch)
add_test(NAME language_parameter_sweep COMMAND ${CMAKE_COMMAND}
  -DBATCH=$<TARGET_FILE:physim-batch>
  -DRUNNER=$<TARGET_FILE:physim-runner>
  -DEXPERIMENT=$<TARGET_FILE:physim-language-parameters>
  -DVERIFY=$<TARGET_FILE:physim-parameter-sweep-tests>
  -DWORK=${CMAKE_BINARY_DIR}/language-parameter-sweep
  -P ${CMAKE_SOURCE_DIR}/tests/check_parameter_sweep.cmake)
add_test(NAME language_parameter_sweep_fixed COMMAND ${CMAKE_COMMAND}
  -DBATCH=$<TARGET_FILE:physim-batch>
  -DRUNNER=$<TARGET_FILE:physim-runner>
  -DEXPERIMENT=$<TARGET_FILE:physim-language-parameter-pair>
  -DVERIFY=$<TARGET_FILE:physim-parameter-sweep-tests>
  -DWORK=${CMAKE_BINARY_DIR}/language-parameter-sweep-fixed
  -DFIXED=2
  -P ${CMAKE_SOURCE_DIR}/tests/check_parameter_sweep.cmake)
physim_add_experiment(physim-language-rng-run-streams examples/language/rng_run_streams.phys)
physim_add_experiment(physim-language-random-error tests/fixtures/language/random_error.phys)
physim_add_experiment(physim-language-random-create-error tests/fixtures/language/random_create_error.phys)
physim_add_experiment(physim-language-scene-shapes examples/language/scene_shapes.phys)
physim_add_experiment(physim-language-spring examples/language/spring.phys)
physim_add_experiment(physim-language-scene-shapes-error tests/fixtures/language/scene_shapes_error.phys)
physim_add_experiment(physim-language-polyline-empty tests/fixtures/language/polyline_empty.phys)
physim_add_experiment(physim-language-sensors examples/language/sensors.phys)
physim_add_experiment(physim-language-uncertain-projectile examples/language/uncertain_projectile.phys)
physim_add_experiment(physim-language-sensor-error tests/fixtures/language/sensor_error.phys)
physim_add_program(physim-language-body-values examples/language/rigid_body.phys)
physim_add_program(physim-language-modules examples/language/modules/main.phys)
physim_add_program(physim-language-module-paths tests/fixtures/language/module_paths/main.phys
  MODULE_PATHS tests/fixtures/language/shared_modules)
add_test(NAME language_module_paths_runtime COMMAND physim-language-module-paths)
add_test(NAME language_module_paths COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-module-paths-check
  -DSOURCE=${CMAKE_SOURCE_DIR}/tests/fixtures/language/module_paths/main.phys
  -DMODULE_ROOT=${CMAKE_SOURCE_DIR}/tests/fixtures/language/shared_modules
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_module_paths.cmake)
physim_add_program(physim-language-delayed-initialization
  tests/fixtures/language/delayed_initialization.phys)
physim_add_program(physim-language-delayed-initialization-error
  tests/fixtures/language/delayed_initialization_error.phys)
physim_add_program(physim-language-delayed-global-error
  tests/fixtures/language/delayed_global_error.phys)
physim_add_program(physim-language-delayed-compound-error
  tests/fixtures/language/delayed_compound_error.phys)
physim_add_program(physim-language-delayed-field-error
  tests/fixtures/language/delayed_field_error.phys)
physim_add_program(physim-language-delayed-capture-error
  tests/fixtures/language/delayed_capture_error.phys)
add_test(NAME language_delayed_initialization_runtime
  COMMAND physim-language-delayed-initialization)
add_test(NAME language_delayed_initialization COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DPROGRAM=$<TARGET_FILE:physim-language-delayed-initialization-error>
  -DPROGRAM_GLOBAL=$<TARGET_FILE:physim-language-delayed-global-error>
  -DPROGRAM_COMPOUND=$<TARGET_FILE:physim-language-delayed-compound-error>
  -DPROGRAM_FIELD=$<TARGET_FILE:physim-language-delayed-field-error>
  -DPROGRAM_CAPTURE=$<TARGET_FILE:physim-language-delayed-capture-error>
  -DWORK=${CMAKE_BINARY_DIR}/language-delayed-initialization-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_delayed_initialization.cmake)
physim_add_program(physim-language-generics examples/language/generics/main.phys)
physim_add_experiment(physim-language-generics-experiment tests/fixtures/language/generic_modes/experiment.phys)
physim_add_analysis(physim-language-generics-analysis tests/fixtures/language/generic_modes/analysis.phys)
add_test(NAME language_generic_modes COMMAND ${CMAKE_COMMAND}
  -DEXPERIMENT=$<TARGET_FILE:physim-language-generics-experiment>
  -DANALYSIS=$<TARGET_FILE:physim-language-generics-analysis>
  -DEXPERIMENT_RUNNER=$<TARGET_FILE:physim-runner>
  -DANALYSIS_RUNNER=$<TARGET_FILE:physim-analysis-runner>
  -DWORK=${CMAKE_BINARY_DIR}/language-generic-modes
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_generic_modes.cmake)
add_test(NAME language_generics_runtime COMMAND physim-language-generics)
add_test(NAME language_generics COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-generics-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_generics.cmake)
add_test(NAME language_generic_structs COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-generic-structs-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_generic_structs.cmake)
physim_add_program(physim-language-comparable tests/fixtures/language/comparable.phys)
add_test(NAME language_comparable_runtime COMMAND physim-language-comparable)
add_test(NAME language_comparable COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-comparable-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_comparable.cmake)
physim_add_program(physim-language-switch-patterns tests/fixtures/language/switch_patterns.phys)
add_test(NAME language_switch_patterns_runtime COMMAND physim-language-switch-patterns)
physim_add_program(physim-language-switch-guards tests/fixtures/language/switch_guards.phys)
add_test(NAME language_switch_guards_runtime COMMAND physim-language-switch-guards)
physim_add_program(physim-language-switch-scalars tests/fixtures/language/switch_scalars.phys)
add_test(NAME language_switch_scalars_runtime COMMAND physim-language-switch-scalars)
add_test(NAME language_switch_scalars COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-switch-scalars-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_switch_scalars.cmake)
physim_add_program(physim-language-switch-string-ranges
  tests/fixtures/language/switch_string_ranges.phys)
add_test(NAME language_switch_string_ranges_runtime
  COMMAND physim-language-switch-string-ranges)
add_test(NAME language_switch_string_ranges COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-switch-string-ranges-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_switch_string_ranges.cmake)
physim_add_program(physim-language-switch-string-payload-ranges
  tests/fixtures/language/switch_string_payload_ranges.phys)
add_test(NAME language_switch_string_payload_ranges_runtime
  COMMAND physim-language-switch-string-payload-ranges)
add_test(NAME language_switch_string_payload_ranges COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-switch-string-payload-ranges-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_switch_string_payload_ranges.cmake)
physim_add_program(physim-language-switch-optionals tests/fixtures/language/switch_optionals.phys)
add_test(NAME language_switch_optionals_runtime COMMAND physim-language-switch-optionals)
physim_add_program(physim-language-range-assignment tests/fixtures/language/range_assignment.phys)
add_test(NAME language_range_assignment_runtime COMMAND physim-language-range-assignment)
physim_add_program(physim-language-strided-ranges tests/fixtures/language/strided_ranges.phys)
add_test(NAME language_strided_ranges_runtime COMMAND physim-language-strided-ranges)
physim_add_program(physim-language-function-values tests/fixtures/language/function_values.phys)
add_test(NAME language_function_values_runtime COMMAND physim-language-function-values)
physim_add_program(physim-language-generic-enums tests/fixtures/language/generic_enums.phys)
add_test(NAME language_generic_enums_runtime COMMAND physim-language-generic-enums)
add_test(NAME language_generic_enums COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-generic-enums-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_generic_enums.cmake)
physim_add_program(physim-language-closures tests/fixtures/language/closures.phys)
add_test(NAME language_closures_runtime COMMAND physim-language-closures)
physim_add_program(physim-language-array-concat tests/fixtures/language/array_concat.phys)
add_test(NAME language_array_concat_runtime COMMAND physim-language-array-concat)
physim_add_program(physim-language-array-endpoints tests/fixtures/language/array_endpoints.phys)
add_test(NAME language_array_endpoints_runtime COMMAND physim-language-array-endpoints)
add_test(NAME language_array_endpoints COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-endpoints-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_endpoints.cmake)
physim_add_program(physim-language-string-endpoints
  tests/fixtures/language/string_endpoints.phys)
add_test(NAME language_string_endpoints_runtime COMMAND physim-language-string-endpoints)
add_test(NAME language_string_endpoints COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-endpoints-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_endpoints.cmake)
physim_add_program(physim-language-string-ordering
  tests/fixtures/language/string_ordering.phys)
add_test(NAME language_string_ordering_runtime COMMAND physim-language-string-ordering)
add_test(NAME language_string_ordering COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-ordering-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_ordering.cmake)
physim_add_program(physim-language-array-insert tests/fixtures/language/array_insert.phys)
add_test(NAME language_array_insert_runtime COMMAND physim-language-array-insert)
physim_add_program(physim-language-array-contains tests/fixtures/language/array_contains.phys)
add_test(NAME language_array_contains_runtime COMMAND physim-language-array-contains)
physim_add_program(physim-language-array-split tests/fixtures/language/array_split.phys)
add_test(NAME language_array_split_runtime COMMAND physim-language-array-split)
  add_test(NAME language_array_split COMMAND ${CMAKE_COMMAND}
    -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-split-check
    -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_split.cmake)
  physim_add_program(physim-language-array-sequence-compare tests/fixtures/language/array_sequence_compare.phys)
  add_test(NAME language_array_sequence_compare_runtime COMMAND physim-language-array-sequence-compare)
  add_test(NAME language_array_sequence_compare COMMAND ${CMAKE_COMMAND}
    -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-sequence-compare-check
    -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_sequence_compare.cmake)
physim_add_program(physim-language-array-filter tests/fixtures/language/array_filter.phys)
add_test(NAME language_array_filter_runtime COMMAND physim-language-array-filter)
add_test(NAME language_array_filter COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-filter-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_filter.cmake)
physim_add_program(physim-language-array-predicate-queries tests/fixtures/language/array_predicate_queries.phys)
add_test(NAME language_array_predicate_queries_runtime COMMAND physim-language-array-predicate-queries)
add_test(NAME language_array_predicate_queries COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-predicate-queries-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_predicate_queries.cmake)
physim_add_program(physim-language-array-map tests/fixtures/language/array_map.phys)
add_test(NAME language_array_map_runtime COMMAND physim-language-array-map)
add_test(NAME language_array_map COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-map-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_map.cmake)
physim_add_program(physim-language-array-compact-map tests/fixtures/language/array_compact_map.phys)
add_test(NAME language_array_compact_map_runtime COMMAND physim-language-array-compact-map)
add_test(NAME language_array_compact_map COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-array-compact-map-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_compact_map.cmake)
physim_add_program(physim-language-array-flat-map tests/fixtures/language/array_flat_map.phys)
add_test(NAME language_array_flat_map_runtime COMMAND physim-language-array-flat-map)
add_test(NAME language_array_flat_map COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-array-flat-map-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_flat_map.cmake)
physim_add_program(physim-language-array-while-selection
  tests/fixtures/language/array_while_selection.phys)
add_test(NAME language_array_while_selection_runtime COMMAND physim-language-array-while-selection)
add_test(NAME language_array_while_selection COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-array-while-selection-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_while_selection.cmake)
physim_add_program(physim-language-string-while-selection
  tests/fixtures/language/string_while_selection.phys)
add_test(NAME language_string_while_selection_runtime COMMAND physim-language-string-while-selection)
add_test(NAME language_string_while_selection COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-while-selection-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_while_selection.cmake)
physim_add_program(physim-language-string-filter
  tests/fixtures/language/string_filter.phys)
add_test(NAME language_string_filter_runtime COMMAND physim-language-string-filter)
add_test(NAME language_string_filter COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-filter-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_filter.cmake)
physim_add_program(physim-language-string-predicate-queries
  tests/fixtures/language/string_predicate_queries.phys)
add_test(NAME language_string_predicate_queries_runtime
  COMMAND physim-language-string-predicate-queries)
add_test(NAME language_string_predicate_queries COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-predicate-queries-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_predicate_queries.cmake)
physim_add_program(physim-language-string-predicate-search
  tests/fixtures/language/string_predicate_search.phys)
add_test(NAME language_string_predicate_search_runtime
  COMMAND physim-language-string-predicate-search)
add_test(NAME language_string_predicate_search COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-predicate-search-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_predicate_search.cmake)
physim_add_program(physim-language-string-map
  tests/fixtures/language/string_map.phys)
add_test(NAME language_string_map_runtime COMMAND physim-language-string-map)
add_test(NAME language_string_map COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-map-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_map.cmake)
physim_add_program(physim-language-string-compact-map
  tests/fixtures/language/string_compact_map.phys)
add_test(NAME language_string_compact_map_runtime COMMAND physim-language-string-compact-map)
add_test(NAME language_string_compact_map COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-compact-map-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_compact_map.cmake)
physim_add_program(physim-language-string-flat-map
  tests/fixtures/language/string_flat_map.phys)
add_test(NAME language_string_flat_map_runtime COMMAND physim-language-string-flat-map)
add_test(NAME language_string_flat_map COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-flat-map-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_flat_map.cmake)
physim_add_program(physim-language-array-reduce tests/fixtures/language/array_reduce.phys)
add_test(NAME language_array_reduce_runtime COMMAND physim-language-array-reduce)
add_test(NAME language_array_reduce COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-reduce-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_reduce.cmake)
physim_add_program(physim-language-string-reduce
  tests/fixtures/language/string_reduce.phys)
add_test(NAME language_string_reduce_runtime COMMAND physim-language-string-reduce)
add_test(NAME language_string_reduce COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-reduce-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_reduce.cmake)
physim_add_program(physim-language-sequence-for-each
  tests/fixtures/language/sequence_for_each.phys)
add_test(NAME language_sequence_for_each_runtime COMMAND physim-language-sequence-for-each)
add_test(NAME language_sequence_for_each COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-sequence-for-each-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_sequence_for_each.cmake)
physim_add_program(physim-language-free-overloads tests/fixtures/language/free_overloads.phys)
add_test(NAME language_free_overloads_runtime COMMAND physim-language-free-overloads)
add_test(NAME language_free_overloads COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-free-overloads-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_free_overloads.cmake)
physim_add_program(physim-language-generic-free-overloads
  tests/fixtures/language/generic_free_overloads.phys)
add_test(NAME language_generic_free_overloads_runtime
  COMMAND physim-language-generic-free-overloads)
add_test(NAME language_generic_free_overloads COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-generic-free-overloads-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_generic_free_overloads.cmake)
physim_add_program(physim-language-method-overloads
  tests/fixtures/language/method_overloads.phys)
add_test(NAME language_method_overloads_runtime COMMAND physim-language-method-overloads)
add_test(NAME language_method_overloads COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-method-overloads-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_method_overloads.cmake)
physim_add_program(physim-language-array-first-index-strings
  tests/fixtures/language/array_first_index_strings.phys)
add_test(NAME language_array_first_index_strings_runtime COMMAND physim-language-array-first-index-strings)
physim_add_program(physim-language-array-reversed tests/fixtures/language/array_reversed.phys)
add_test(NAME language_array_reversed_runtime COMMAND physim-language-array-reversed)
physim_add_program(physim-language-array-swap-at tests/fixtures/language/array_swap_at.phys)
add_test(NAME language_array_swap_at_runtime COMMAND physim-language-array-swap-at)
add_test(NAME language_array_swap_at COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-swap-at-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_swap_at.cmake)
physim_add_program(physim-language-array-subrange-methods
  tests/fixtures/language/array_subrange_methods.phys)
add_test(NAME language_array_subrange_methods_runtime COMMAND physim-language-array-subrange-methods)
add_test(NAME language_array_subrange_methods COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-array-subrange-methods-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_subrange_methods.cmake)
physim_add_program(physim-language-array-edge-selection
  tests/fixtures/language/array_edge_selection.phys)
add_test(NAME language_array_edge_selection_runtime COMMAND physim-language-array-edge-selection)
add_test(NAME language_array_edge_selection COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-array-edge-selection-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_edge_selection.cmake)
physim_add_program(physim-language-string-edge-selection
  tests/fixtures/language/string_edge_selection.phys)
add_test(NAME language_string_edge_selection_runtime COMMAND physim-language-string-edge-selection)
add_test(NAME language_string_edge_selection COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-edge-selection-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_edge_selection.cmake)
physim_add_program(physim-language-array-repeated tests/fixtures/language/array_repeated.phys)
physim_add_program(physim-language-array-repeated-negative tests/fixtures/language/array_repeated_negative.phys)
add_test(NAME language_array_repeated_runtime COMMAND physim-language-array-repeated)
add_test(NAME language_array_repeated COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-array-repeated-check
  -DPROGRAM=$<TARGET_FILE:physim-language-array-repeated-negative>
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_repeated.cmake)
physim_add_program(physim-language-array-sorted tests/fixtures/language/array_sorted.phys)
add_test(NAME language_array_sorted_runtime COMMAND physim-language-array-sorted)
physim_add_program(physim-language-array-sort-by tests/fixtures/language/array_sort_by.phys)
add_test(NAME language_array_sort_by_runtime COMMAND physim-language-array-sort-by)
physim_add_program(physim-language-array-extrema tests/fixtures/language/array_extrema.phys)
add_test(NAME language_array_extrema_runtime COMMAND physim-language-array-extrema)
add_test(NAME language_array_extrema COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-extrema-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_extrema.cmake)
physim_add_program(physim-language-array-remove-all tests/fixtures/language/array_remove_all.phys)
add_test(NAME language_array_remove_all_runtime COMMAND physim-language-array-remove-all)
add_test(NAME language_array_remove_all COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-remove-all-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_remove_all.cmake)
physim_add_program(physim-language-array-contents tests/fixtures/language/array_contents.phys)
add_test(NAME language_array_contents_runtime COMMAND physim-language-array-contents)
add_test(NAME language_array_contents COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-contents-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_contents.cmake)
physim_add_program(physim-language-array-edge-removal tests/fixtures/language/array_edge_removal.phys)
add_test(NAME language_array_edge_removal_runtime COMMAND physim-language-array-edge-removal)
add_test(NAME language_array_edge_removal COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-edge-removal-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_edge_removal.cmake)
add_test(NAME language_array_sorted COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-array-sorted-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_sorted.cmake)
add_test(NAME language_static_units COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-static-units-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_static_units.cmake)
physim_add_program(physim-language-quantity-operators tests/fixtures/language/quantity_operators.phys)
physim_add_program(physim-language-quantity-operator-error tests/fixtures/language/quantity_operator_error.phys)
physim_add_program(physim-language-quantity-scalar-error tests/fixtures/language/quantity_scalar_error.phys)
add_test(NAME language_quantity_operators_runtime COMMAND physim-language-quantity-operators)
add_test(NAME language_quantity_operator_error COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:physim-language-quantity-operator-error>
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_quantity_operator_error.cmake)
add_test(NAME language_quantity_scalar_error COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:physim-language-quantity-scalar-error>
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_quantity_scalar_error.cmake)
physim_add_program(physim-language-medium-values tests/fixtures/language/medium_values.phys)
physim_add_program(physim-language-submersion-values tests/fixtures/language/submersion_values.phys)
physim_add_program(physim-language-submersion-invalid-radius tests/fixtures/language/submersion_invalid_radius.phys)
physim_add_program(physim-language-submersion-invalid-range tests/fixtures/language/submersion_invalid_range.phys)
add_test(NAME language_submersion_values_runtime COMMAND physim-language-submersion-values)
add_test(NAME language_submersion COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DPROGRAM_RADIUS=$<TARGET_FILE:physim-language-submersion-invalid-radius>
  -DPROGRAM_RANGE=$<TARGET_FILE:physim-language-submersion-invalid-range>
  -DWORK=${CMAKE_BINARY_DIR}/language-submersion-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_submersion.cmake)
physim_add_program(physim-language-medium-invalid-density tests/fixtures/language/medium_invalid_density.phys)
physim_add_program(physim-language-medium-invalid-drag tests/fixtures/language/medium_invalid_drag.phys)
physim_add_program(physim-language-medium-invalid-stokes tests/fixtures/language/medium_invalid_stokes.phys)
add_test(NAME language_medium_values_runtime COMMAND physim-language-medium-values)
add_test(NAME language_medium COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-medium-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_medium.cmake)
add_test(NAME language_medium_invalid_density COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:physim-language-medium-invalid-density>
  -DSOURCE=medium_invalid_density -DEXPECT=Medium.density.and.viscosity
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_medium_error.cmake)
add_test(NAME language_medium_invalid_drag COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:physim-language-medium-invalid-drag>
  -DSOURCE=medium_invalid_drag -DEXPECT=Invalid.medium.drag.parameters
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_medium_error.cmake)
add_test(NAME language_medium_invalid_stokes COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:physim-language-medium-invalid-stokes>
  -DSOURCE=medium_invalid_stokes -DEXPECT=Invalid.force.parameters
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_medium_error.cmake)
physim_add_program(physim-language-material-values tests/fixtures/language/material_values.phys)
physim_add_program(physim-language-material-invalid tests/fixtures/language/material_invalid.phys)
add_test(NAME language_material_values_runtime COMMAND physim-language-material-values)
add_test(NAME language_material COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-material-check
  -DPROGRAM=$<TARGET_FILE:physim-language-material-invalid>
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_material.cmake)
physim_add_program(physim-language-string-iteration tests/fixtures/language/string_iteration.phys)
add_test(NAME language_string_iteration_runtime COMMAND physim-language-string-iteration)
physim_add_program(physim-language-string-ownership tests/fixtures/language/string_ownership.phys)
add_test(NAME language_string_ownership_runtime COMMAND physim-language-string-ownership)
add_test(NAME language_string_split COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-string-split-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_split.cmake)
physim_add_program(physim-language-string-repeated tests/fixtures/language/string_repeated.phys)
physim_add_program(physim-language-string-repeated-negative tests/fixtures/language/string_repeated_negative.phys)
add_test(NAME language_string_repeated_runtime COMMAND physim-language-string-repeated)
add_test(NAME language_string_repeated COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-repeated-check
  -DPROGRAM=$<TARGET_FILE:physim-language-string-repeated-negative>
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_repeated.cmake)
physim_add_program(physim-language-string-trimmed tests/fixtures/language/string_trimmed.phys)
add_test(NAME language_string_trimmed_runtime COMMAND physim-language-string-trimmed)
add_test(NAME language_string_trimmed COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-trimmed-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_trimmed.cmake)
physim_add_program(physim-language-string-interpolation tests/fixtures/language/string_interpolation.phys)
add_test(NAME language_string_interpolation_runtime COMMAND physim-language-string-interpolation)
add_test(NAME language_string_interpolation COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-string-interpolation-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_interpolation.cmake)
physim_add_program(physim-language-conditional-expression tests/fixtures/language/conditional_expression.phys)
add_test(NAME language_conditional_expression_runtime COMMAND physim-language-conditional-expression)
add_test(NAME language_conditional_expression COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-conditional-expression-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_conditional_expression.cmake)
physim_add_program(physim-language-guard tests/fixtures/language/guard.phys)
add_test(NAME language_guard_runtime COMMAND physim-language-guard)
add_test(NAME language_guard COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-guard-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_guard.cmake)
physim_add_program(physim-language-numeric-parse tests/fixtures/language/numeric_parse.phys)
add_test(NAME language_numeric_parse_runtime COMMAND physim-language-numeric-parse)
add_test(NAME language_numeric_parse COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-numeric-parse-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_numeric_parse.cmake)
physim_add_program(physim-language-array-safe-index tests/fixtures/language/array_safe_index.phys)
add_test(NAME language_array_safe_index_runtime COMMAND physim-language-array-safe-index)
physim_add_program(physim-language-string-safe-index tests/fixtures/language/string_safe_index.phys)
add_test(NAME language_string_safe_index_runtime COMMAND physim-language-string-safe-index)
physim_add_program(physim-language-array-safe-remove tests/fixtures/language/array_safe_remove.phys)
add_test(NAME language_array_safe_remove_runtime COMMAND physim-language-array-safe-remove)
add_test(NAME language_array_safe_remove COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-array-safe-remove-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_array_safe_remove.cmake)
physim_add_experiment(physim-language-array-safe-remove-module tests/fixtures/language/array_safe_remove_module.phys)
add_executable(physim-language-array-safe-remove-module-tests tests/test_language_array_safe_remove_module.c)
target_link_libraries(physim-language-array-safe-remove-module-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-language-array-safe-remove-module-tests)
add_dependencies(physim-language-array-safe-remove-module-tests physim-language-array-safe-remove-module)
add_test(NAME language_array_safe_remove_module COMMAND physim-language-array-safe-remove-module-tests
  $<TARGET_FILE:physim-language-array-safe-remove-module>)
physim_add_program(physim-language-attempt tests/fixtures/language/attempt.phys)
add_test(NAME language_attempt_runtime COMMAND physim-language-attempt)
add_test(NAME language_attempt COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc>
  -DWORK=${CMAKE_BINARY_DIR}/language-attempt-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_attempt.cmake)
physim_add_experiment(physim-language-attempt-module tests/fixtures/language/attempt_module.phys)
add_executable(physim-language-attempt-module-tests tests/test_language_attempt_module.c)
target_link_libraries(physim-language-attempt-module-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-language-attempt-module-tests)
add_dependencies(physim-language-attempt-module-tests physim-language-attempt-module)
add_test(NAME language_attempt_module COMMAND physim-language-attempt-module-tests
  $<TARGET_FILE:physim-language-attempt-module>)
physim_add_program(physim-language-multiline-strings tests/fixtures/language/multiline_strings.phys)
add_test(NAME language_multiline_strings_runtime COMMAND physim-language-multiline-strings)
add_test(NAME language_multiline_strings_crlf COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-multiline-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_multiline.cmake)
physim_add_program(physim-language-unicode-escapes tests/fixtures/language/unicode_escapes.phys)
add_test(NAME language_unicode_escapes_runtime COMMAND physim-language-unicode-escapes)
add_test(NAME language_unicode_escapes COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-unicode-escape-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_unicode_escapes.cmake)
physim_add_program(physim-language-int-math tests/fixtures/language/int_math.phys)
physim_add_program(physim-language-int-abs-overflow tests/fixtures/language/int_abs_overflow.phys)
physim_add_program(physim-language-int-clamp-bounds tests/fixtures/language/int_clamp_bounds.phys)
add_test(NAME language_int_math_runtime COMMAND physim-language-int-math)
add_test(NAME language_int_math_errors COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-int-math-check
  -DABS_PROGRAM=$<TARGET_FILE:physim-language-int-abs-overflow>
  -DCLAMP_PROGRAM=$<TARGET_FILE:physim-language-int-clamp-bounds>
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_int_math.cmake)
add_test(NAME language_string_index COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-string-index-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_string_index.cmake)
add_test(NAME language_switch_optionals COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-switch-optionals-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_switch_optionals.cmake)
physim_add_program(physim-language-enum-payload tests/fixtures/language/enum_payload.phys)
add_test(NAME language_enum_payload_runtime COMMAND physim-language-enum-payload)
physim_add_program(physim-language-enum-raw-values tests/fixtures/language/enum_raw_values.phys)
add_test(NAME language_enum_raw_values_runtime COMMAND physim-language-enum-raw-values)
physim_add_program(physim-language-struct-defaults tests/fixtures/language/struct_defaults.phys)
add_test(NAME language_struct_defaults_runtime COMMAND physim-language-struct-defaults)
add_test(NAME language_struct_defaults COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-struct-defaults-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_struct_defaults.cmake)
physim_add_program(physim-language-struct-initializers
  tests/fixtures/language/struct_initializers.phys)
add_test(NAME language_struct_initializers_runtime COMMAND physim-language-struct-initializers)
add_test(NAME language_struct_initializers COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-struct-initializers-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_struct_initializers.cmake)
add_test(NAME language_enum_raw_values COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-enum-raw-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_raw_values.cmake)
physim_add_program(physim-language-enum-owned-payload
  tests/fixtures/language/enum_owned_payload.phys)
add_test(NAME language_enum_owned_payload_runtime COMMAND physim-language-enum-owned-payload)
physim_add_program(physim-language-enum-payload-module
  tests/fixtures/language/enum_payload_module/main.phys)
add_test(NAME language_enum_payload_module_runtime COMMAND physim-language-enum-payload-module)
physim_add_program(physim-language-unicode-modules
  tests/fixtures/language/unicode_modules/main.phys)
add_test(NAME language_unicode_modules_runtime COMMAND physim-language-unicode-modules)
add_test(NAME language_enum_payload COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-enum-payload-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_enum_payload.cmake)
add_test(NAME language_switch_patterns COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-patterns-check
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_patterns.cmake)
add_test(NAME language_modules_runtime COMMAND physim-language-modules)
physim_add_program(physim-language-modules-error tests/fixtures/language/modules/error.phys)
add_test(NAME language_modules_runtime_error COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:physim-language-modules-error>
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_module_runtime.cmake)
add_test(NAME language_modules COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-modules-check
  -DEXAMPLE=${CMAKE_SOURCE_DIR}/examples/language/modules/main.phys
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_modules.cmake)
physim_add_program(physim-language-matrix-values examples/language/coordinate_frames.phys)
add_test(NAME language_matrix_values COMMAND physim-language-matrix-values)
physim_add_program(physim-language-optional-values examples/language/optional_values.phys)
add_test(NAME language_optional_values COMMAND physim-language-optional-values)
set_tests_properties(language_optional_values PROPERTIES TIMEOUT 90)
physim_add_program(physim-language-rng-streams examples/language/rng_streams.phys)
add_test(NAME language_rng_streams COMMAND physim-language-rng-streams)
set_tests_properties(language_rng_streams PROPERTIES TIMEOUT 90)
physim_add_program(physim-language-bindings examples/language/optional_bindings.phys)
add_test(NAME language_bindings COMMAND physim-language-bindings)
set_tests_properties(language_bindings PROPERTIES TIMEOUT 90)
physim_add_program(physim-language-sweep-values examples/language/sweeps.phys)
add_test(NAME language_sweep_values COMMAND physim-language-sweep-values)
physim_add_experiment(physim-language-sweep examples/language/fast_sphere.phys)
physim_add_experiment(physim-language-sweep-error tests/fixtures/language/sweep_error.phys)
add_executable(physim-language-sweep-tests tests/test_language_sweep.c)
target_link_libraries(physim-language-sweep-tests PRIVATE physim_platform)
ps_warnings(physim-language-sweep-tests)
add_dependencies(physim-language-sweep-tests physim-language-sweep physim-language-sweep-error physim-runner)
add_test(NAME language_sweep COMMAND physim-language-sweep-tests
  $<TARGET_FILE:physim-language-sweep> $<TARGET_FILE:physim-language-sweep-error>
  $<TARGET_FILE:physim-runner> "${CMAKE_BINARY_DIR}/language-runs")
set_tests_properties(language_sweep PROPERTIES TIMEOUT 90)
physim_add_program(physim-language-graph-values examples/language/constraint_graph.phys)
add_test(NAME language_graph_values COMMAND physim-language-graph-values)
physim_add_experiment(physim-language-graph examples/language/coupled_bodies.phys)
physim_add_experiment(physim-language-graph-error tests/fixtures/language/graph_error.phys)
add_executable(physim-language-graph-tests tests/test_language_graph.c)
target_link_libraries(physim-language-graph-tests PRIVATE physim_platform)
ps_warnings(physim-language-graph-tests)
add_dependencies(physim-language-graph-tests physim-language-graph physim-language-graph-error physim-runner)
add_test(NAME language_graph COMMAND physim-language-graph-tests
  $<TARGET_FILE:physim-language-graph> $<TARGET_FILE:physim-language-graph-error>
  $<TARGET_FILE:physim-runner> "${CMAKE_BINARY_DIR}/language-runs")
set_tests_properties(language_graph PROPERTIES TIMEOUT 90)
physim_add_program(physim-language-joint-values examples/language/distance_joints.phys)
add_test(NAME language_joint_values COMMAND physim-language-joint-values)
physim_add_experiment(physim-language-joint examples/language/joint_pendulum.phys)
physim_add_experiment(physim-language-joint-error tests/fixtures/language/joint_error.phys)
add_executable(physim-language-joint-tests tests/test_language_joint.c)
target_link_libraries(physim-language-joint-tests PRIVATE physim_platform)
ps_warnings(physim-language-joint-tests)
add_dependencies(physim-language-joint-tests physim-language-joint physim-language-joint-error physim-runner)
add_test(NAME language_joint COMMAND physim-language-joint-tests
  $<TARGET_FILE:physim-language-joint> $<TARGET_FILE:physim-language-joint-error>
  $<TARGET_FILE:physim-runner> "${CMAKE_BINARY_DIR}/language-runs")
set_tests_properties(language_joint PROPERTIES TIMEOUT 90)
physim_add_program(physim-language-contact-values examples/language/contacts.phys)
add_test(NAME language_contact_values COMMAND physim-language-contact-values)
physim_add_experiment(physim-language-contact-error tests/fixtures/language/contact_error.phys)
physim_add_experiment(physim-language-contact examples/language/box_contacts.phys)
add_executable(physim-language-contact-tests tests/test_language_contact.c)
target_link_libraries(physim-language-contact-tests PRIVATE physim_platform)
ps_warnings(physim-language-contact-tests)
add_dependencies(physim-language-contact-tests physim-language-contact physim-language-contact-error box_floor physim-runner)
add_test(NAME language_contact COMMAND physim-language-contact-tests
  $<TARGET_FILE:physim-language-contact> $<TARGET_FILE:box_floor> $<TARGET_FILE:physim-language-contact-error>
  $<TARGET_FILE:physim-runner> "${CMAKE_BINARY_DIR}/language-runs")
set_tests_properties(language_contact PROPERTIES TIMEOUT 90)
add_test(NAME language_body_values COMMAND physim-language-body-values)
physim_add_experiment(physim-language-body examples/language/spinning_body.phys)
physim_add_experiment(physim-language-body-error tests/fixtures/language/body_error.phys)
add_executable(physim-language-body-tests tests/test_language_body.c)
target_link_libraries(physim-language-body-tests PRIVATE physim_platform)
ps_warnings(physim-language-body-tests)
add_dependencies(physim-language-body-tests physim-language-body physim-language-body-error physim-runner)
add_test(NAME language_body COMMAND physim-language-body-tests
  $<TARGET_FILE:physim-language-body> $<TARGET_FILE:physim-language-body-error>
  $<TARGET_FILE:physim-runner> "${CMAKE_BINARY_DIR}/language-runs")
set_tests_properties(language_body PROPERTIES TIMEOUT 60)
add_executable(physim-language-sensor-tests tests/test_language_sensor.c)
target_link_libraries(physim-language-sensor-tests PRIVATE physim_platform)
ps_warnings(physim-language-sensor-tests)
add_dependencies(physim-language-sensor-tests physim-language-sensors physim-language-sensor-error physim-runner)
add_test(NAME language_sensor COMMAND physim-language-sensor-tests
  $<TARGET_FILE:physim-language-sensors> $<TARGET_FILE:physim-language-sensor-error>
  $<TARGET_FILE:physim-runner> "${CMAKE_BINARY_DIR}/language-runs")
set_tests_properties(language_sensor PROPERTIES TIMEOUT 90)
add_executable(physim-uncertain-projectile-parity-tests tests/test_uncertain_projectile_parity.c)
target_link_libraries(physim-uncertain-projectile-parity-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-uncertain-projectile-parity-tests)
add_dependencies(physim-uncertain-projectile-parity-tests uncertain_projectile
  physim-language-uncertain-projectile pendulum_analysis physim-language-analysis-sensors
  physim-runner)
add_test(NAME language_uncertain_projectile_parity COMMAND physim-uncertain-projectile-parity-tests
  $<TARGET_FILE:physim-runner> $<TARGET_FILE:uncertain_projectile>
  $<TARGET_FILE:physim-language-uncertain-projectile> "${CMAKE_BINARY_DIR}"
  $<TARGET_FILE:pendulum_analysis> $<TARGET_FILE:physim-language-analysis-sensors>)
set_tests_properties(language_uncertain_projectile_parity PROPERTIES TIMEOUT 90)
add_library(physim-language-pendulum-reference MODULE examples/pendulum/main.c)
target_compile_definitions(physim-language-pendulum-reference PRIVATE PS_PENDULUM_METHOD=PS_SYMPLECTIC)
target_link_libraries(physim-language-pendulum-reference PRIVATE physim_core)
set_target_properties(physim-language-pendulum-reference PROPERTIES PREFIX "")
ps_warnings(physim-language-pendulum-reference)
add_executable(physim-language-experiment-tests tests/test_language_experiment.c)
target_link_libraries(physim-language-experiment-tests PRIVATE physim_platform)
ps_warnings(physim-language-experiment-tests)
add_dependencies(physim-language-experiment-tests physim-language-pendulum physim-language-pendulum-rk4
  physim-language-pendulum-integrator physim-language-pendulum-rk45 pendulum_rk45
  physim-language-pendulum-verlet pendulum_verlet
  pendulum physim-language-projectile
  physim-language-error physim-language-scene_error physim-language-create_error physim-language-pendulum-reference physim-runner
  physim-language-random physim-language-rng-run-streams physim-language-random-error physim-language-random-create-error
  physim-language-scene-shapes physim-language-scene-shapes-error physim-language-spring spring
  physim-language-polyline-empty)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/language-runs")
add_test(NAME language_experiment COMMAND physim-language-experiment-tests
  $<TARGET_FILE:physim-language-pendulum> $<TARGET_FILE:physim-language-pendulum-reference>
  $<TARGET_FILE:physim-language-projectile> $<TARGET_FILE:physim-language-error>
  $<TARGET_FILE:physim-language-scene_error> $<TARGET_FILE:physim-language-create_error>
  $<TARGET_FILE:physim-runner> "${CMAKE_BINARY_DIR}/language-runs"
  $<TARGET_FILE:physim-language-random> $<TARGET_FILE:physim-language-random-error>
  $<TARGET_FILE:physim-language-random-create-error>
  $<TARGET_FILE:physim-language-scene-shapes> $<TARGET_FILE:physim-language-scene-shapes-error>
  $<TARGET_FILE:physim-language-spring> $<TARGET_FILE:spring>
  $<TARGET_FILE:physim-language-polyline-empty>
  $<TARGET_FILE:physim-language-rng-run-streams>
  $<TARGET_FILE:physim-language-pendulum-rk4> $<TARGET_FILE:pendulum>
  $<TARGET_FILE:physim-language-pendulum-integrator>
  $<TARGET_FILE:physim-language-pendulum-rk45> $<TARGET_FILE:pendulum_rk45>
  $<TARGET_FILE:physim-language-pendulum-verlet> $<TARGET_FILE:pendulum_verlet>)
set_tests_properties(language_experiment PROPERTIES TIMEOUT 90)
add_test(NAME language_cli COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-cli
  -DEXAMPLE=${CMAKE_SOURCE_DIR}/examples/language/energy.phys
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_cli.cmake)
set_tests_properties(language_cli PROPERTIES TIMEOUT 90)
add_test(NAME language_native COMMAND ${CMAKE_COMMAND}
  -DCOMPILER=$<TARGET_FILE:physimc> -DWORK=${CMAKE_BINARY_DIR}/language-native
  -DSOURCE_DIR=${CMAKE_SOURCE_DIR} -DC_COMPILER=${CMAKE_C_COMPILER}
  -DGENERATOR=${CMAKE_GENERATOR} -DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
  -DCONFIG=$<CONFIG> -DC_FLAGS=${CMAKE_C_FLAGS}
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_native.cmake)
set_tests_properties(language_native PROPERTIES TIMEOUT 300)
add_executable(physim-language-lexer-tests tests/test_language_lexer.c)
target_link_libraries(physim-language-lexer-tests PRIVATE physim_language)
ps_warnings(physim-language-lexer-tests)
add_test(NAME language_lexer COMMAND physim-language-lexer-tests)
set_tests_properties(language_lexer PROPERTIES TIMEOUT 30)
add_executable(physim-language-parser-tests tests/test_language_parser.c)
target_link_libraries(physim-language-parser-tests PRIVATE physim_language)
ps_warnings(physim-language-parser-tests)
add_test(NAME language_parser COMMAND physim-language-parser-tests)
set_tests_properties(language_parser PROPERTIES TIMEOUT 30)
add_executable(physim-language-checker-tests tests/test_language_checker.c)
target_link_libraries(physim-language-checker-tests PRIVATE physim_language)
ps_warnings(physim-language-checker-tests)
add_test(NAME language_checker COMMAND physim-language-checker-tests)
set_tests_properties(language_checker PROPERTIES TIMEOUT 30)
option(PHYSIM_BUILD_FUZZERS "Build Clang libFuzzer IPC target with AddressSanitizer" OFF)
if(PHYSIM_BUILD_FUZZERS)
  if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang" OR NOT CMAKE_C_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
    message(FATAL_ERROR "Fuzzers require the clang/clang.exe driver (not clang-cl or MSVC)")
  endif()
  get_target_property(fuzz_core_sources physim_core SOURCES)
  add_library(physim_fuzz_core STATIC ${fuzz_core_sources})
  target_include_directories(physim_fuzz_core PUBLIC include)
  target_compile_options(physim_fuzz_core PRIVATE -fsanitize=fuzzer-no-link,address -fno-omit-frame-pointer)
  target_compile_definitions(physim_fuzz_core PRIVATE _CRT_SECURE_NO_WARNINGS _POSIX_C_SOURCE=200809L)
  if(NOT WIN32)
    target_link_libraries(physim_fuzz_core PUBLIC m)
  endif()
  add_executable(physim-protocol-libfuzzer tests/fuzz_protocol.c src/protocol.c)
  target_include_directories(physim-protocol-libfuzzer PRIVATE src)
  target_link_libraries(physim-protocol-libfuzzer PRIVATE physim_fuzz_core)
  target_compile_options(physim-protocol-libfuzzer PRIVATE -fsanitize=fuzzer,address -fno-omit-frame-pointer)
  target_link_options(physim-protocol-libfuzzer PRIVATE -fsanitize=fuzzer,address)
  if(WIN32)
    target_compile_options(physim_fuzz_core PRIVATE -fms-runtime-lib=dll)
    target_compile_options(physim-protocol-libfuzzer PRIVATE -fms-runtime-lib=dll)
    target_link_options(physim-protocol-libfuzzer PRIVATE -fms-runtime-lib=dll -shared-libasan)
    target_link_options(physim-protocol-libfuzzer PRIVATE
      -Wl,/nodefaultlib:libcmt,/nodefaultlib:libucrt,/nodefaultlib:libvcruntime)
    target_link_libraries(physim-protocol-libfuzzer PRIVATE msvcrt ucrt vcruntime)
  endif()
endif()
add_executable(physim-protocol-fuzz tests/fuzz_protocol.c)
target_compile_definitions(physim-protocol-fuzz PRIVATE PS_FUZZ_STANDALONE)
target_link_libraries(physim-protocol-fuzz PRIVATE physim_platform)
ps_warnings(physim-protocol-fuzz)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/protocol-fuzz")
add_test(NAME protocol_mutations COMMAND physim-protocol-fuzz)
set_tests_properties(protocol_mutations PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/protocol-fuzz" TIMEOUT 120)
add_executable(physim-run-fuzz tests/fuzz_run.c)
target_link_libraries(physim-run-fuzz PRIVATE physim_core)
ps_warnings(physim-run-fuzz)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/run-fuzz")
add_test(NAME run_mutations COMMAND physim-run-fuzz)
set_tests_properties(run_mutations PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/run-fuzz" TIMEOUT 180)
add_executable(physim-report-fuzz tests/fuzz_report.c)
target_link_libraries(physim-report-fuzz PRIVATE physim_core)
ps_warnings(physim-report-fuzz)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/report-fuzz")
add_test(NAME report_mutations COMMAND physim-report-fuzz)
set_tests_properties(report_mutations PROPERTIES WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/report-fuzz" TIMEOUT 300)
add_executable(physim-hashmap-tests tests/test_hashmap.c)
target_link_libraries(physim-hashmap-tests PRIVATE physim_core)
ps_warnings(physim-hashmap-tests)
add_test(NAME hashmap COMMAND physim-hashmap-tests)
add_executable(physim-string-view-tests tests/test_string_view.c)
target_link_libraries(physim-string-view-tests PRIVATE physim_core)
ps_warnings(physim-string-view-tests)
add_test(NAME string_view COMMAND physim-string-view-tests)
add_executable(physim-array-tests tests/test_array.c)
target_link_libraries(physim-array-tests PRIVATE physim_core)
ps_warnings(physim-array-tests)
add_test(NAME array COMMAND physim-array-tests)
add_executable(physim-language-values-tests tests/test_language_values.c)
target_link_libraries(physim-language-values-tests PRIVATE physim_core)
ps_warnings(physim-language-values-tests)
add_test(NAME language_values COMMAND physim-language-values-tests)
set_tests_properties(language_values PROPERTIES TIMEOUT 30)
add_executable(physim-language-string-tests tests/test_language_string.c)
target_link_libraries(physim-language-string-tests PRIVATE physim_core)
ps_warnings(physim-language-string-tests)
add_test(NAME language_string_storage COMMAND physim-language-string-tests)
add_executable(physim-language-locale-tests tests/test_language_locale.c)
target_link_libraries(physim-language-locale-tests PRIVATE physim_core)
ps_warnings(physim-language-locale-tests)
add_test(NAME language_locale COMMAND physim-language-locale-tests)
add_executable(physim-language-values-exit tests/test_language_values_exit.c)
target_link_libraries(physim-language-values-exit PRIVATE physim_core)
ps_warnings(physim-language-values-exit)
add_test(NAME language_values_exit COMMAND ${CMAKE_COMMAND}
  -DPROGRAM=$<TARGET_FILE:physim-language-values-exit>
  -P ${CMAKE_SOURCE_DIR}/tests/check_language_values_exit.cmake)
set_tests_properties(language_values_exit PROPERTIES TIMEOUT 30)
add_executable(physim-crc-tests tests/test_crc.c)
target_link_libraries(physim-crc-tests PRIVATE physim_platform)
ps_warnings(physim-crc-tests)
add_test(NAME crc COMMAND physim-crc-tests)
add_test(NAME crc_report_roundtrip COMMAND physim-crc-tests --roundtrip ${CMAKE_SOURCE_DIR}/tests/fixtures/crc-report-v1.bin)
if(PHYSIM_BUILD_APP)
  add_executable(physim-ui-geometry-tests tests/test_ui_geometry.c app/ui_geometry.c app/ui_backend.c)
  target_include_directories(physim-ui-geometry-tests PRIVATE app third_party)
  target_link_libraries(physim-ui-geometry-tests PRIVATE physim_core SDL3::SDL3)
  ps_warnings(physim-ui-geometry-tests)
  add_test(NAME ui_geometry COMMAND physim-ui-geometry-tests)
  add_executable(physim-editor-clipboard-tests tests/test_editor_clipboard.c app/ui_backend.c)
  target_include_directories(physim-editor-clipboard-tests PRIVATE app third_party)
  target_link_libraries(physim-editor-clipboard-tests PRIVATE physim_core SDL3::SDL3)
  ps_warnings(physim-editor-clipboard-tests)
  add_test(NAME editor_clipboard COMMAND physim-editor-clipboard-tests)
endif()
option(PHYSIM_BUILD_BENCHMARKS "Build reproducible performance workloads" OFF)
if(PHYSIM_BUILD_BENCHMARKS)
  if(PHYSIM_BUILD_APP)
    add_executable(physim-ui-benchmark tools/ui_benchmark.c app/graphics.c app/ui_geometry.c app/ui_backend.c app/ui_sdl.c app/png.c)
    target_include_directories(physim-ui-benchmark PRIVATE app third_party)
    target_link_libraries(physim-ui-benchmark PRIVATE physim_platform physim_zlib SDL3::SDL3)
    if(WIN32)
      target_link_libraries(physim-ui-benchmark PRIVATE dwmapi)
    endif()
    target_compile_definitions(physim-ui-benchmark PRIVATE
      PS_BENCH_COMPILER="${CMAKE_C_COMPILER_ID}-${CMAKE_C_COMPILER_VERSION}"
      PS_BENCH_CONFIG="$<CONFIG>")
    if(WIN32)
      add_custom_command(TARGET physim-ui-benchmark POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:physim-ui-benchmark>)
    endif()
    ps_warnings(physim-ui-benchmark)
  endif()
  add_executable(physim-benchmark tools/benchmark.c)
  target_link_libraries(physim-benchmark PRIVATE physim_platform)
  target_compile_definitions(physim-benchmark PRIVATE
    PS_BENCH_COMPILER="${CMAKE_C_COMPILER_ID}-${CMAKE_C_COMPILER_VERSION}"
    PS_BENCH_CONFIG="$<CONFIG>")
  ps_warnings(physim-benchmark)
  add_test(NAME benchmark_smoke COMMAND physim-benchmark 257 2)
  set_tests_properties(benchmark_smoke PROPERTIES LABELS benchmark TIMEOUT 60)
  find_package(Python3 3.9 COMPONENTS Interpreter QUIET)
  if(Python3_Interpreter_FOUND)
    add_test(NAME benchmark_driver COMMAND ${Python3_EXECUTABLE}
      ${CMAKE_SOURCE_DIR}/tests/test_benchmark.py ${CMAKE_SOURCE_DIR}/tools/benchmark.py
      $<TARGET_FILE:physim-benchmark>)
    set_tests_properties(benchmark_driver PROPERTIES LABELS benchmark TIMEOUT 120)
    if(PHYSIM_BUILD_APP AND PHYSIM_GRAPHICS_TESTS)
      add_test(NAME ui_rendering COMMAND ${CMAKE_COMMAND}
        -DPYTHON=${Python3_EXECUTABLE} -DDRIVER=${CMAKE_SOURCE_DIR}/tools/ui_benchmark.py
        -DBENCHMARK=$<TARGET_FILE:physim-ui-benchmark> -DROOT=${CMAKE_BINARY_DIR}/ui-rendering
        -P ${CMAKE_SOURCE_DIR}/tests/ui_rendering.cmake)
      set_tests_properties(ui_rendering PROPERTIES LABELS "display;benchmark" TIMEOUT 120 RUN_SERIAL TRUE)
    endif()
  endif()
endif()
foreach(memory_test memory memory_owners)
  add_executable(physim-${memory_test}-tests tests/test_${memory_test}.c)
  target_link_libraries(physim-${memory_test}-tests PRIVATE physim_core)
  ps_warnings(physim-${memory_test}-tests)
  add_test(NAME ${memory_test} COMMAND physim-${memory_test}-tests)
endforeach()
add_executable(physim-math-tests tests/test_math.c)
target_link_libraries(physim-math-tests PRIVATE physim_core)
ps_warnings(physim-math-tests)
add_test(NAME math COMMAND physim-math-tests)
add_executable(physim-box-contact-tests tests/test_box_contacts.c)
target_link_libraries(physim-box-contact-tests PRIVATE physim_core)
ps_warnings(physim-box-contact-tests)
add_test(NAME box_contacts COMMAND physim-box-contact-tests)
add_executable(physim-contact-graph-tests tests/test_contact_graph.c)
target_link_libraries(physim-contact-graph-tests PRIVATE physim_core)
ps_warnings(physim-contact-graph-tests)
add_test(NAME contact_graph COMMAND physim-contact-graph-tests)
add_executable(physim-distance-joint-tests tests/test_distance_joint.c)
target_link_libraries(physim-distance-joint-tests PRIVATE physim_core)
ps_warnings(physim-distance-joint-tests)
add_test(NAME distance_joint COMMAND physim-distance-joint-tests)
add_executable(physim-constraint-graph-tests tests/test_constraint_graph.c)
target_link_libraries(physim-constraint-graph-tests PRIVATE physim_core)
ps_warnings(physim-constraint-graph-tests)
add_test(NAME constraint_graph COMMAND physim-constraint-graph-tests)
add_executable(physim-broad-phase-tests tests/test_broad_phase.c)
target_link_libraries(physim-broad-phase-tests PRIVATE physim_core)
ps_warnings(physim-broad-phase-tests)
add_test(NAME broad_phase COMMAND physim-broad-phase-tests)
add_executable(physim-sweep-tests tests/test_sweep.c)
target_link_libraries(physim-sweep-tests PRIVATE physim_core)
ps_warnings(physim-sweep-tests)
add_test(NAME sphere_sweep COMMAND physim-sweep-tests)
foreach(mode ccd discrete)
  add_library(collision_fast_${mode} MODULE examples/collision/main.c)
  target_link_libraries(collision_fast_${mode} PRIVATE physim_core)
  ps_warnings(collision_fast_${mode})
  target_compile_definitions(collision_fast_${mode} PRIVATE PS_COLLISION_SPEED=100.0)
endforeach()
target_compile_definitions(collision_fast_ccd PRIVATE PS_COLLISION_CCD=1)
target_compile_definitions(collision_fast_discrete PRIVATE PS_COLLISION_CCD=0)
add_executable(physim-collision-ccd-tests tests/verify_collision_ccd.c)
target_link_libraries(physim-collision-ccd-tests PRIVATE physim_platform)
ps_warnings(physim-collision-ccd-tests)
add_test(NAME collision_ccd_reference COMMAND physim-collision-ccd-tests $<TARGET_FILE:physim-runner>
  $<TARGET_FILE:collision_fast_ccd> $<TARGET_FILE:collision_fast_discrete>)
set_tests_properties(collision_ccd_reference PROPERTIES TIMEOUT 60)

foreach(mode inelastic oblique)
  add_library(box_collision_${mode} MODULE examples/box_collision/main.c)
  target_link_libraries(box_collision_${mode} PRIVATE physim_core)
  ps_warnings(box_collision_${mode})
endforeach()
target_compile_definitions(box_collision_inelastic PRIVATE PS_BOX_RESTITUTION=0.5)
target_compile_definitions(box_collision_oblique PRIVATE PS_BOX_ANGLE=0.35 PS_BOX_OFFSET=0.12 PS_BOX_RESTITUTION=0.6 PS_BOX_FRICTION=0.3)
add_executable(physim-box-collision-tests tests/verify_box_collision.c)
target_link_libraries(physim-box-collision-tests PRIVATE physim_platform)
ps_warnings(physim-box-collision-tests)
add_test(NAME box_collision_reference COMMAND physim-box-collision-tests $<TARGET_FILE:physim-runner>
  $<TARGET_FILE:box_collision> $<TARGET_FILE:box_collision_inelastic> $<TARGET_FILE:box_collision_oblique>)
set_tests_properties(box_collision_reference PROPERTIES TIMEOUT 60)
add_executable(physim-png-tests tests/test_png.c app/png.c)
target_include_directories(physim-png-tests PRIVATE app)
target_link_libraries(physim-png-tests PRIVATE physim_core physim_zlib)
ps_warnings(physim-png-tests)
add_test(NAME png COMMAND physim-png-tests ${CMAKE_BINARY_DIR}/png-reference.png)
add_executable(physim-scene-view-tests tests/test_scene_view.c)
target_include_directories(physim-scene-view-tests PRIVATE app)
target_link_libraries(physim-scene-view-tests PRIVATE physim_core)
ps_warnings(physim-scene-view-tests)
add_test(NAME scene_view COMMAND physim-scene-view-tests)
add_executable(physim-template-id-tests tests/test_template_ids.c)
target_link_libraries(physim-template-id-tests PRIVATE physim_platform)
ps_warnings(physim-template-id-tests)
add_test(NAME template_ids COMMAND physim-template-id-tests
  $<TARGET_FILE:pendulum> $<TARGET_FILE:projectile> $<TARGET_FILE:collision>
  $<TARGET_FILE:box_floor> $<TARGET_FILE:spring> $<TARGET_FILE:uncertain_projectile>
  $<TARGET_FILE:box_collision> $<TARGET_FILE:buoyancy>)
find_package(Python3 3.9 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
  add_test(NAME documentation_reference COMMAND ${Python3_EXECUTABLE}
    ${CMAKE_SOURCE_DIR}/tools/generate-reference.py --check)
  add_test(NAME png_decode COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/tests/verify_png.py ${CMAKE_BINARY_DIR})
  set_tests_properties(png PROPERTIES FIXTURES_SETUP png_files)
  set_tests_properties(png_decode PROPERTIES FIXTURES_REQUIRED png_files)
endif()
add_executable(physim-plot-view-tests tests/test_plot_view.c app/plot_view.c)
target_include_directories(physim-plot-view-tests PRIVATE app)
target_link_libraries(physim-plot-view-tests PRIVATE physim_core)
ps_warnings(physim-plot-view-tests)
add_test(NAME plot_view COMMAND physim-plot-view-tests)
add_executable(physim-measurement-tests tests/test_measurement.c)
target_link_libraries(physim-measurement-tests PRIVATE physim_core)
ps_warnings(physim-measurement-tests)
add_test(NAME measurement COMMAND physim-measurement-tests)
foreach(mode ideal dropped)
  add_library(sensor_${mode} MODULE examples/uncertain_projectile/main.c)
  target_link_libraries(sensor_${mode} PRIVATE physim_core)
  ps_warnings(sensor_${mode})
endforeach()
target_compile_definitions(sensor_ideal PRIVATE PS_SENSOR_IDEAL=1)
target_compile_definitions(sensor_dropped PRIVATE PS_SENSOR_DROPOUT=1)
add_executable(physim-sensor-tests tests/verify_sensor.c)
target_link_libraries(physim-sensor-tests PRIVATE physim_batch)
ps_warnings(physim-sensor-tests)
add_test(NAME sensor_reference COMMAND physim-sensor-tests $<TARGET_FILE:physim-runner>
  $<TARGET_FILE:uncertain_projectile> $<TARGET_FILE:sensor_ideal> $<TARGET_FILE:sensor_dropped>
  $<TARGET_FILE:physim-analysis-runner> $<TARGET_FILE:pendulum_analysis> ${CMAKE_BINARY_DIR})
set_tests_properties(sensor_reference PROPERTIES TIMEOUT 90)
add_library(batch_fixture MODULE tests/batch_fixture.c)
target_link_libraries(batch_fixture PRIVATE physim_core)
ps_warnings(batch_fixture)
add_executable(physim-batch-tests tests/test_batch.c)
target_link_libraries(physim-batch-tests PRIVATE physim_batch)
ps_warnings(physim-batch-tests)
add_test(NAME batch_reference COMMAND physim-batch-tests $<TARGET_FILE:physim-runner>
  $<TARGET_FILE:uncertain_projectile> ${CMAKE_BINARY_DIR} $<TARGET_FILE:fixture_1>
  $<TARGET_FILE:fixture_2> $<TARGET_FILE:batch_fixture>)
set_tests_properties(batch_reference PROPERTIES TIMEOUT 120)
foreach(mode RANGE 0 3)
  add_library(batch_parallel_${mode} MODULE tests/batch_parallel_fixture.c)
  target_link_libraries(batch_parallel_${mode} PRIVATE physim_platform)
  target_compile_definitions(batch_parallel_${mode} PRIVATE PARALLEL_FIXTURE_MODE=${mode})
  ps_warnings(batch_parallel_${mode})
endforeach()
add_executable(physim-batch-parallel-tests tests/test_batch_parallel.c)
target_link_libraries(physim-batch-parallel-tests PRIVATE physim_batch)
ps_warnings(physim-batch-parallel-tests)
add_test(NAME batch_parallel COMMAND physim-batch-parallel-tests $<TARGET_FILE:physim-runner>
  ${CMAKE_BINARY_DIR} $<TARGET_FILE:batch_parallel_0> $<TARGET_FILE:batch_parallel_1>
  $<TARGET_FILE:batch_parallel_2> $<TARGET_FILE:batch_parallel_3> $<TARGET_FILE:physim-batch>
  ${CMAKE_SOURCE_DIR}/tests/batch_parallel_fixture.c)
set_tests_properties(batch_parallel PROPERTIES TIMEOUT 60)
if(PHYSIM_BUILD_APP)
  add_executable(physim-library-tests tests/test_library.c app/library.c)
  target_include_directories(physim-library-tests PRIVATE app include)
  target_link_libraries(physim-library-tests PRIVATE SDL3::SDL3)
  ps_warnings(physim-library-tests)
  if(WIN32)
    add_custom_command(TARGET physim-library-tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:SDL3::SDL3> $<TARGET_FILE_DIR:physim-library-tests>)
  endif()
  add_test(NAME library COMMAND physim-library-tests)
  add_executable(physim-autosave-tests tests/test_autosave.c app/autosave.c)
  target_include_directories(physim-autosave-tests PRIVATE app include)
  target_link_libraries(physim-autosave-tests PRIVATE physim_core SDL3::SDL3)
  ps_warnings(physim-autosave-tests)
  add_test(NAME autosave COMMAND physim-autosave-tests)
  add_executable(physim-preferences-tests tests/test_preferences.c app/preferences.c)
  target_include_directories(physim-preferences-tests PRIVATE app include)
  target_link_libraries(physim-preferences-tests PRIVATE physim_core SDL3::SDL3)
  ps_warnings(physim-preferences-tests)
  add_test(NAME preferences COMMAND physim-preferences-tests ${CMAKE_BINARY_DIR})
endif()
if(PHYSIM_BUILD_APP AND PHYSIM_GRAPHICS_TESTS)
  foreach(mode language_full language_mixed language_projectile language_sensors language_body language_contact language_joint language_graph language_sweep language_spring language_buoyancy language_collision language_box_collision)
    add_test(NAME ${mode}_workflow COMMAND ${CMAKE_COMMAND}
      -DAPP=$<TARGET_FILE:physim> -DROOT=${CMAKE_BINARY_DIR}/language-ui -DMODE=${mode}
      -P ${CMAKE_SOURCE_DIR}/tests/language_workflow.cmake)
    set_tests_properties(${mode}_workflow PROPERTIES TIMEOUT 240 RUN_SERIAL TRUE
      LABELS "display;language" ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  endforeach()
  add_test(NAME floating_workflow COMMAND ${CMAKE_COMMAND} -DAPP=$<TARGET_FILE:physim> -DROOT=${CMAKE_BINARY_DIR}/floating-ui -P ${CMAKE_SOURCE_DIR}/tests/floating_workflow.cmake)
  set_tests_properties(floating_workflow PROPERTIES TIMEOUT 180 RUN_SERIAL TRUE LABELS display ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  add_test(NAME settings_workflow COMMAND ${CMAKE_COMMAND} -DAPP=$<TARGET_FILE:physim> -DROOT=${CMAKE_BINARY_DIR}/settings-ui -P ${CMAKE_SOURCE_DIR}/tests/settings_workflow.cmake)
  set_tests_properties(settings_workflow PROPERTIES TIMEOUT 90 RUN_SERIAL TRUE ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  add_test(NAME plot_workflow COMMAND ${CMAKE_COMMAND} -DAPP=$<TARGET_FILE:physim> -DROOT=${CMAKE_BINARY_DIR}/plot-ui -P ${CMAKE_SOURCE_DIR}/tests/plot_workflow.cmake)
  set_tests_properties(plot_workflow PROPERTIES TIMEOUT 45 RUN_SERIAL TRUE ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  add_test(NAME plot_input_isolation COMMAND ${CMAKE_COMMAND} -DAPP=$<TARGET_FILE:physim> -DROOT=${CMAKE_BINARY_DIR}/plot-noise -DNOISE=ON -P ${CMAKE_SOURCE_DIR}/tests/plot_workflow.cmake)
  set_tests_properties(plot_input_isolation PROPERTIES TIMEOUT 45 RUN_SERIAL TRUE LABELS display ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  add_test(NAME graphics COMMAND physim --renderer-test)
  add_test(NAME batch_workflow COMMAND ${CMAKE_COMMAND} -DAPP=$<TARGET_FILE:physim> -DROOT=${CMAKE_BINARY_DIR}/batch-ui -P ${CMAKE_SOURCE_DIR}/tests/batch_workflow.cmake)
  set_tests_properties(batch_workflow PROPERTIES TIMEOUT 180 RUN_SERIAL TRUE ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  add_test(NAME window_smoke COMMAND physim --smoke)
  add_test(NAME workspace_workflow COMMAND physim --workspace-test ${CMAKE_BINARY_DIR}/workspace-ui)
  set_tests_properties(workspace_workflow PROPERTIES TIMEOUT 30 RUN_SERIAL TRUE LABELS display
    ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  add_test(NAME language_editor_unicode_preview COMMAND physim --syntax-preview-test
    ${CMAKE_BINARY_DIR}/language-editor-unicode
    ${CMAKE_SOURCE_DIR}/tests/fixtures/language/unicode_identifiers.phys)
  set_tests_properties(language_editor_unicode_preview PROPERTIES TIMEOUT 30 RUN_SERIAL TRUE
    LABELS "display;language")
  add_test(NAME language_editor_multiline_preview COMMAND physim --syntax-preview-test
    ${CMAKE_BINARY_DIR}/language-editor-multiline
    ${CMAKE_SOURCE_DIR}/tests/fixtures/language/multiline_strings.phys)
  set_tests_properties(language_editor_multiline_preview PROPERTIES TIMEOUT 30 RUN_SERIAL TRUE
    LABELS "display;language")
  add_test(NAME documentation_window COMMAND physim --docs-test ${CMAKE_BINARY_DIR}/docs-ui-test)
  add_test(NAME autosave_workflow COMMAND ${CMAKE_COMMAND} -DAPP=$<TARGET_FILE:physim> -DROOT=${CMAKE_BINARY_DIR}/autosave-ui -P ${CMAKE_SOURCE_DIR}/tests/autosave_workflow.cmake)
  set_tests_properties(autosave_workflow PROPERTIES TIMEOUT 180 RUN_SERIAL TRUE ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  set_tests_properties(documentation_window PROPERTIES TIMEOUT 30 RUN_SERIAL TRUE ENVIRONMENT "PHYSIM_TEST_SMALL=1")
  set_tests_properties(graphics window_smoke PROPERTIES TIMEOUT 30 RUN_SERIAL TRUE)
  set_tests_properties(settings_workflow plot_workflow graphics batch_workflow window_smoke
    documentation_window autosave_workflow PROPERTIES LABELS display)
endif()
add_executable(physim-tests tests/test_core.c)
add_executable(physim-mechanics-tests tests/test_mechanics.c)
target_link_libraries(physim-mechanics-tests PRIVATE physim_core)
ps_warnings(physim-mechanics-tests)
add_test(NAME mechanics COMMAND physim-mechanics-tests)
add_executable(physim-buoyancy-tests tests/test_buoyancy.c)
target_link_libraries(physim-buoyancy-tests PRIVATE physim_core)
ps_warnings(physim-buoyancy-tests)
add_test(NAME buoyancy COMMAND physim-buoyancy-tests)
foreach(mode equilibrium neutral sink rise)
  add_library(buoyancy_${mode} MODULE examples/buoyancy/main.c)
  target_link_libraries(buoyancy_${mode} PRIVATE physim_core)
  ps_warnings(buoyancy_${mode})
  target_compile_definitions(buoyancy_${mode} PRIVATE PS_FLOAT_DAMPING=0)
endforeach()
target_compile_definitions(buoyancy_equilibrium PRIVATE PS_FLOAT_HEIGHT=0)
target_compile_definitions(buoyancy_neutral PRIVATE PS_FLOAT_DENSITY=1000 PS_FLOAT_HEIGHT=-0.5)
target_compile_definitions(buoyancy_sink PRIVATE PS_FLOAT_DENSITY=1500 PS_FLOAT_HEIGHT=-0.5)
target_compile_definitions(buoyancy_rise PRIVATE PS_FLOAT_HEIGHT=-0.5)
add_executable(physim-floating-tests tests/verify_floating.c)
target_link_libraries(physim-floating-tests PRIVATE physim_platform)
ps_warnings(physim-floating-tests)
add_test(NAME floating_reference COMMAND physim-floating-tests $<TARGET_FILE:physim-runner>
  $<TARGET_FILE:buoyancy> $<TARGET_FILE:buoyancy_equilibrium> $<TARGET_FILE:buoyancy_neutral>
  $<TARGET_FILE:buoyancy_sink> $<TARGET_FILE:buoyancy_rise>
  $<TARGET_FILE:physim-analysis-runner> $<TARGET_FILE:pendulum_analysis>)
set_tests_properties(floating_reference PROPERTIES TIMEOUT 90)
add_executable(physim-contact-tests tests/test_contacts.c)
target_link_libraries(physim-contact-tests PRIVATE physim_core)
ps_warnings(physim-contact-tests)
add_test(NAME contacts COMMAND physim-contact-tests)
foreach(mode undamped critical over)
  add_library(spring_${mode} MODULE examples/spring/main.c)
  target_link_libraries(spring_${mode} PRIVATE physim_core)
  ps_warnings(spring_${mode})
endforeach()
target_compile_definitions(spring_undamped PRIVATE PS_SPRING_DAMPING=0)
target_compile_definitions(spring_critical PRIVATE PS_SPRING_DAMPING=8)
target_compile_definitions(spring_over PRIVATE PS_SPRING_DAMPING=12)
add_executable(physim-spring-tests tests/verify_spring.c)
target_link_libraries(physim-spring-tests PRIVATE physim_platform)
ps_warnings(physim-spring-tests)
add_test(NAME spring_reference COMMAND physim-spring-tests $<TARGET_FILE:physim-runner> $<TARGET_FILE:spring> $<TARGET_FILE:spring_undamped> $<TARGET_FILE:spring_critical> $<TARGET_FILE:spring_over> $<TARGET_FILE:physim-analysis-runner> $<TARGET_FILE:pendulum_analysis>)
set_tests_properties(spring_reference PROPERTIES TIMEOUT 90)
foreach(variant air custom friction)
  add_library(collision_${variant} MODULE examples/collision/main.c)
  target_link_libraries(collision_${variant} PRIVATE physim_core)
  set_target_properties(collision_${variant} PROPERTIES PREFIX "")
  ps_warnings(collision_${variant})
endforeach()
target_compile_definitions(collision_air PRIVATE PS_COLLISION_MEDIUM=1)
target_compile_definitions(collision_custom PRIVATE PS_COLLISION_MEDIUM=3)
target_compile_definitions(collision_friction PRIVATE PS_COLLISION_FRICTION=0.3 PS_COLLISION_SPIN=1.0)
add_dependencies(physim-collision-language-parity-tests collision_air collision_custom collision_friction)
add_executable(physim-collision-tests tests/verify_collision.c)
target_link_libraries(physim-collision-tests PRIVATE physim_platform)
ps_warnings(physim-collision-tests)
add_test(NAME collision_reference COMMAND physim-collision-tests $<TARGET_FILE:physim-runner>
  $<TARGET_FILE:collision> $<TARGET_FILE:collision_air> $<TARGET_FILE:collision_custom> $<TARGET_FILE:collision_friction>)
set_tests_properties(collision_reference PROPERTIES TIMEOUT 30)
add_executable(physim-resample-tests tests/test_resample.c)
target_link_libraries(physim-resample-tests PRIVATE physim_core)
ps_warnings(physim-resample-tests)
add_test(NAME resample COMMAND physim-resample-tests)
target_link_libraries(physim-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-tests)
foreach(mode 1 2 3)
  add_library(fixture_${mode} MODULE tests/fixture.c)
  target_compile_definitions(fixture_${mode} PRIVATE FIXTURE_MODE=${mode})
  target_link_libraries(fixture_${mode} PRIVATE physim_core)
  ps_warnings(fixture_${mode})
endforeach()
add_executable(physim-integration-tests tests/test_integration.c)
target_link_libraries(physim-integration-tests PRIVATE physim_platform)
ps_warnings(physim-integration-tests)
add_test(NAME runner_isolation COMMAND physim-integration-tests $<TARGET_FILE:physim-runner> $<TARGET_FILE:pendulum> $<TARGET_FILE:fixture_1> $<TARGET_FILE:fixture_2> $<TARGET_FILE:fixture_3>)
set_tests_properties(runner_isolation PROPERTIES TIMEOUT 30)
add_test(NAME core COMMAND physim-tests)
foreach(mode 1 2 3 4)
  add_library(analysis_fixture_${mode} MODULE tests/analysis_fixture.c)
  target_compile_definitions(analysis_fixture_${mode} PRIVATE FIXTURE_MODE=${mode})
  target_link_libraries(analysis_fixture_${mode} PRIVATE physim_core)
  ps_warnings(analysis_fixture_${mode})
endforeach()
add_executable(physim-analysis-tests tests/test_analysis_runner.c)
target_link_libraries(physim-analysis-tests PRIVATE physim_platform)
ps_warnings(physim-analysis-tests)
add_test(NAME analysis_isolation COMMAND physim-analysis-tests $<TARGET_FILE:physim-analysis-runner> $<TARGET_FILE:analysis_fixture_1> $<TARGET_FILE:analysis_fixture_2> $<TARGET_FILE:analysis_fixture_3> $<TARGET_FILE:analysis_fixture_4>)
set_tests_properties(analysis_isolation PROPERTIES TIMEOUT 30)
add_executable(physim-comparison-tests tests/test_comparison.c)
target_link_libraries(physim-comparison-tests PRIVATE physim_platform)
ps_warnings(physim-comparison-tests)
add_test(NAME run_comparison COMMAND physim-comparison-tests $<TARGET_FILE:physim-analysis-runner> $<TARGET_FILE:pendulum_analysis>)
set_tests_properties(run_comparison PROPERTIES TIMEOUT 30)
add_executable(physim-numerics-tests tests/test_numerics.c)
target_link_libraries(physim-numerics-tests PRIVATE physim_core)
ps_warnings(physim-numerics-tests)
add_test(NAME numerics COMMAND physim-numerics-tests)
add_executable(physim-series-tests tests/test_series.c)
target_link_libraries(physim-series-tests PRIVATE physim_core)
ps_warnings(physim-series-tests)
add_test(NAME series COMMAND physim-series-tests)
add_executable(physim-select-tests tests/test_select.c)
target_link_libraries(physim-select-tests PRIVATE physim_core)
ps_warnings(physim-select-tests)
add_test(NAME series_select COMMAND physim-select-tests)
add_executable(physim-report-tests tests/test_report.c)
target_link_libraries(physim-report-tests PRIVATE physim_core)
ps_warnings(physim-report-tests)
add_test(NAME report COMMAND physim-report-tests)
add_executable(physim-documentation-tests tests/test_documentation.c app/documentation.c)
target_include_directories(physim-documentation-tests PRIVATE app include)
ps_warnings(physim-documentation-tests)
add_test(NAME documentation COMMAND physim-documentation-tests)
foreach(part main analysis)
  add_library(documentation_${part} MODULE examples/documentation/${part}.c)
  target_link_libraries(documentation_${part} PRIVATE physim_core)
  set_target_properties(documentation_${part} PROPERTIES PREFIX "")
  ps_warnings(documentation_${part})
endforeach()
foreach(part main analysis)
  add_library(documentation_vacuum_${part} MODULE examples/documentation/vacuum_${part}.c)
  target_link_libraries(documentation_vacuum_${part} PRIVATE physim_core)
  set_target_properties(documentation_vacuum_${part} PROPERTIES PREFIX "")
  ps_warnings(documentation_vacuum_${part})
endforeach()
add_executable(physim-vacuum-tutorial-tests tests/test_vacuum_tutorial.c)
target_link_libraries(physim-vacuum-tutorial-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-vacuum-tutorial-tests)
add_dependencies(physim-vacuum-tutorial-tests documentation_vacuum_main
  documentation_vacuum_analysis physim-language-projectile physim-language-vacuum-analysis)
add_test(NAME documentation_vacuum_tutorial COMMAND physim-vacuum-tutorial-tests
  $<TARGET_FILE:documentation_vacuum_main> $<TARGET_FILE:physim-language-projectile>
  $<TARGET_FILE:documentation_vacuum_analysis> $<TARGET_FILE:physim-language-vacuum-analysis>
  ${CMAKE_BINARY_DIR})
add_executable(physim-documentation-example-tests tests/test_documentation_example.c)
target_link_libraries(physim-documentation-example-tests PRIVATE physim_core physim_platform)
ps_warnings(physim-documentation-example-tests)
add_test(NAME documentation_example COMMAND physim-documentation-example-tests
  $<TARGET_FILE:documentation_main> $<TARGET_FILE:documentation_analysis> ${CMAKE_BINARY_DIR})
add_dependencies(physim-documentation-example-tests physim-language-documentation-main
  physim-language-documentation-analysis)
add_test(NAME documentation_language_example COMMAND physim-documentation-example-tests
  $<TARGET_FILE:physim-language-documentation-main>
  $<TARGET_FILE:physim-language-documentation-analysis> ${CMAKE_BINARY_DIR})
add_test(NAME documentation_language_source COMMAND ${CMAKE_COMMAND}
  -DROOT=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/tests/check_language_tutorial.cmake)
add_test(NAME documentation_language_analysis_source COMMAND ${CMAKE_COMMAND}
  -DROOT=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/tests/check_language_analysis_tutorial.cmake)
add_test(NAME documentation_vacuum_source COMMAND ${CMAKE_COMMAND}
  -DROOT=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/tests/check_vacuum_tutorial.cmake)
add_test(NAME documentation_projectile_drag_source COMMAND ${CMAKE_COMMAND}
  -DROOT=${CMAKE_SOURCE_DIR} -P ${CMAKE_SOURCE_DIR}/tests/check_projectile_drag_tutorial.cmake)
add_executable(physim-scene-tests tests/test_scene.c)
target_link_libraries(physim-scene-tests PRIVATE physim_platform)
ps_warnings(physim-scene-tests)
add_test(NAME scene_protocol COMMAND physim-scene-tests)
add_test(NAME prepare COMMAND ${CMAKE_COMMAND} -DOUTPUT=${CMAKE_BINARY_DIR} -P ${CMAKE_SOURCE_DIR}/tests/prepare.cmake)
set_tests_properties(prepare PROPERTIES FIXTURES_SETUP run_files)
add_test(NAME box_floor_run COMMAND physim-runner $<TARGET_FILE:box_floor> ${CMAKE_BINARY_DIR}/box-floor.psrun --steps 2000 --dt 0.005)
set_tests_properties(box_floor_run PROPERTIES FIXTURES_REQUIRED run_files FIXTURES_SETUP box_data)
add_executable(physim-box-tests tests/verify_box.c)
target_link_libraries(physim-box-tests PRIVATE physim_core)
ps_warnings(physim-box-tests)
add_test(NAME box_floor_reference COMMAND physim-box-tests ${CMAKE_BINARY_DIR}/box-floor.psrun)
set_tests_properties(box_floor_reference PROPERTIES FIXTURES_REQUIRED box_data)
add_test(NAME pendulum_run COMMAND physim-runner $<TARGET_FILE:pendulum> ${CMAKE_BINARY_DIR}/pendulum.psrun --steps 4000)
set_tests_properties(pendulum_run PROPERTIES FIXTURES_REQUIRED run_files FIXTURES_SETUP pendulum_data)
add_test(NAME pendulum_analysis COMMAND physim-analysis-runner $<TARGET_FILE:pendulum_analysis> ${CMAKE_BINARY_DIR}/pendulum.psrun ${CMAKE_BINARY_DIR}/report)
set_tests_properties(pendulum_analysis PROPERTIES FIXTURES_REQUIRED pendulum_data)
set_tests_properties(pendulum_analysis PROPERTIES FIXTURES_SETUP analysis_data)
add_test(NAME language_scalar_search_analysis COMMAND physim-analysis-runner
  $<TARGET_FILE:physim-language-scalar-search-analysis>
  ${CMAKE_BINARY_DIR}/pendulum.psrun ${CMAKE_BINARY_DIR}/scalar-search-report)
set_tests_properties(language_scalar_search_analysis PROPERTIES FIXTURES_REQUIRED pendulum_data)
add_executable(physim-derived-tests tests/verify_derived.c)
target_link_libraries(physim-derived-tests PRIVATE physim_core)
ps_warnings(physim-derived-tests)
add_test(NAME derived_reference COMMAND physim-derived-tests ${CMAKE_BINARY_DIR}/pendulum.psrun ${CMAKE_BINARY_DIR}/report-derived.csv ${CMAKE_BINARY_DIR}/report.psreport)
set_tests_properties(derived_reference PROPERTIES FIXTURES_REQUIRED analysis_data)
add_executable(physim-reference-tests tests/verify_pendulum.c)
target_link_libraries(physim-reference-tests PRIVATE physim_core)
ps_warnings(physim-reference-tests)
add_test(NAME pendulum_reference COMMAND physim-reference-tests ${CMAKE_BINARY_DIR}/pendulum.psrun)
set_tests_properties(pendulum_reference PROPERTIES FIXTURES_REQUIRED pendulum_data)
foreach(method rk45 verlet)
  string(TOUPPER ${method} METHOD)
  add_library(pendulum_${method} MODULE examples/pendulum/main.c)
  target_compile_definitions(pendulum_${method} PRIVATE PS_PENDULUM_METHOD=PS_${METHOD})
  target_link_libraries(pendulum_${method} PRIVATE physim_core)
  ps_warnings(pendulum_${method})
  add_test(NAME pendulum_${method}_run COMMAND physim-runner $<TARGET_FILE:pendulum_${method}> ${CMAKE_BINARY_DIR}/pendulum-${method}.psrun --steps 4000)
  set_tests_properties(pendulum_${method}_run PROPERTIES FIXTURES_REQUIRED run_files FIXTURES_SETUP ${method}_data)
  add_test(NAME pendulum_${method}_reference COMMAND physim-reference-tests ${CMAKE_BINARY_DIR}/pendulum-${method}.psrun ${method})
  set_tests_properties(pendulum_${method}_reference PROPERTIES FIXTURES_REQUIRED ${method}_data)
endforeach()
install(TARGETS physim_core physim-runner physim-analysis-runner physim-batch pendulum projectile collision box_floor spring uncertain_projectile box_collision buoyancy pendulum_analysis RUNTIME DESTINATION bin LIBRARY DESTINATION bin ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY examples/ DESTINATION examples)
install(FILES src/core.c src/memory.c src/array.c src/string_view.c src/hashmap.c src/math.c src/data.c src/analysis.c src/scene.c src/numerics.c src/units.c src/series.c src/report.c src/report_export.c src/mechanics.c src/box_contacts.c src/collision.c src/measurement.c src/report_internal.h DESTINATION src)
install(FILES cmake/PhysimExperiment.cmake cmake/PhysimLanguage.cmake cmake/PhysimEmitC.cmake DESTINATION cmake)
install(DIRECTORY docs/ DESTINATION docs)
install(FILES third_party/Nuklear-LICENSE DESTINATION licenses)
install(FILES third_party/zlib-1.3.2/LICENSE DESTINATION licenses RENAME zlib-LICENSE.txt)
install(FILES third_party/README.md DESTINATION licenses RENAME Dependencies.md)
if(WIN32 AND EXISTS "${CMAKE_SOURCE_DIR}/third_party/SDL3-3.2.30/LICENSE.txt")
  install(FILES third_party/SDL3-3.2.30/LICENSE.txt DESTINATION licenses RENAME SDL3-LICENSE.txt)
endif()
if(MSVC)
  set(CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin)
  include(InstallRequiredSystemLibraries)
endif()
install(FILES LICENSE README.md Physim_Projektplan.md DESTINATION .)
set(CPACK_PACKAGE_NAME Physim)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_FILE_NAME "Physim-${PROJECT_VERSION}-dev-${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR}")
set(CPACK_GENERATOR ZIP)
include(CPack)
