#include <iostream>
#include <cstring>
#include <fstream>
#include <limits>

using namespace std;

#define MAX_PRODUCTS 100
#define MAX_CART 100
#define MAX_ORDERS 100
#define MAX_NAME_LEN 50
#define MAX_PAYMENT_METHOD_LEN 20


class InputException {
public:
    const char* what() const { return "Invalid input! Please try again."; }
};


int getStrictIntInput(const char* prompt) {
    int value;
    cout << prompt;
    cin >> value;
    if (cin.fail()) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw InputException();
    }
    if (cin.rdbuf()->in_avail() > 1 || cin.peek() != '\n') {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw InputException();
    }
    return value;
}

int getStrictMenuChoice(const char* prompt, int min, int max) {
    int choice;
    cout << prompt;
    cin >> choice;
    if (cin.fail() || choice < min || choice > max) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw InputException();
    }
    if (cin.rdbuf()->in_avail() > 1 || cin.peek() != '\n') {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        throw InputException();
    }
    return choice;
}

char getStrictYNInput(const char* prompt) {
    char input;
    cout << prompt;
    cin >> input;
    if (cin.fail() || (input != 'Y' && input != 'y' && input != 'N' && input != 'n')) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    if (cin.rdbuf()->in_avail() > 1 || cin.peek() != '\n') {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
    return input;
}


class Product {
public:
    int id;
    char name[MAX_NAME_LEN];
    int price;

    Product() : id(0), price(0) { name[0] = '\0'; }
    Product(int pid, const char* pname, int pprice) : id(pid), price(pprice) {
        strncpy(name, pname, MAX_NAME_LEN - 1);
        name[MAX_NAME_LEN - 1] = '\0';
    }
};


class CartItem {
public:
    int productId;
    char name[MAX_NAME_LEN];
    int price;
    int quantity;

    CartItem() : productId(0), price(0), quantity(0) { name[0] = '\0'; }
};


class Order {
public:
    int orderId;
    char paymentMethod[MAX_PAYMENT_METHOD_LEN];
    CartItem items[MAX_CART];
    int itemCount;
    int totalAmount;

    Order() : orderId(0), itemCount(0), totalAmount(0) { paymentMethod[0] = '\0'; }
};


class PaymentStrategy {
public:
    virtual void pay(int amount) = 0;
    virtual ~PaymentStrategy() {}
};

class CashPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Paid " << amount << " using Cash." << endl;
    }
};

class CardPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Paid " << amount << " using Credit / Debit Card." << endl;
    }
};

class GCashPayment : public PaymentStrategy {
public:
    void pay(int amount) override {
        cout << "Paid " << amount << " using GCash." << endl;
    }
};


class OrderManager {
private:
    static OrderManager* instance;
    Order orders[MAX_ORDERS];
    int orderCount;
    static int lastOrderId;

    OrderManager() : orderCount(0) {}

public:
    static OrderManager* getInstance() {
        if (!instance) {
            instance = new OrderManager();
        }
        return instance;
    }

    static void initialize(int startOrderId = 1) {
        lastOrderId = startOrderId;
    }

    int getNextOrderId() {
        return lastOrderId++;
    }

    void saveOrder(Order order) {
        if (orderCount < MAX_ORDERS) {
            order.orderId = getNextOrderId();
            orders[orderCount++] = order;
            ofstream log("orders.txt", ios::app);
            log << "[LOG] -> Order ID: " << order.orderId << " has been successfully checked out and paid using " << order.paymentMethod << ".\n";
            log.close();
        } else {
            cout << "Warning: Maximum number of orders reached.\n";
        }
    }

