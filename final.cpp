#include <iostream>
#include <string>
#include <cstdlib>
#include <sstream>
#include <stdexcept>
#define _WIN32_WINNT 0x0A00
#include "httplib.h"

using namespace std;

int id[100], quantity[100], price[100];
string name[100];
int productCount = 0;

int orderId[100], orderProductId[100], orderQuantity[100];
string orderStatus[100];
int orderCount = 0;
int nextOrderId = 1001;

int findProduct(int searchId) {
    for (int i = 0; i < productCount; i++) {
        if (id[i] == searchId)
            return i;
    }
    return -1;
}

int getNumber(const httplib::Request& req, const string& key) {
    if (!req.has_param(key))
        return -1;

    try {
        return stoi(req.get_param_value(key));
    }
    catch (...) {
        return -1;
    }
}

string escapeHTML(const string& value) {
    string result;
    for (char c : value) {
        if (c == '&') result += "&amp;";
        else if (c == '<') result += "&lt;";
        else if (c == '>') result += "&gt;";
        else if (c == '"') result += "&quot;";
        else result += c;
    }
    return result;
}

void addProduct(const httplib::Request& req,
                httplib::Response& res) {

    string productName = req.get_param_value("name");
    int productPrice = getNumber(req, "price");
    int productQuantity = getNumber(req, "quantity");

    if (productName.empty() || productPrice <= 0 ||
        productQuantity < 0) {
        res.status = 400;
        res.set_content("Please enter valid product details.",
                        "text/plain");
        return;
    }

    if (productCount >= 100) {
        res.status = 400;
        res.set_content("Product storage is full.",
                        "text/plain");
        return;
    }

    id[productCount] = 101 + productCount;
    name[productCount] = productName;
    price[productCount] = productPrice;
    quantity[productCount] = productQuantity;

    int newId = id[productCount];
    productCount++;

    res.set_content(
        "Product added successfully! Product ID: " +
        to_string(newId),
        "text/plain"
    );
}


void viewProducts(const httplib::Request& req,
                  httplib::Response& res) {

    string output =
        "<h3>Available Products</h3>"
        "<table><tr><th>ID</th><th>Name</th>"
        "<th>Price</th><th>Quantity</th></tr>";

    for (int i = 0; i < productCount; i++) {
        output += "<tr><td>" + to_string(id[i]) +
            "</td><td>" + escapeHTML(name[i]) +
            "</td><td>Rs. " + to_string(price[i]) +
            "</td><td>" + to_string(quantity[i]) +
            "</td></tr>";
    }

    output += "</table>";

    if (productCount == 0) {
        output = "<p>No products available. Add a product first.</p>";
    }

    res.set_content(output, "text/html");
}


void searchProduct(const httplib::Request& req,
                   httplib::Response& res) {
    int searchId = getNumber(req, "id");
    int index = findProduct(searchId);

    if (index == -1) {
        res.status = 404;
        res.set_content("Product ID not found.",
                        "text/plain");
        return;
    }

    string output =
        "<h3>Product Found</h3>"
        "<p>Product ID: " + to_string(id[index]) + "</p>" +
        "<p>Name: " + escapeHTML(name[index]) + "</p>" +
        "<p>Price: Rs. " + to_string(price[index]) + "</p>" +
        "<p>Quantity: " + to_string(quantity[index]) + "</p>";

    res.set_content(output, "text/html");
}

void updateQuantity(const httplib::Request& req,
                    httplib::Response& res) {
    int searchId = getNumber(req, "id");
    int newQuantity = getNumber(req, "quantity");

    int index = findProduct(searchId);

    if (index == -1) {
        res.status = 404;
        res.set_content("Product ID not found.",
                        "text/plain");
        return;
    }

    if (newQuantity < 0) {
        res.status = 400;
        res.set_content("Enter a valid quantity.",
                        "text/plain");
        return;
    }

    quantity[index] = newQuantity;

    res.set_content("Product quantity updated successfully!",
                    "text/plain");
}

void placeOrder(const httplib::Request& req,
                httplib::Response& res) {
    int searchId = getNumber(req, "id");
    int orderQty = getNumber(req, "quantity");

    int index = findProduct(searchId);

    if (index == -1) {
        res.status = 404;
        res.set_content("Product ID not found.",
                        "text/plain");
        return;
    }

    if (orderQty <= 0) {
        res.status = 400;
        res.set_content("Enter a valid order quantity.",
                        "text/plain");
        return;
    }

    if (orderQty > quantity[index]) {
        res.status = 400;
        res.set_content("Insufficient stock available.",
                        "text/plain");
        return;
    }

    if (orderCount >= 100) {
        res.status = 400;
        res.set_content("Order storage is full.",
                        "text/plain");
        return;
    }

    quantity[index] -= orderQty;

    orderId[orderCount] = nextOrderId;
    orderProductId[orderCount] = searchId;
    orderQuantity[orderCount] = orderQty;
    orderStatus[orderCount] = "Placed";

    int currentOrderId = nextOrderId;
    nextOrderId++;
    orderCount++;

    res.set_content(
        "Order placed successfully! Order ID: " +
        to_string(currentOrderId) +
        "\nRemaining Stock: " + to_string(quantity[index]),
        "text/plain"
    );
}


void trackOrders(const httplib::Request& req,
                 httplib::Response& res) {

    string output =
        "<h3>Order History</h3>"
        "<table><tr><th>Order ID</th>"
        "<th>Product ID</th><th>Quantity</th>"
        "<th>Status</th></tr>";

    for (int i = 0; i < orderCount; i++) {
        output += "<tr><td>" + to_string(orderId[i]) +
            "</td><td>" + to_string(orderProductId[i]) +
            "</td><td>" + to_string(orderQuantity[i]) +
            "</td><td>" + escapeHTML(orderStatus[i]) +
            "</td></tr>";
    }

    output += "</table>";

    if (orderCount == 0) {
        output = "<p>No orders have been placed yet.</p>";
    }

    res.set_content(output, "text/html");
}


int main() {
    httplib::Server server;

    server.Post("/api/add", addProduct);
    server.Get("/api/view", viewProducts);
    server.Post("/api/search", searchProduct);
    server.Post("/api/update", updateQuantity);
    server.Post("/api/order", placeOrder);
    server.Get("/api/track", trackOrders);

    server.Get("/api/health",
        [](const httplib::Request& req,
           httplib::Response& res) {
            res.set_content("C++ Server Connected!",
                            "text/plain");
        });

    server.set_mount_point("/", "./public");

    const char* portValue = getenv("PORT");
    int port = portValue ? atoi(portValue) : 8080;

    cout << "Inventory Server running on port "
         << port << endl;

    server.listen("0.0.0.0", port);

    return 0;
}
