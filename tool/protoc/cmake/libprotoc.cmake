set(libprotoc_files
  ${protobuf_source_dir}/src/google/protobuf/compiler/code_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/command_line_interface.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_enum.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_enum_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_extension.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_file.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_helpers.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_map_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_message.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_message_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_padding_optimizer.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_primitive_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_service.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_string_field.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/plugin.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/plugin.pb.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/subprocess.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/zip_writer.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/class/class_generator.cc
  ${protobuf_source_dir}/src/google/protobuf/compiler/class/class_serialize.cc
)

set(libprotoc_headers
  ${protobuf_source_dir}/src/google/protobuf/compiler/code_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/command_line_interface.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/importer.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/mock_code_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/package_info.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/parser.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/plugin.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/plugin.pb.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/subprocess.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/zip_writer.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_enum.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_enum_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_extension.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_file.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_helpers.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_map_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_message.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_message_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_options.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_primitive_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_service.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/cpp/cpp_string_field.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/plugin.pb.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/class/class_generator.h
  ${protobuf_source_dir}/src/google/protobuf/compiler/class/class_serialize.h
)

add_library(libprotoc ${protobuf_SHARED_OR_STATIC}
  ${libprotoc_files} ${libprotoc_headers})
target_link_libraries(libprotoc libprotobuf)
if(MSVC AND protobuf_BUILD_SHARED_LIBS)
  target_compile_definitions(libprotoc
    PUBLIC  PROTOBUF_USE_DLLS
    PRIVATE LIBPROTOC_EXPORTS)
endif()
set_target_properties(libprotoc PROPERTIES
    COMPILE_DEFINITIONS LIBPROTOC_EXPORTS
    OUTPUT_NAME ${LIB_PREFIX}protoc
    DEBUG_POSTFIX "${protobuf_DEBUG_POSTFIX}")