    void viewOrders() {
        if (orderCount == 0) {
            cout << "\nNo orders have been placed yet.\n";
            return;
        }

        for (int i = 0; i < orderCount; i++) {
            const Order& o = orders[i];
            cout << "Order ID: " << o.orderId << endl;
            cout << "Total Amount: " << o.totalAmount << endl;
            cout << "Payment Method: " << o.paymentMethod << endl;
            cout << "Order Details:" << endl;
            cout.width(15);
            cout.left;
            cout << "Product ID";
            cout.width(20);
            cout.left;
            cout << "Name";
            cout.width(10);
            cout.right;
            cout << "Price";
            cout.width(10);
            cout.right;
            cout << "Quantity" << endl;
            for (int j = 0; j < o.itemCount; j++) {
                cout.width(15);
                cout.left;
                cout << o.items[j].productId;
                cout.width(20);
                cout.left;
                cout << o.items[j].name;
                cout.width(10);
                cout.right;
                cout << o.items[j].price;
                cout.width(10);
                cout.right;
                cout << o.items[j].quantity << endl;
            }
            cout << endl;
        }
    }

    OrderManager(const OrderManager&) = delete;
    OrderManager& operator=(const OrderManager&) = delete;
    ~OrderManager() {}
};

OrderManager* OrderManager::instance = nullptr;
int OrderManager::lastOrderId = 1;


Product products[MAX_PRODUCTS] = {
    Product(1, "Laptop", 1200),
    Product(2, "Mouse", 25),
    Product(3, "Keyboard", 75),
    Product(4, "Monitor", 300)
};
int productCount = 4;


CartItem cart[MAX_CART];
int cartCount = 0;


void displayProducts() {
    cout << "Available Products:" << endl;
    cout.width(10);
    cout.left;
    cout << "Product ID";
    cout.width(15);
    cout.left;
    cout << "Name";
    cout.width(10);
    cout.right;
    cout << "Price" << endl;
    for (int i = 0; i < productCount; i++) {
        cout.width(10);
        cout.left;
        cout << products[i].id;
        cout.width(15);
        cout.left;
        cout << products[i].name;
        cout.width(10);
        cout.right;
        cout << products[i].price << endl;
    }
    cout << endl;

    bool addingMore = true;
    while (addingMore) {
        try {
            int productId;
            bool validId = false;
            while (!validId) {
                productId = getStrictIntInput("Enter Product ID to add to cart (0 to go back): ");
                if (productId == 0) {
                    addingMore = false;
                    break;
                }
                for (int i = 0; i < productCount; ++i) {
                    if (products[i].id == productId) {
                        validId = true;
                        break;
                    }
                }
                if (!validId && addingMore) {
                    cout << "Invalid Product ID. Please enter a valid ID from the list.\n";
                }
            }

            if (addingMore && validId) {
                int quantity;
                bool validQuantity = false;
                while (!validQuantity) {
                    try {
                        quantity = getStrictIntInput("Enter quantity: ");
                        validQuantity = true;
                        for (int i = 0; i < productCount; ++i) {
                            if (products[i].id == productId) {
                                if (cartCount < MAX_CART) {
                                    cart[cartCount].productId = products[i].id;
                                    strncpy(cart[cartCount].name, products[i].name, MAX_NAME_LEN - 1);
                                    cart[cartCount].name[MAX_NAME_LEN - 1] = '\0';
                                    cart[cartCount].price = products[i].price;
                                    cart[cartCount].quantity = quantity;
                                    cartCount++;
                                    cout << "Product added successfully!\n";
                                } else {
                                    cout << "Shopping cart is full!\n";
                                }
                                break;
                            }
                        }
                    } catch (InputException& e) {
                        cout << e.what() << endl;
                    }
                }
                if (addingMore) {
                    char addAnother = getStrictYNInput("Add another product? (Y/N): ");
                    if (addAnother != 'Y' && addAnother != 'y') {
                        addingMore = false;
                    }
                }
            }
        } catch (InputException& e) {
            cout << e.what() << endl;
        }
    }
}


