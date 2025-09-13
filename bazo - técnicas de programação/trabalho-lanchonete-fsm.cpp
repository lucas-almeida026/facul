#include <iostream>
#include <string>
#include <vector>
#include <limits>

using namespace std;

// Data structures
struct Item {
    string name;
    uint price_in_cents;
};

struct AppState {
    string customerName;
    vector<Item> order;
};

// Application states
enum class State {
    WELCOME,
    MAIN_MENU,
    LANCHES_MENU,
    BEBIDAS_MENU,
    SOBREMESAS_MENU,
    ORDER_SUMMARY,
    EXIT
};

// Menu data
const vector<Item> lanches = {
    {"Hambúrguer", 1000},
    {"Cheeseburguer", 1200},
    {"X-Bacon", 1500},
};

const vector<Item> bebidas = {
    {"Refrigerante", 500},
    {"Suco", 700},
    {"Água", 300},
};

const vector<Item> sobremesas = {
    {"Sorvete", 800},
    {"Bolo", 600},
    {"Pudim", 500},
};

// Constants
const uint DISCOUNT_THRESHOLD_CENTS = 5000;
const double DISCOUNT_RATE = 0.1;

// Utility functions
string formatPrice(uint priceInCents) {
    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%.2f", priceInCents / 100.0);
    return string(buffer);
}

string itemFormatedPrice(const Item& item) {
    return formatPrice(item.price_in_cents);
}

vector<string> itemsAsOptions(const vector<Item>& items) {
    vector<string> options;
    for (const auto& item : items) {
        options.push_back(item.name + " - R$" + itemFormatedPrice(item));
    }
    return options;
}

uint calcSubtotal(const vector<Item>& items) {
    uint total = 0;
    for (const auto& item : items) {
        total += item.price_in_cents;
    }
    return total;
}

uint calcTotal(const vector<Item>& items) {
    uint subtotal = calcSubtotal(items);
    if (subtotal >= DISCOUNT_THRESHOLD_CENTS) {
        return subtotal * (1.0 - DISCOUNT_RATE);
    }
    return subtotal;
}

void printOptions(const vector<string>& options) {
    for (size_t i = 0; i < options.size(); i++) {
        cout << i + 1 << ". " << options[i] << endl;
    }
}

bool isValidChoice(uint choice, size_t maxOptions) {
    return choice > 0 && choice <= maxOptions;
}

void clearInputError() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

uint readValidChoice(size_t maxOptions) {
    uint choice;
    cin >> choice;
    
    while (!isValidChoice(choice, maxOptions)) {
        if (cin.fail()) {
            clearInputError();
            cout << "Entrada inválida. Por favor, insira um número: ";
        } else {
            cout << "Opção inválida. Tente novamente: ";
        }
        cin >> choice;
    }
    return choice;
}

uint getChoiceSync(const vector<string>& options, const string& prompt, const string& header = "") {
    if (!header.empty()) {
        cout << header << endl;
    }
    cout << endl;
    printOptions(options);
    cout << prompt << "(1-" << options.size() << "): ";
    return readValidChoice(options.size());
}

uint getChoiceOrGoBackSync(const vector<Item>& items, const string& prompt) {
    vector<string> options = itemsAsOptions(items);
    options.push_back("Voltar");
    uint choice = getChoiceSync(options, prompt);
    if (choice == options.size()) {
        return 0; // represents go back option
    }
    return choice;
}

// State handlers
State handleWelcomeState(AppState& appState) {
    cout << "Bem-vindo à lanchonete!" << endl;
    cout << "Por favor, insira seu nome: ";
    cin >> appState.customerName;
    cout << "Olá, " << appState.customerName << "! Vamos começar seu pedido." << endl;
    return State::MAIN_MENU;
}

State handleMainMenuState(AppState& appState) {
    vector<string> menuOptions = {
        "Lanches",
        "Bebidas", 
        "Sobremesas",
        "Finalizar pedido"
    };
    
    uint choice = getChoiceSync(menuOptions, "Escolha uma categoria, ou finalize o pedido ");
    
    switch (choice) {
        case 1:
            return State::LANCHES_MENU;
        case 2:
            return State::BEBIDAS_MENU;
        case 3:
            return State::SOBREMESAS_MENU;
        case 4:
            return State::ORDER_SUMMARY;
        default:
            cout << "Opção inválida. Tente novamente." << endl;
            return State::MAIN_MENU;
    }
}

State handleItemMenuState(AppState& appState, const vector<Item>& items, const string& categoryName, State currentState) {
    uint itemChoice = getChoiceOrGoBackSync(items, "Escolha um " + categoryName + " ");
    
    if (itemChoice == 0) {
        return State::MAIN_MENU;
    }
    
    const Item& selectedItem = items[itemChoice - 1];
    appState.order.push_back(selectedItem);
    cout << selectedItem.name << " adicionado ao pedido." << endl;
    
    return currentState; // Stay in the same category state
}

State handleLanchesMenuState(AppState& appState) {
    return handleItemMenuState(appState, lanches, "lanche", State::LANCHES_MENU);
}

State handleBebidasMenuState(AppState& appState) {
    return handleItemMenuState(appState, bebidas, "bebida", State::BEBIDAS_MENU);
}

State handleSobremesasMenuState(AppState& appState) {
    return handleItemMenuState(appState, sobremesas, "sobremesa", State::SOBREMESAS_MENU);
}

State handleOrderSummaryState(AppState& appState) {
    if (appState.order.empty()) {
        cout << "Nenhum item foi pedido. Até logo, " << appState.customerName << "!" << endl;
        return State::EXIT;
    }
    
    cout << endl;
    cout << "Obrigado pelo seu pedido, " << appState.customerName << "!" << endl;
    cout << "Resumo do pedido:" << endl;
    
    for (const auto& item : appState.order) {
        cout << "- " << item.name << ": R$" << itemFormatedPrice(item) << endl;
    }
    
    uint subtotal = calcSubtotal(appState.order);
    uint total = calcTotal(appState.order);
    
    cout << "Subtotal: R$" << formatPrice(subtotal) << endl;
    
    if (total < subtotal) {
        cout << "Desconto aplicado! Novo total: R$" << formatPrice(total) << endl;
    } else {
        cout << "Total: R$" << formatPrice(total) << endl;
    }
    
    cout << "Volte sempre!" << endl;
    return State::EXIT;
}

// State machine dispatcher
State executeState(State currentState, AppState& appState) {
    switch (currentState) {
        case State::WELCOME:
            return handleWelcomeState(appState);
        case State::MAIN_MENU:
            return handleMainMenuState(appState);
        case State::LANCHES_MENU:
            return handleLanchesMenuState(appState);
        case State::BEBIDAS_MENU:
            return handleBebidasMenuState(appState);
        case State::SOBREMESAS_MENU:
            return handleSobremesasMenuState(appState);
        case State::ORDER_SUMMARY:
            return handleOrderSummaryState(appState);
        case State::EXIT:
            return State::EXIT;
        default:
            cout << "Estado inválido!" << endl;
            return State::EXIT;
    }
}

// Main function with state machine
int main() {
    AppState appState;
    State currentState = State::WELCOME;
    
    while (currentState != State::EXIT) {
        currentState = executeState(currentState, appState);
    }
    
    return 0;
}
