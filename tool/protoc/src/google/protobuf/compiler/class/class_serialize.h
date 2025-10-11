#ifndef __CLASS_SERIALIZE_H__
#define __CLASS_SERIALIZE_H__

#include <google/protobuf/compiler/cpp/cpp_helpers.h>
#include <vector>

namespace google {
namespace protobuf {
class FileDescriptor;
namespace io {
class Printer;
}// namespace io

namespace compiler {
namespace cpp {

struct Options;

class CodeSerialize {
    struct FieldDescriptorArr {
        std::vector<const FieldDescriptor*> _vec;
        std::string _name;
    };
    std::vector<FieldDescriptorArr> _message_generators;
    SCCAnalyzer scc_analyzer_;
    const FileDescriptor* _file;

public:
    CodeSerialize(const FileDescriptor* file, const Options& options);
    ~CodeSerialize();

    void printHeader(google::protobuf::io::Printer& printer, const char* szName) const;
    static std::string FieldName(const FieldDescriptor& field);

private:
    void prepareMsgs();
    static bool sortMsgs(std::vector<FieldDescriptorArr>& msgs);
    static uint32_t getInsertIdx(const std::vector<FieldDescriptorArr>& msgs,
                                 const FieldDescriptorArr& item);
    void printInclude(google::protobuf::io::Printer& printer) const;
    void printEnum(google::protobuf::io::Printer& printer) const;
    void printClass(google::protobuf::io::Printer& printer, FileDescriptor::Syntax syntax) const;
    void printDeclare(google::protobuf::io::Printer& printer,
                      const FieldDescriptorArr& messages) const;
    void printConstruction(google::protobuf::io::Printer& printer,
                           const FieldDescriptorArr& messages) const;
    void printGetSetHas(google::protobuf::io::Printer& printer,
                        const FieldDescriptorArr& messages) const;
    void printInitField(google::protobuf::io::Printer& printer, const FieldDescriptor& field) const;
    void printInitFields(google::protobuf::io::Printer& printer,
                         const FieldDescriptorArr& messages) const;
    void printSerialize(google::protobuf::io::Printer& printer,
                        const FieldDescriptorArr& messages) const;
    bool hasInt(google::protobuf::io::Printer& printer) const;
    bool hasString(google::protobuf::io::Printer& printer) const;
    bool hasVector(google::protobuf::io::Printer& printer) const;
    bool hasMap(google::protobuf::io::Printer& printer) const;
};

}// namespace cpp
}// namespace compiler
}// namespace protobuf
}// namespace google

#endif
