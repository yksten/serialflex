#ifndef GOOGLE_PROTOBUF_COMPILER_CLASS_GENERATOR_H__
#define GOOGLE_PROTOBUF_COMPILER_CLASS_GENERATOR_H__

#include <google/protobuf/compiler/code_generator.h>
#include <string>

namespace google {
namespace protobuf {
namespace compiler {
namespace cpp {

class LIBPROTOC_EXPORT ClassGenerator : public CodeGenerator {
public:
    ClassGenerator();
    ~ClassGenerator();

    // implements CodeGenerator ----------------------------------------
    bool Generate(const FileDescriptor* file, const string& parameter,
                  GeneratorContext* generator_context, string* error) const;

private:
    GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(ClassGenerator);
};

}// namespace cpp
}// namespace compiler
}// namespace protobuf

}// namespace google
#endif
