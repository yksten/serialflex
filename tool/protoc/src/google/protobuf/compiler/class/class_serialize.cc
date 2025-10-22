#include "class_serialize.h"

#include <google/protobuf/compiler/cpp/cpp_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace cpp {

const char* const kKeywordList[] = {
    "alignas",      "alignof",  "and",       "and_eq",   "asm",       "auto",
    "bitand",       "bitor",    "bool",      "break",    "case",      "catch",
    "char",         "class",    "compl",     "const",    "constexpr", "const_cast",
    "continue",     "decltype", "default",   "delete",   "do",        "double",
    "dynamic_cast", "else",     "enum",      "explicit", "export",    "extern",
    "false",        "float",    "for",       "friend",   "goto",      "if",
    "inline",       "int",      "long",      "mutable",  "namespace", "new",
    "noexcept",     "not",      "not_eq",    "NULL",     "operator",  "or",
    "or_eq",        "private",  "protected", "public",   "register",  "reinterpret_cast",
    "return",       "short",    "signed",    "sizeof",   "static",    "static_assert",
    "static_cast",  "struct",   "switch",    "template", "this",      "thread_local",
    "throw",        "true",     "try",       "typedef",  "typeid",    "typename",
    "union",        "unsigned", "using",     "virtual",  "void",      "volatile",
    "wchar_t",      "while",    "xor",       "xor_eq"};

hash_set<string> MakeKeywordsMap() {
    hash_set<string> result;
    for (int i = 0; i < GOOGLE_ARRAYSIZE(kKeywordList); i++) {
        result.insert(kKeywordList[i]);
    }
    return result;
}

hash_set<string> kKeywords = MakeKeywordsMap();

std::string type2string(const FieldDescriptor& field) {
    switch (field.type()) {
        case FieldDescriptor::TYPE_DOUBLE:
            return "double";
        case FieldDescriptor::TYPE_FLOAT:
            return "float";
        case FieldDescriptor::TYPE_INT64:
        case FieldDescriptor::TYPE_SFIXED64:
        case FieldDescriptor::TYPE_SINT64:
            return "int64_t";
        case FieldDescriptor::TYPE_UINT64:
        case FieldDescriptor::TYPE_FIXED64:
            return "uint64_t";
        case FieldDescriptor::TYPE_INT32:
        case FieldDescriptor::TYPE_SFIXED32:
        case FieldDescriptor::TYPE_SINT32:
            return "int32_t";
        case FieldDescriptor::TYPE_FIXED32:
        case FieldDescriptor::TYPE_UINT32:
            return "uint32_t";
        case FieldDescriptor::TYPE_ENUM: {
            std::string result;
            result = ClassName(field.enum_type(), true);
            return result;
        }
        case FieldDescriptor::TYPE_BOOL:
            return "bool";
        case FieldDescriptor::TYPE_STRING:
        case FieldDescriptor::TYPE_BYTES:
            return "std::string";
        default: {
            std::string result;
            result = ClassName(field.message_type(), true);
            return result;
        }
    }
}

std::string map2string(const FieldDescriptor& field) {
    std::string result;
    result.clear();
    assert(field.message_type()->field_count() == 2);
    const FieldDescriptor* key = field.message_type()->field(0);
    const FieldDescriptor* value = field.message_type()->field(1);
    result.append(type2string(*key)).append(", ").append(type2string(*value));
    return result;
}

