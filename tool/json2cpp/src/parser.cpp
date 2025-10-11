#include "parser.h"
#include <cctype>
#include "reader.h"

namespace json2cpp {

const char* GLOBAL_PREFIX = "CXX_";
class StructDetails {
    std::string name_;
    const custom::GenericValue* value_;

public:
    enum ITEMTYPE { TYPE_NULL = 0, TYPE_STRING = 1, TYPE_VECTOR = 2, TYPE_MAP = 4 };

    StructDetails(const std::string& name, const custom::GenericValue* value)
        : name_(name), value_(value) {
        if (!name_.empty()) {
            name_.front() = std::toupper(name_.front());
        }
    }

    std::string toText() const {
        // class name
        std::string str_result("class ");
        str_result.append(GLOBAL_PREFIX).append(name_).append(" {\n");
        // field
        for (const custom::GenericValue* item = value_->child; item; item = item->next) {
            str_result.append("    ").append(StructDetails::toTypeByItem(*item, true));
            std::string field_name = StructDetails::getFieldName(*item);
            str_result.append(1, ' ').append(field_name).append(1, ';');
            str_result.append(1, '\n');
            // has field
            str_result.append("    bool has_").append(field_name).append("_;");
            str_result.append(1, '\n');
        }
        // public
        str_result.append("public:\n");
        // constructor
        str_result.append("    ").append(GLOBAL_PREFIX).append(name_).append("() ");
        std::string str_constructor;
        for (const custom::GenericValue* item = value_->child; item; item = item->next) {
            if (item->type == custom::GenericValue::VALUE_BOOL) {
                if (!str_constructor.empty()) {
                    str_constructor.append(", ");
                }
                str_constructor.append(item->key, item->key_size).append("_(false)");
            } else if (item->type == custom::GenericValue::VALUE_NUMBER) {
                if (!str_constructor.empty()) {
                    str_constructor.append(", ");
                }
                str_constructor.append(item->key, item->key_size).append("_(0)");
            }
        }
        if (!str_constructor.empty()) {
            str_result.append(": ").append(str_constructor);
        }
        str_result.append(" {}\n");
        // set/get function
        for (const custom::GenericValue* item = value_->child; item; item = item->next) {
            // type
            const std::string field_type = StructDetails::toTypeByItem(*item, true);
            // name
            std::string field_name = StructDetails::getFieldName(*item);
            // set_field
            str_result.append("\n    ")
                .append("void set_")
                .append(field_name)
                .append("(const ")
                .append(field_type)
                .append("& value) { ")
                .append(field_name)
                .append("_ = value; ")
                .append("has_")
                .append(field_name)
                .append("_ = true; }");
            // get_field
            str_result.append("\n    ")
                .append("const ")
                .append(field_type)
                .append("& get_")
                .append(field_name)
                .append("() const { return ")
                .append(field_name)
                .append("_; }");
        }
        // serialize
        str_result.append("\n    template<typename Archive>");
        str_result.append("\n    void serialize(Archive& ar) {\n        ar");
        for (const custom::GenericValue* item = value_->child; item; item = item->next) {
            std::string field_name = StructDetails::getFieldName(*item);
            str_result.append(".convert(\"")
                .append(field_name)
                .append("\", ")
                .append(field_name)
                .append("_, &has_")
                .append(field_name)
                .append("_)");
        }
        str_result.append(";\n    }\n");

        str_result.append("};");

        return str_result;
    }

    int32_t hasType() const {
        int32_t type = TYPE_NULL;
        for (const custom::GenericValue* item = value_->child; item; item = item->next) {
            if (item->type == custom::GenericValue::VALUE_STRING) {
                type |= TYPE_STRING;
            } else if (item->type == custom::GenericValue::VALUE_ARRAY) {
                type |= TYPE_VECTOR;
            }
        }
        return type;
    }

private:
    static std::string toTypeByItem(const custom::GenericValue& item, const bool is_child) {
        std::string str_result;
        if (item.type == custom::GenericValue::VALUE_BOOL) {
            str_result.append("bool");
        } else if (item.type == custom::GenericValue::VALUE_NUMBER) {
            bool is_float = false;
            for (uint32_t idx = 0; idx < item.value_size; ++idx) {
                if (item.value[idx] == '.') {
                    is_float = true;
                    break;
                }
            }
            if (is_float) {
                // double db = custom::GenericReader::convertDouble(item.value,
                // item.valueSize);
                str_result.append("float");
            } else {
                // int64_t value = custom::GenericReader::convertInt(item.value,
                // item.valueSize);
                str_result.append("int32_t");
            }
        } else if (item.type == custom::GenericValue::VALUE_STRING) {
            str_result.append("std::string");
        } else if (item.type == custom::GenericValue::VALUE_ARRAY) {
            if (item.child) {
                str_result.append("std::vector<")
                    .append(toTypeByItem(*item.child, false))
                    .append(">");
            }
        } else if (item.type == custom::GenericValue::VALUE_OBJECT) {
            std::string field_name = StructDetails::getFieldName(item, is_child);
            if (!field_name.empty()) {
                field_name.front() = std::toupper(field_name.front());
            }
            str_result.append(GLOBAL_PREFIX).append(field_name);
        }
        return str_result;
    }

