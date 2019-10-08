# oatpp-mongocxx

**oatpp-mongocxx** is a object mapper for libmongocxx. It converts oatpp's DTOs to libmongcxx documents and vice versa

More about oat++:
- Website: [https://oatpp.io](https://oatpp.io)
- oatpp DTO's: [https://oatpp.io/docs/components/dto/](DTO)
## Requires

libmongocxx installed.

### Example

```c++
/**
 * DTO.hpp
 * See https://oatpp.io/docs/components/dto/
 */
class UserDto : public oatpp::data::mapping::type::Object {

 DTO_INIT(UserDto, Object)
 DTO_FIELD(String, name);
 DTO_FIELD(Int64, id);

};

/**
 * Database.hpp
 */
// [...]

class Database : public oatpp::base::Countable {
private:
    std::shared_ptr<oatpp::parser::mongocxx::mapping::ObjectMapper> m_mongomapper;
    mongocxx::client m_client;
public:
    Database() : m_mongomapper(oatpp::parser::mongocxx::mapping::ObjectMapper::createShared()) {
      // How ever you create your mongocxx::client and mongocxx::instance
    }

    int Database::insertUser(UserDto::ObjectWrapper &user) {
        auto mongouser = m_mongomapper->writeAsDocument(user);
        auto result = m_client["test"]["user"].insert_one(mongouser.view());
        if(result) {
            OATPP_LOGD(TAG, "Inserted new user '%s' (%llu) with id %s", user->name->c_str(), user->id->getValue(), result->inserted_id().get_oid().value.to_string().c_str());
            return 0;
        }
        return -1;
    }
}
```