std::string type2value(const FieldDescriptor& field) {
    char sz[64] = {0};
    switch (field.type()) {
        case FieldDescriptor::TYPE_DOUBLE:
            snprintf(sz, 64, "%lf", field.default_value_double());
            break;
        case FieldDescriptor::TYPE_FLOAT:
            snprintf(sz, 64, "%f", field.default_value_float());
            break;
        case FieldDescriptor::TYPE_INT64:
        case FieldDescriptor::TYPE_FIXED64:
        case FieldDescriptor::TYPE_SFIXED64:
        case FieldDescriptor::TYPE_SINT64:
            snprintf(sz, 64, "%lld", field.default_value_int64());
            break;
        case FieldDescriptor::TYPE_UINT64:
            snprintf(sz, 64, "%llu", field.default_value_uint64());
            break;
        case FieldDescriptor::TYPE_INT32:
        case FieldDescriptor::TYPE_FIXED32:
        case FieldDescriptor::TYPE_SFIXED32:
        case FieldDescriptor::TYPE_SINT32:
            snprintf(sz, 64, "%d", field.default_value_int32());
            break;
        case FieldDescriptor::TYPE_UINT32:
            snprintf(sz, 64, "%u", field.default_value_uint32());
            break;
        case FieldDescriptor::TYPE_ENUM:
            snprintf(sz, 64, "%d", field.default_value_enum()->number());
            break;
        case FieldDescriptor::TYPE_BOOL:
            if (field.default_value_bool()) {
                return "true";
            }
            return "false";
        case FieldDescriptor::TYPE_STRING:
        case FieldDescriptor::TYPE_BYTES:
        default:
            assert(false);
    }
    return std::string(sz);
}

const char* getFieldType(const FieldDescriptor& field) {
    switch (field.type()) {
        case FieldDescriptor::TYPE_DOUBLE:
            return "serialflex::protobuf::FIELDTYPE_DOUBLE";
        case FieldDescriptor::TYPE_FLOAT:
            return "serialflex::protobuf::FIELDTYPE_FLOAT";
        case FieldDescriptor::TYPE_INT64:
            return "serialflex::protobuf::FIELDTYPE_INT64";
        case FieldDescriptor::TYPE_UINT64:
            return "serialflex::protobuf::FIELDTYPE_UINT64";
        case FieldDescriptor::TYPE_INT32:
            return "serialflex::protobuf::FIELDTYPE_INT32";
        case FieldDescriptor::TYPE_FIXED64:
            return "serialflex::protobuf::FIELDTYPE_FIXED64";
        case FieldDescriptor::TYPE_FIXED32:
            return "serialflex::protobuf::FIELDTYPE_FIXED32";
        case FieldDescriptor::TYPE_BOOL:
            return "serialflex::protobuf::FIELDTYPE_BOOL";
        case FieldDescriptor::TYPE_STRING:
            return "serialflex::protobuf::FIELDTYPE_STRING";
        case FieldDescriptor::TYPE_GROUP:
            return "serialflex::protobuf::FIELDTYPE_GROUP";
        case FieldDescriptor::TYPE_MESSAGE:
            return "serialflex::protobuf::FIELDTYPE_MESSAGE";
        case FieldDescriptor::TYPE_BYTES:
            return "serialflex::protobuf::FIELDTYPE_BYTES";
        case FieldDescriptor::TYPE_UINT32:
            return "serialflex::protobuf::FIELDTYPE_UINT32";
        case FieldDescriptor::TYPE_ENUM:
            return "serialflex::protobuf::FIELDTYPE_ENUM";
        case FieldDescriptor::TYPE_SFIXED32:
            return "serialflex::protobuf::FIELDTYPE_SFIXED32";
        case FieldDescriptor::TYPE_SFIXED64:
            return "serialflex::protobuf::FIELDTYPE_SFIXED64";
        case FieldDescriptor::TYPE_SINT32:
            return "serialflex::protobuf::FIELDTYPE_SINT32";
        case FieldDescriptor::TYPE_SINT64:
            return "serialflex::protobuf::FIELDTYPE_SINT64";
        default:
            assert(false);
            return NULL;
    }
}

