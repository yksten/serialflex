#include <assert.h>
#include <iostream>

#include <serialflex/json/decoder.h>
#include <serialflex/json/encoder.h>
#include <serialflex/serialize.h>

#include <serialflex/xml/encoder.h>
#include <serialflex/xml/decoder.h>

#include <serialflex/protobuf/encoder.h>
#include <serialflex/protobuf/decoder.h>


enum EnumType {
    ET1 = 1,
    ET2,
    ET3,
};

class data {
    bool has_ve;
    std::vector<EnumType> ve;
    
public:
    data() : has_ve(false) {}
    void set_ve(const std::vector<EnumType>& v) {
        ve = v;
        has_ve = true;
    }
    const std::vector<EnumType>& get_ve() const {
        return ve;
    }
    template<class Archive>
    void serialize(Archive& archive) {
        // t.convert("ve", ve, &has_ve);
        archive & MAKE_FIELD("ve", 1, serialflex::protobuf::FIELDTYPE_ENUM, ve, &has_ve);
    }
};


struct Item {
    std::string name;
    double price;
    int quantity;

    template<class Archive>
    void serialize(Archive& archive) {
        archive.convert("name", name).convert("price", price).convert("quantity", quantity);
    }
};

struct Inventory {
    int shopId;
    std::string shopName;
    std::vector<Item> items; // 这就是我们要序列化的数组
    std::vector<int> items2; // 这就是我们要序列化的数组
    std::map<int, int> map;
    std::map<int, Item> map2;

    template<class Archive>
    void serialize(Archive & archive) {
        archive.convert("shopId", shopId).convert("shopName", shopName).convert("items", items).convert("items2", items2);
        archive.convert("map", map).convert("map2", map2);
    }
};

int main(int argc, char* argv[]) {
    data d, pb_d;
    std::vector<EnumType> v;
    v.push_back(ET1);
    v.push_back(ET3);
    v.push_back(ET1);
    d.set_ve(v);
    std::string str_pb;
    bool encode_status_pb = serialflex::ProtobufEncoder(str_pb) << d;
    assert(encode_status_pb);
    bool decode_status_pb = serialflex::ProtobufDecoder((const uint8_t*)str_pb.data(), (uint32_t)str_pb.size()) >> pb_d;
    assert(decode_status_pb);

    
    std::string str_json;
    bool encode_status_json = serialflex::JSONEncoder(str_json) << d;
    assert(encode_status_json);
    std::cout << "json is :" << str_json.c_str() << std::endl;
    data d_json;
    bool decode_status_json = serialflex::JSONDecoder(str_json.c_str()) >> d_json;
    assert(decode_status_json);
    
    
    std::string str_xml;
    bool encode_xml_status = serialflex::XMLEncoder(str_xml) << d;
    assert(encode_xml_status);
    std::cout << "xml is :" << str_xml.c_str() << std::endl;
    data d_xml;
    bool decode_status_xml = serialflex::XMLDecoder(str_xml.c_str()) >> d_xml;
    assert(decode_status_xml);
    
    
    

    Inventory myInventory, my_inventory;
    myInventory.shopId = 9001;
    myInventory.shopName = "Cereal Supermart";
    myInventory.shopName = "![CDATA[...]]";
    
    Item it;
    it.name = "Sword < of C++";
    it.price = 99.99;
    it.quantity = 1;
    myInventory.items.push_back(it);
    myInventory.map2[1] = it;
    it.name = "Shield of Templates";
    it.price = 40.95;
    it.quantity = 5;
    myInventory.items.push_back(it);
    myInventory.map2[2] = it;
    it.name = "Potion of Polymorphism";
    it.price = 15;
    it.quantity = 20;
    myInventory.items.push_back(it);
    myInventory.map2[3] = it;
    
    myInventory.items2.push_back(1);
    myInventory.items2.push_back(2);
    myInventory.items2.push_back(3);
    
    myInventory.map[1] = 11;
    myInventory.map[2] = 22;
    myInventory.map[3] = 33;
    
    std::string str_xml1;
    bool encode_xml_status1 = serialflex::XMLEncoder(str_xml1, false) << myInventory;
    assert(encode_xml_status1);
    std::cout << "xml is :" << str_xml1.c_str() << std::endl;
    
    
    bool decode_xml_status1 = serialflex::XMLDecoder(str_xml1.c_str()) >> my_inventory;
    assert(decode_xml_status1);


    return 0;
}
