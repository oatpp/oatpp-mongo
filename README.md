# oatpp-mongo 

---
**NOTE:**  

- BSON ObjectMapper - ready-to-use.
- Database driver - **in development**. While you can do basic CRUD operations, it's still on POC stage. API is not ready and it's not recommended to use.
To work with MongoDB - use BSON ObjectMapper + mongocxx driver.
---

**oatpp-mongo** is the oatpp native client for MongoDB. It contains DTO to BSON mapper plus database driver.

More about Oat++:

- [Oat++ Website](https://oatpp.io/)
- [Oat++ Github Repository](https://github.com/oatpp/oatpp)

## How To Build

`oatpp-mongo` has no extrernal dependencies (*The main oatpp module is still required*).  
`libmongoxcc` is used (and linked) in module **tests only**. Use `-DOATPP_BUILD_TESTS=OFF` option to build without tests and without dependency on `libmongoxcc`.

### Install oatpp-mongo

- Clone this repository. 
- In the root of the repository run:

   ```bash
   mkdir build && cd build
   cmake -DOATPP_BUILD_TESTS=OFF ..
   make install
   ```