/*--------------------------------------------------------------------------------*/
class PackagePartsWrapper {
    const std::vector<string>& _package_parts;
    google::protobuf::io::Printer& _printer;

public:
    PackagePartsWrapper(const std::vector<string>& package_parts,
                        google::protobuf::io::Printer& printer)
        : _package_parts(package_parts), _printer(printer) {
        uint32_t nSize = (uint32_t)_package_parts.size();
        for (uint32_t idx = 0; idx < nSize; ++idx) {
            _printer.Print("namespace $name$ {\n", "name", _package_parts.at(idx));
        }
    }
    ~PackagePartsWrapper() {
        _printer.Print("\n");
        for (std::vector<string>::const_reverse_iterator it = _package_parts.rbegin();
             it != _package_parts.rend(); ++it) {
            _printer.Print("} // namespace $name$\n", "name", *it);
        }
    }
};
/*--------------------------------------------------------------------------------*/
CodeSerialize::CodeSerialize(const FileDescriptor* file, const Options& options)
    : scc_analyzer_(options), _file(file) {
    prepareMsgs();
}

CodeSerialize::~CodeSerialize() {}

void CodeSerialize::prepareMsgs() {
    std::vector<const Descriptor*> msgs = FlattenMessagesInFile(_file);
    for (int i = 0; i < msgs.size(); ++i) {
        const Descriptor* descriptor = msgs[i];
        FieldDescriptorArr optimized_order;
        optimized_order._name = ClassName(msgs[i]);
        for (int idx = 0; idx < descriptor->field_count(); ++idx) {
            const FieldDescriptor* field = descriptor->field(idx);
            if (field->options().weak()) {
                ;
            } else if (!field->containing_oneof()) {
                optimized_order._vec.push_back(field);
            }
        }
        _message_generators.push_back(optimized_order);
    }
    // sort
    sortMsgs(_message_generators);
}

bool CodeSerialize::sortMsgs(std::vector<FieldDescriptorArr>& msgs) {
    std::vector<FieldDescriptorArr> result;
    uint32_t nSize = (uint32_t)msgs.size();
    for (uint32_t idx = 0; idx < nSize; ++idx) {
        const FieldDescriptorArr& item = msgs.at(idx);
        if (result.empty()) {
            result.insert(result.begin(), item);
        } else {
            result.insert(result.begin() + getInsertIdx(result, item), item);
        }
    }
    msgs.swap(result);
    return true;
}

uint32_t CodeSerialize::getInsertIdx(const std::vector<FieldDescriptorArr>& msgs,
                                     const FieldDescriptorArr& item) {
    uint32_t idx = 0;
    for (; idx < msgs.size(); ++idx) {
        const FieldDescriptorArr& curItem = msgs.at(idx);
        uint32_t curItemIdx = 0;
        for (; curItemIdx < curItem._vec.size(); ++curItemIdx) {
            const FieldDescriptor* field = curItem._vec.at(curItemIdx);
            if (field->message_type() && field->message_type()->name() == item._name) {
                return idx;
            }
        }
    }
    return idx;
}

void CodeSerialize::printHeader(google::protobuf::io::Printer& printer, const char* szName) const {
    std::string strStructName(szName);
    std::transform(strStructName.begin(), strStructName.end(), strStructName.begin(), ::toupper);
    // 1.program once
    printer.Print(
        "#ifndef __CLASS_$basename$_INCLUDE__H_\n#define __CLASS_$basename$_INCLUDE__H_\n",
        "basename", strStructName);
    // 2.include
    printInclude(printer);
    do {
        // 3.namespace
        std::vector<string> package_parts = Split(_file->package(), ".", true);
        PackagePartsWrapper ins(package_parts, printer);
        printEnum(printer);
        // class
        // constructed function
        // Initialization fidlds
        // fields
        printClass(printer, _file->syntax());
    } while (false);
    // 4.serialize functions
    // printSerialize(printer);
    printer.Print("\n#endif\n");
}

