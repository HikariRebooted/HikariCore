if (NOT HIKARI_BUILD_IN_TREE)
project(LLVMObfuscation)
cmake_minimum_required(VERSION 3.4.3)
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
add_library(LLVMObfuscationCommon STATIC
  src/Passes/FunctionCallObfuscate.cpp
  src/CryptoUtils.cpp
  src/Passes/BogusControlFlow.cpp
  src/Passes/Substitution.cpp
  src/Passes/Flattening.cpp
  src/Utils.cpp
  src/Passes/SplitBasicBlocks.cpp
  src/Passes/AntiClassDump.cpp
  src/Passes/StringEncryption.cpp
  src/Passes/IndirectBranch.cpp
  src/Passes/FunctionWrapper.cpp
  src/PassScheduler.cpp
  src/LegacyLowerSwitch.cpp
)
target_include_directories(LLVMObfuscationCommon PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  OUTPUT_VARIABLE HIKARI_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(LLVMObfuscationCommon PRIVATE "-DGIT_COMMIT_HASH=\"${HIKARI_GIT_COMMIT_HASH}\"")

add_library(LLVMObfuscation SHARED
  ${CMAKE_CURRENT_LIST_DIR}/src/AddPasses/Register_Passes.cpp
)
target_link_libraries(LLVMObfuscation LLVMObfuscationCommon)
target_include_directories(LLVMObfuscation PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)

else()
add_llvm_library(LLVMObfuscationCommon
  src/Passes/FunctionCallObfuscate.cpp
  src/CryptoUtils.cpp
  src/Passes/BogusControlFlow.cpp
  src/Passes/Substitution.cpp
  src/Passes/Flattening.cpp
  src/Utils.cpp
  src/Passes/SplitBasicBlocks.cpp
  src/Passes/AntiClassDump.cpp
  src/Passes/StringEncryption.cpp
  src/Passes/IndirectBranch.cpp
  src/Passes/FunctionWrapper.cpp
  src/PassScheduler.cpp
  src/LegacyLowerSwitch.cpp
  ${CMAKE_CURRENT_LIST_DIR}/src/AddPasses/Register_Passes.cpp
  DEPENDS
  intrinsics_gen
  )
target_include_directories(LLVMObfuscationCommon PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
execute_process(
  COMMAND git log -1 --format=%H
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  OUTPUT_VARIABLE HIKARI_GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
target_compile_definitions(LLVMObfuscationCommon PRIVATE "-DGIT_COMMIT_HASH=\"${HIKARI_GIT_COMMIT_HASH}\"")
endif()

if(APPLE)
  SET(CMAKE_BUILD_WITH_INSTALL_NAME_DIR TRUE)
  set(CMAKE_CXX_FLAGS "-std=c++14")
  execute_process(
    COMMAND cp ${CMAKE_CURRENT_LIST_DIR}/substitute/libsubstitute.dylib .
  )

  # LLVMObfuscationDeps handling
  add_library(LLVMObfuscationDeps SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/Dummy.cpp
  )
  target_link_options(LLVMObfuscationDeps PRIVATE -undefined PRIVATE dynamic_lookup PRIVATE -all_load)
  target_link_libraries(LLVMObfuscationDeps PRIVATE LLVMCore LLVMSupport)
  # LLVMObfuscation
  target_link_options(LLVMObfuscation PRIVATE -undefined PRIVATE dynamic_lookup PRIVATE -flat_namespace)
  
  # LLVMObfuscation hooking mode
  add_library(LLVMObfuscationHook SHARED
    ${CMAKE_CURRENT_LIST_DIR}/src/AddPasses/Loader_Hook.cpp
  )
  target_link_options(LLVMObfuscationHook PRIVATE -undefined PRIVATE dynamic_lookup PRIVATE -flat_namespace)
  target_include_directories(LLVMObfuscationHook PRIVATE ${CMAKE_CURRENT_LIST_DIR}/substitute)
  target_include_directories(LLVMObfuscationHook PRIVATE ${CMAKE_CURRENT_LIST_DIR}/include)
  target_link_directories(LLVMObfuscationHook PRIVATE ${CMAKE_CURRENT_LIST_DIR}/substitute)
  target_link_libraries(LLVMObfuscationHook LLVMObfuscationDeps LLVMObfuscationCommon substitute)
  add_custom_target(install_name_tool ALL
    COMMAND install_name_tool -change @executable_path/libsubstitute.dylib @rpath/libsubstitute.dylib ./libLLVMObfuscationHook.dylib
	DEPENDS LLVMObfuscationHook
  )  
else()
endif(APPLE)