void viewCart() {
    if (cartCount == 0) {
        cout << "\nYour Shopping Cart is empty.\n";
        return;
    }

    cout << "Your Shopping Cart:" << endl;
    cout.width(15);
    cout.left;
    cout << "Product ID";
    cout.width(20);
    cout.left;
    cout << "Name";
    cout.width(10);
    cout.right;
    cout << "Price";
    cout.width(10);
    cout.right;
    cout << "Quantity" << endl;

    int total = 0;
    for (int i = 0; i < cartCount; i++) {
        cout.width(15);
        cout.left;
        cout << cart[i].productId;
        cout.width(20);
        cout.left;
        cout << cart[i].name;
        cout.width(10);
        cout.right;
        cout << cart[i].price;
        cout.width(10);
        cout.right;
        cout << cart[i].quantity << endl;
        total += cart[i].price * cart[i].quantity;
    }
    cout << "\nTotal Amount: " << total << endl;

    bool gettingCheckoutChoice = true;
    char checkoutChoice = 'n';
    while (gettingCheckoutChoice) {
        try {
            checkoutChoice = getStrictYNInput("Do you want to check out? (Y/N): ");
            gettingCheckoutChoice = false;
        } catch (InputException& e) {
            cout << e.what() << endl;
        }
    }

    if (checkoutChoice == 'Y' || checkoutChoice == 'y') {
        if (cartCount > 0) {
            int method;
            bool gettingMethod = true;
            while (gettingMethod) {
                try {
                    cout << "\nSelect Payment Method:\n1. Cash\n2. Card\n3. GCash\n";
                    method = getStrictMenuChoice("Enter choice: ", 1, 3);
                    gettingMethod = false;
                } catch (InputException& e) {
                    cout << e.what() << endl;
                }
            }

            PaymentStrategy* strategy = nullptr;
            char methodStr[MAX_PAYMENT_METHOD_LEN];

            switch (method) {
                case 1: strategy = new CashPayment(); strncpy(methodStr, "Cash", MAX_PAYMENT_METHOD_LEN - 1); break;
                case 2: strategy = new CardPayment(); strncpy(methodStr, "Credit / Debit Card", MAX_PAYMENT_METHOD_LEN - 1); break;
                case 3: strategy = new GCashPayment(); strncpy(methodStr, "GCash", MAX_PAYMENT_METHOD_LEN - 1); break;
                default:
                    cout << "Error: Invalid payment method.\n";
                    return;
            }
            methodStr[MAX_PAYMENT_METHOD_LEN - 1] = '\0';

            strategy->pay(total);

            Order order;
            order.orderId = OrderManager::getInstance()->getNextOrderId();
            strncpy(order.paymentMethod, methodStr, MAX_PAYMENT_METHOD_LEN - 1);
            order.paymentMethod[MAX_PAYMENT_METHOD_LEN - 1] = '\0';
            order.itemCount = cartCount;
            order.totalAmount = total;
            for (int i = 0; i < cartCount; i++) {
                order.items[i] = cart[i];
            }

            OrderManager::getInstance()->saveOrder(order);
            cout << "\nYou have successfully checked out the products!\n";

            cartCount = 0;
            delete strategy;
        } else {
            cout << "Your cart is empty. Nothing to checkout.\n";
        }
    }
}

int main() {
    OrderManager::initialize();
    bool running = true;
    while (running) {
        try {
            cout << "\nE-Commerce System Menu:\n";
            cout << "1. View Products\n";
            cout << "2. View Shopping Cart\n";
            cout << "3. View Orders\n";
            cout << "4. Exit\n";
            int choice = getStrictMenuChoice("Enter your choice: ", 1, 4);

            switch (choice) {
                case 1:
                    displayProducts();
                    break;
                case 2:
                    viewCart();
                    break;
                case 3:
                    OrderManager::getInstance()->viewOrders();
                    break;
                case 4:
                    running = false;
                    cout << "Exiting the E-Commerce System. Thank you!\n";
                    break;
                default:
                    cout << "Invalid choice. Please try again.\n";
            }
        } catch (InputException& e) {
            cout << e.what() << endl;
        }
    }
    return 0;
}