void CodeSerialize::printInclude(google::protobuf::io::Printer& printer) const {
    printer.Print("\n");
    // printer.Print("#include \"serialize.h\"\n");
    if (hasInt(printer)) {
        printer.Print("#include <stdint.h>\n");
    }
    if (hasString(printer)) {
        printer.Print("#include <string>\n");
    }
    if (hasVector(printer)) {
        printer.Print("#include <vector>\n");
    }
    if (hasMap(printer)) {
        printer.Print("#include <map>\n");
    }
    // import
    std::set<string> public_import_names;
    for (int i = 0; i < _file->public_dependency_count(); i++) {
        public_import_names.insert(_file->public_dependency(i)->name());
    }

    for (int i = 0; i < _file->dependency_count(); i++) {
        const bool use_system_include = IsWellKnownMessage(_file->dependency(i));
        const string& name = _file->dependency(i)->name();
        bool public_import = (public_import_names.count(name) != 0);
        printer.Print("#include $left$$dependency$.pb.h$right$$iwyu$\n", "dependency",
                      StripProto(name), "iwyu", (public_import) ? "  // IWYU pragma: export" : "",
                      "left", use_system_include ? "<" : "\"", "right",
                      use_system_include ? ">" : "\"");
    }

    printer.Print("\n");
}

void CodeSerialize::printEnum(google::protobuf::io::Printer& printer) const {
    for (int i = 0; i < _file->enum_type_count(); ++i) {
        const EnumDescriptor* item = _file->enum_type(i);
        printer.Print("\n");
        printer.Print("enum $enumName$ \{\n", "enumName", item->name());
        for (int idx = 0; idx < item->value_count(); ++idx) {
            const EnumValueDescriptor* value = item->value(idx);
            char sz[64] = {0};
            snprintf(sz, 64, "%d", value->number());
            printer.Print("    $valueName$ = $value$,\n", "valueName", value->name(), "value", sz);
        }
        printer.Print("};\n");
    }
}

void CodeSerialize::printClass(google::protobuf::io::Printer& printer,
                               FileDescriptor::Syntax syntax) const {
    uint32_t size = (uint32_t)_message_generators.size();
    for (uint32_t i = 0; i < size; ++i) {
        const FieldDescriptorArr& messages = _message_generators.at(i);
        if (messages._vec.empty() || messages._name.find("Entry_DoNotUse") != std::string::npos) {
            continue;
        }
        printer.Print("\n");
        // class X {
        printer.Print("class $className$ \{\n", "className", messages._name);

        // declare
        printDeclare(printer, messages);
        // construction
        printConstruction(printer, messages);
        // get、set、has
        printGetSetHas(printer, messages);
        // serialize
        printSerialize(printer, messages);

        printer.Print("};\n");
    }
}

void CodeSerialize::printDeclare(google::protobuf::io::Printer& printer,
                                 const FieldDescriptorArr& messages) const {
    std::string fields;
    uint32_t message_size = (uint32_t)messages._vec.size();
    for (uint32_t idx = 0, flag = 0; idx < message_size; ++idx) {
        if (const FieldDescriptor* field = messages._vec.at(idx)) {
            if (field->is_map()) {
                printer.Print("    std::map<$type$>", "type", map2string(*field));
            } else if (field->is_repeated()) {
                printer.Print("    std::vector<$type$>", "type", type2string(*field));
            } else {
                printer.Print("    $type$", "type", type2string(*field));
            }
            printer.Print(" $name$_;\n", "name", FieldName(*field));
            // has declare
            if (!field->is_map() && !field->is_repeated()) {
                printer.Print("    bool has_$name$_;\n", "name", FieldName(*field));
            }
        }
    }
}

void CodeSerialize::printConstruction(google::protobuf::io::Printer& printer,
                                      const FieldDescriptorArr& messages) const {
    printer.Print("public:\n    $className$\()", "className", messages._name);
    // init fields
    printInitFields(printer, messages);
    printer.Print("\n");
}