    static std::string getFieldName(const custom::GenericValue& item, const bool is_child = true) {
        std::string field_name;
        if (!item.key) {
            const custom::GenericValue* item_prev = item.prev;
            field_name.append(item_prev->key, item_prev->key_size);
            if (is_child) {
                field_name.append("child");
            }
        } else {
            field_name.append(item.key, item.key_size);
        }
        return field_name;
    }
};

/*----------------------------------------------------------------------------------------------------------------*/

class CppDescription {
    int32_t has_string_vector_map_;
    std::vector<StructDetails> vec_;

public:
    CppDescription(): has_string_vector_map_(StructDetails::TYPE_NULL) {}
    void push_front(const StructDetails& item) {
        has_string_vector_map_ |= item.hasType();
        vec_.insert(vec_.begin(), item);
    }

    void push_back(const StructDetails& item) {
        has_string_vector_map_ |= item.hasType();
        vec_.push_back(item);
    }

    std::string toText() const {
        std::string str_result("#include <stdint.h>");
        if ((has_string_vector_map_ & StructDetails::TYPE_STRING) == StructDetails::TYPE_STRING) {
            str_result.append("\n#include <string>");
        }
        if ((has_string_vector_map_ & StructDetails::TYPE_VECTOR) == StructDetails::TYPE_VECTOR) {
            str_result.append("\n#include <vector>");
        }
        if ((has_string_vector_map_ & StructDetails::TYPE_MAP) == StructDetails::TYPE_MAP) {
            str_result.append("\n#include <map>");
        }
        str_result.append("\n\n");
        for (const StructDetails& item : vec_) {
            str_result.append(1, '\n');
            str_result.append(item.toText());
            str_result.append(1, '\n');
        }
        return str_result;
    }

    static void getDescriptionChild(const custom::GenericValue* root,
                                    CppDescription& descriptions) {
        if (!root) {
            return;
        }
        for (const custom::GenericValue* item = root->child; item; item = item->next) {
            if (item->type == custom::GenericValue::VALUE_OBJECT) {
                getDescription(item, descriptions);
            } else if (item->type == custom::GenericValue::VALUE_ARRAY && item->child &&
                       item->child->type == custom::GenericValue::VALUE_OBJECT) {
                getDescription(item->child, descriptions);
            }
        }
    }

    static void getDescription(const custom::GenericValue* root, CppDescription& descriptions) {
        for (const custom::GenericValue* item = root->child; item; item = item->next) {
            const custom::GenericValue* temp_item = NULL;
            const char* key = NULL;
            uint32_t keySize = 0;
            if (item->type == custom::GenericValue::VALUE_OBJECT) {
                temp_item = item;
                key = item->key;
                keySize = item->key_size;
            } else if (item->type == custom::GenericValue::VALUE_ARRAY && item->child &&
                       item->child->type == custom::GenericValue::VALUE_OBJECT) {
                temp_item = item->child;
                key = item->key;
                keySize = item->key_size;
            } else {
                continue;
            }
            // 1.child
            getDescriptionChild(temp_item, descriptions);
            // 2.current
            descriptions.push_back(StructDetails(std::string(key, keySize), temp_item));
        }
        if (root->key_size) {
            descriptions.push_back(StructDetails(std::string(root->key, root->key_size), root));
        } else if (root->prev) {
            descriptions.push_back(
                StructDetails(std::string(root->prev->key, root->prev->key_size), root));
        }
    }
};

/*----------------------------------------------------------------------------------------------------------------*/

std::string buildStruct(const char* json) {
    custom::Reader reader;
    const custom::GenericValue* root = reader.parse(json);
    if (!root) {
        return std::string(reader.getError());
    }
    CppDescription descriptions;
    CppDescription::getDescription(root, descriptions);
    descriptions.push_back(StructDetails("Result", root));

    return descriptions.toText();
}

}// namespace json2cpp
