#include <iostream>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/stdx/optional.hpp>

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::finalize;

int main() {
    // Initialize the MongoDB C++ driver
    mongocxx::instance instance{};

    // Connect to a MongoDB server
    mongocxx::client client{mongocxx::uri{"mongodb://localhost:27017"}};

    // Get a handle to the "mydatabase" database and "users" collection
    auto db = client["mydatabase"];
    auto collection = db["users"];

    // Insert a new document
    auto result = collection.insert_one(document{}
        << "name" << "John Doe"
        << "email" << "john.doe@example.com"
        << "age" << 30
        << finalize);
    std::cout << "Inserted a document into the users collection\n";

    // Get the ID of the inserted document
    bsoncxx::stdx::optional<bsoncxx::types::bson_value::view> id = result->inserted_id();
    if (id) {
        // Find a document by ID
        auto cursor = collection.find_one(document{} << "_id" << *id << finalize);
        if (cursor) {
            std::cout << "Found the following document:\n"
                << bsoncxx::to_json(*cursor) << "\n";

            // Update a document by ID
            auto result = collection.update_one(document{} << "_id" << *id << finalize,
                document{} << "$set" << open_document << "name" << "Jane Doe" << close_document << finalize);
            std::cout << "Updated the document\n";

            // Delete a document by ID
            auto result2 = collection.delete_one(document{} << "_id" << *id << finalize);
            std::cout << "Deleted the document\n";
        } else {
            std::cout << "Cannot find the document\n";
        }
    } else {
        std::cout << "Cannot get the ID of the inserted document\n";
    }

    // Close the connection to the MongoDB server
    client.close();

    return 0;
}