void CodeSerialize::printGetSetHas(google::protobuf::io::Printer& printer,
                                   const FieldDescriptorArr& messages) const {
    uint32_t message_size = (uint32_t)messages._vec.size();
    for (uint32_t idx = 0, flag = 0; idx < message_size; ++idx) {
        if (const FieldDescriptor* field = messages._vec.at(idx)) {
            std::string field_name(FieldName(*field));
            std::string field_type;
            if (field->is_map()) {
                field_type.append("std::map<").append(map2string(*field)).append(">");
            } else if (field->is_repeated()) {
                field_type.append("std::vector<").append(type2string(*field)).append(">");
            } else {
                field_type.append(type2string(*field));
            }
            // function get
            printer.Print("    const $type$& get_$name$() const { return $name$_; }\n", "type",
                          field_type, "name", field_name);
            // function set
            if (field->is_map() || field->is_repeated() ||
                field->type() == FieldDescriptor::TYPE_MESSAGE) {
                printer.Print("    $type$* mutable_$name$() { ", "type", field_type, "name",
                              field_name);
                if (!field->is_map() && !field->is_repeated()) {
                    printer.Print("has_$name$_ = true; ", "name", field_name);
                }
                printer.Print("return &$name$_; }\n", "name", field_name);
            } else {
                printer.Print("    void set_$name$(const $type$& value) { has_$name$_ = true; "
                              "$name$_ = value; }\n",
                              "name", field_name, "type", field_type);
            }
            // function has
            printer.Print("    bool has_$name$() const { return ", "name", field_name);
            if (field->is_map() || field->is_repeated()) {
                printer.Print("(!$name$_.empty()); }\n", "name", field_name);
            } else {
                printer.Print("has_$name$_; }\n", "name", field_name);
            }
        }
    }
    printer.Print("\n");
}

void CodeSerialize::printInitFields(google::protobuf::io::Printer& printer,
                                    const FieldDescriptorArr& messages) const {
    uint32_t message_size = (uint32_t)messages._vec.size();
    std::string delimiter(":");
    for (uint32_t idx = 0, flag = 0; idx < message_size; ++idx) {
        if (const FieldDescriptor* field = messages._vec.at(idx)) {
            if ((!field->is_map() && !field->is_repeated() &&
                 field->type() != FieldDescriptor::TYPE_STRING &&
                 field->type() != FieldDescriptor::TYPE_BYTES &&
                 field->type() != FieldDescriptor::TYPE_MESSAGE)) {
                printer.Print("$delimiter$ ", "delimiter", delimiter);
                delimiter = ",";
                printer.Print("$fieldName$_(", "fieldName", FieldName(*field));
                if (field->has_default_value()) {
                    printer.Print("$value$", "value", type2value(*field));
                }
                printer.Print(")");
            }

            if (!field->is_map() && !field->is_repeated()) {
                printer.Print("$delimiter$ has_$fieldName$_(false)", "delimiter", delimiter,
                              "fieldName", FieldName(*field));
                delimiter = ",";
            }
        }
    }
    printer.Print(" {}\n");
}

