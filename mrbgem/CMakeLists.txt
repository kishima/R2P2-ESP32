cmake_minimum_required(VERSION 3.16)

# --- 1) 変数定義（パスは現状の構成に合わせてそのまま） ---
set(PICORUBY_DIR ${COMPONENT_DIR}/picoruby)
set(PRB_BUILD_DIR ${PICORUBY_DIR}/build/esp32)   # CrossBuild名が"esp32"
set(PRB_LIB      ${PRB_BUILD_DIR}/lib/libmruby.a)
set(GEM_INIT_C   ${PRB_BUILD_DIR}/mrbgems/gem_init.c)

# --- 2) 有効化されたgemだけのラッパ(src)を自動抽出 ---
set(ENABLED_WRAP_SRCS "")
if(EXISTS ${GEM_INIT_C})
  file(READ ${GEM_INIT_C} _GEMTXT)
  # GENERATED_TMP_mrb_picoruby_foo_gem_init / GENERATED_TMP_mrb_mruby_foo_gem_init の両系統を拾う
  string(REGEX MATCHALL "GENERATED_TMP_mrb_[A-Za-z0-9_]+_gem_init" _MATCHES "${_GEMTXT}")
  foreach(M ${_MATCHES})
    string(REGEX REPLACE "^GENERATED_TMP_mrb_" "" GEM "${M}")
    string(REGEX REPLACE "_gem_init$" "" GEM "${GEM}")
    # ディレクトリ名は "picoruby-foo" 系なので "_"→"-" に寄せる
    string(REPLACE "_" "-" GEM_DIR "${GEM}")

    # ★ ラッパは各gem配下の ports/esp32/ をビルド対象にする（既存構成を踏襲）
    file(GLOB _ONE_WRAP
      ${COMPONENT_DIR}/picoruby/mrbgems/${GEM_DIR}/ports/esp32/*.c
      ${COMPONENT_DIR}/picoruby/mrbgems/${GEM_DIR}/ports/common/*.c
    )
    list(APPEND ENABLED_WRAP_SRCS ${_ONE_WRAP})
  endforeach()
else()
  message(STATUS "gem_init.c not found yet: ${GEM_INIT_C} (first build will generate)")
endif()

# --- 3) include の自動検出 ---
# 小ヘルパー: ディレクトリが存在する時だけ追加
function(_append_if_dir lst dirpath)
  if(IS_DIRECTORY "${dirpath}")
    list(APPEND ${lst} "${dirpath}")
    set(${lst} "${${lst}}" PARENT_SCOPE)
  endif()
endfunction()

set(AUTO_INC_DIRS "")
# gem_init.c から得た GEM_DIR をもう一度走査して include 候補を拾う
foreach(M ${_MATCHES})
  string(REGEX REPLACE "^GENERATED_TMP_mrb_" "" _G "${M}")
  string(REGEX REPLACE "_gem_init$" "" _G "${_G}")
  string(REPLACE "_" "-" GEM_DIR "${_G}")

  # 典型的なヘッダ配置（必要十分）
  _append_if_dir(AUTO_INC_DIRS ${COMPONENT_DIR}/picoruby/mrbgems/${GEM_DIR}/include)
  _append_if_dir(AUTO_INC_DIRS ${COMPONENT_DIR}/picoruby/mrbgems/${GEM_DIR}/ports/esp32)
  _append_if_dir(AUTO_INC_DIRS ${COMPONENT_DIR}/picoruby/mrbgems/${GEM_DIR}/ports/common)
  # Rake生成物のヘッダ（gem固有生成物がここに出る場合がある）
  _append_if_dir(AUTO_INC_DIRS ${PRB_BUILD_DIR}/mrbgems/${GEM_DIR})
endforeach()

# Rake生成の共通インクルードルート（全gemの生成ヘッダを見せる）
_append_if_dir(AUTO_INC_DIRS ${PRB_BUILD_DIR}/mrbgems)

# 既存で固定的に必要な include は明示（picoruby本体・prism など）
# ※既に書いてあるものは残す。自動と固定を合算して重複排除
set(FIXED_INC_DIRS
  ${COMPONENT_DIR}
  ${COMPONENT_DIR}/picoruby/include
  ${COMPONENT_DIR}/picoruby/mrbgems/picoruby-mrubyc/lib/mrubyc/src
  ${COMPONENT_DIR}/picoruby/mrbgems/picoruby-mrubyc/lib/mrubyc/hal/esp32
  ${COMPONENT_DIR}/picoruby/mrbgems/mruby-compiler2/include
  ${COMPONENT_DIR}/picoruby/mrbgems/mruby-compiler2/lib/prism/include
)

# 重複削除
list(APPEND FIXED_INC_DIRS ${AUTO_INC_DIRS})
list(REMOVE_DUPLICATES FIXED_INC_DIRS)

# --- 4) idf_component_register の INCLUDE_DIRS を差し替え ---
idf_component_register(
  SRCS
    ${COMPONENT_DIR}/picoruby-esp32.c
    ${ENABLED_WRAP_SRCS}
  INCLUDE_DIRS
    ${FIXED_INC_DIRS}
  PRIV_REQUIRES
    esp_driver_gpio esp_driver_uart esp_driver_spi esp_driver_ledc
    esp_driver_rmt  esp_driver_i2c  esp_adc        esp_timer
    esp_hw_support  spi_flash       mbedtls
)

# --- 5) 先にRakeを回してpicorubyをビルド（gem_init.cを確定させる） ---
add_custom_command(
  OUTPUT ${PRB_LIB}
  COMMAND ${CMAKE_COMMAND} -E echo "MRUBY_CONFIG=${IDF_TARGET_ARCH}-esp rake"
  COMMAND ${CMAKE_COMMAND} -E env MRUBY_CONFIG=${IDF_TARGET_ARCH}-esp rake
  WORKING_DIRECTORY ${PICORUBY_DIR}
  COMMENT "PicoRuby build"
  VERBATIM
)
add_custom_target(picoruby ALL DEPENDS ${PRB_LIB})

# --- 6) prebuiltライブラリを登録 ---
add_prebuilt_library(
  libmruby ${PRB_LIB}
  REQUIRES ${COMPONENT_NAME}
)
target_link_libraries(${COMPONENT_LIB} PRIVATE libmruby)
target_include_directories(
  ${COMPONENT_LIB}
  PRIVATE
    ${COMPONENT_DIR}/picoruby/mrbgems/mruby-compiler2/include
    ${COMPONENT_DIR}/picoruby/mrbgems/mruby-compiler2/lib/prism/include
    ${COMPONENT_DIR}/picoruby/build/esp32/mrbgems
)

# --- 7) IDFの定義は従来どおり ---
add_definitions(
  -DMRBC_TICK_UNIT=10
  -DMRBC_TIMESLICE_TICK_COUNT=1
  -DMRBC_USE_FLOAT=2
  -DMRC_CUSTOM_ALLOC
  -DMRBC_CONVERT_CRLF=1
  -DUSE_FAT_FLASH_DISK
  -DNDEBUG
  -DPICORB_VM_MRUBYC
)

# --- 8) 依存（ビルド順） ---
add_dependencies(${COMPONENT_LIB} picoruby)

# --- 9) Ruby→Cの生成は既存のまま（必要なら残す） ---
set(RUBY_FILES main_task)
set(PICORBC ${COMPONENT_DIR}/picoruby/bin/picorbc)
set(GENERATED_C_FILES "")
foreach(rb ${RUBY_FILES})
  set(C_FILE ${COMPONENT_DIR}/mrb/${rb}.c)
  set(RB_FILE ${COMPONENT_DIR}/mrblib/${rb}.rb)
  add_custom_command(
    OUTPUT ${C_FILE}
    COMMAND ${CMAKE_COMMAND} -E echo "Compiling ${RB_FILE}..."
    COMMAND ${CMAKE_COMMAND} -E make_directory ${COMPONENT_DIR}/mrb
    COMMAND ${PICORBC} -B${rb} -o${C_FILE} ${RB_FILE}
    DEPENDS picoruby ${RB_FILE}
    COMMENT "Compiling ${RB_FILE}"
    VERBATIM
  )
  list(APPEND GENERATED_C_FILES ${C_FILE})
endforeach()
target_sources(${COMPONENT_LIB} PRIVATE ${GENERATED_C_FILES})