void CodeSerialize::printSerialize(google::protobuf::io::Printer& printer,
                                   const FieldDescriptorArr& messages) const {
    printer.Print("    template <typename Archive>\n    void serialize(Archive& "
                  "archive) {\n        archive",
                  "nameSpace", _file->package(), "className", messages._name);
    uint32_t message_size = (uint32_t)messages._vec.size();
    for (uint32_t idx = 0; idx < message_size; ++idx) {
        if (const FieldDescriptor* field = messages._vec.at(idx)) {
            char sz[20] = {0};
            snprintf(sz, 20, "%d", field->number());
            const std::string& strOrgName = field->name();
            std::string fieldName(FieldName(*field));
            if (field->is_map()) {
                printer.Print(
                    " & MAKE_FIELD(\"$name$\", $number$, $type$, $field$_, NULL, $type2$)", "name",
                    fieldName, "number", sz, "type", getFieldType(*field->message_type()->field(0)),
                    "field", fieldName, "type2", getFieldType(*field->message_type()->field(1)));
            } else if (field->is_repeated()) {
                printer.Print(" & MAKE_FIELD(\"$name$\", $number$, $type$, $field$_, NULL", "name",
                              fieldName, "number", sz, "type", getFieldType(*field), "field",
                              fieldName);
                if (field->is_packed()) {
                    printer.Print(", true");
                }
                printer.Print(")");
            } else {
                printer.Print(" & MAKE_FIELD(\"$name$\", $number$, $type$, $field$_, &has_$name$_)",
                              "name", fieldName, "number", sz, "type", getFieldType(*field),
                              "field", fieldName);
            }
        }
    }
    printer.Print(";\n    }\n");
}

bool CodeSerialize::hasInt(google::protobuf::io::Printer& printer) const {
    uint32_t size = (uint32_t)_message_generators.size();
    for (uint32_t i = 0; i < size; ++i) {
        const FieldDescriptorArr& messages = _message_generators.at(i);
        uint32_t message_size = (uint32_t)messages._vec.size();
        for (uint32_t idx = 0; idx < message_size; ++idx) {
            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                FieldDescriptor::Type type = field->type();
                if (type == FieldDescriptor::TYPE_INT64 || type == FieldDescriptor::TYPE_UINT64 ||
                    type == FieldDescriptor::TYPE_INT32 || type == FieldDescriptor::TYPE_FIXED64 ||
                    type == FieldDescriptor::TYPE_FIXED32 || type == FieldDescriptor::TYPE_UINT32 ||
                    type == FieldDescriptor::TYPE_ENUM || type == FieldDescriptor::TYPE_SFIXED32 ||
                    type == FieldDescriptor::TYPE_SFIXED64 ||
                    type == FieldDescriptor::TYPE_SINT32 || type == FieldDescriptor::TYPE_SINT64) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool CodeSerialize::hasString(google::protobuf::io::Printer& printer) const {
    uint32_t size = (uint32_t)_message_generators.size();
    for (uint32_t i = 0; i < size; ++i) {
        const FieldDescriptorArr& messages = _message_generators.at(i);
        uint32_t message_size = (uint32_t)messages._vec.size();
        for (uint32_t idx = 0; idx < message_size; ++idx) {
            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                if (field->type() == FieldDescriptor::TYPE_STRING ||
                    field->type() == FieldDescriptor::TYPE_BYTES) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool CodeSerialize::hasVector(google::protobuf::io::Printer& printer) const {
    uint32_t size = (uint32_t)_message_generators.size();
    for (uint32_t i = 0; i < size; ++i) {
        const FieldDescriptorArr& messages = _message_generators.at(i);
        uint32_t message_size = (uint32_t)messages._vec.size();
        for (uint32_t idx = 0; idx < message_size; ++idx) {
            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                if (field->is_repeated()) {
                    return true;
                }
            }
        }
    }
    return false;
}
bool CodeSerialize::hasMap(google::protobuf::io::Printer& printer) const {
    uint32_t size = (uint32_t)_message_generators.size();
    for (uint32_t i = 0; i < size; ++i) {
        const FieldDescriptorArr& messages = _message_generators.at(i);
        uint32_t message_size = (uint32_t)messages._vec.size();
        for (uint32_t idx = 0; idx < message_size; ++idx) {
            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                if (field->is_map()) {
                    return true;
                }
            }
        }
    }
    return false;
}

std::string CodeSerialize::FieldName(const FieldDescriptor& field) {
    string result = field.name();
    LowerString(&result);
    if (kKeywords.count(result) > 0) {
        result.append("_");
    }
    return result;
}
}// namespace cpp
}// namespace compiler
}// namespace protobuf
}// namespace google
